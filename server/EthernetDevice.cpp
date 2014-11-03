#include "EthernetDevice.h"

#include "common.h"
#include "kdutils.h"

#include "providers/EthernetInputProvider.h"
#include "providers/EthernetOutputProvider.h"
#include "providers/EthernetIRProvider.h"
#include "providers/EthernetTempProvider.h"

EthernetDevice::EthernetDevice(UdpServer* server, uint32_t id, const string& ip, const string& name)
	: m_server(server), m_id(id), m_ip(ip), m_name(name), m_lastPacketTime(0), m_connected(false), m_lastRecvPacketId(0), m_sendPacketId(0),
	  m_sessKey(0), m_registrationDataSendTime(0), m_inputListener(0)
{
}
EthernetDevice::~EthernetDevice()
{
	for (auto& prov : m_providers)
		delete prov;
}

bool EthernetDevice::hasProvider(uint16_t type)
{
	for (size_t i = 0; i < m_providers.size(); i++)
		if (m_providers[i]->getType() == type)
			return true;
	return false;
}
IProvider* EthernetDevice::getProvider(uint16_t type)
{
	for (size_t i = 0; i < m_providers.size(); i++)
	{
		IProvider *provider = m_providers[i];
		if (provider->getType() == type)
		{
			return provider;
		}
	}
	return 0;
}

string EthernetDevice::getName() const
{
	return m_name;
}

void EthernetDevice::processData(ByteBuffer& buffer)
{
	m_lastPacketTime = getTicks();
	checkConnection();
	
	TProvHeader *header = (TProvHeader*)buffer.data();
	
	logInfo(str(Format("Packet #{} received - type: {} cmd: {} sessKey: {}") << header->packetId << header->type << (int)header->cmd << header->sessKey));
	
	if (m_connected)
	{
		if (m_sessKey == 0 || m_sessKey != header->sessKey)
		{
			markDisconnected();
			registerDevice();
			return;
		}
		
		if (header->packetId <= m_lastRecvPacketId && header->packetId - m_lastRecvPacketId > 10) // prevent disconnecting on wrap
		{
			markDisconnected();
			registerDevice();
			return;
		}
	}
	else // if not connected
	{
		if (m_sessKey == header->sessKey)
		{
			markConnected();
		}
		else
		{
			if (getTicks() - m_registrationDataSendTime > REGISTRATION_DATA_DELAY)
			{
				registerDevice();
			}
		}
	}
	
	if (m_connected)
	{
		m_lastRecvPacketId = header->packetId;
		
		if (header->type == 0)
		{
			logInfo(str(Format("cmd: {}") << (int)header->cmd));
		}
		else
		{
			if (header->cmd == 0x00)
			{
				switch(header->type)
				{
				case PROVIDER_TYPE_INPUT:
				{
					TProvInputRegisterPacket *p = (TProvInputRegisterPacket*)buffer.data();
					EthernetInputProvider *prov = new EthernetInputProvider(this, p->cnt);
					prov->setListener(m_inputListener);
					addProvider(prov);
					logInfo(str(Format("Added input provider to device #{} with {} inputs") << 0 << (int)p->cnt));
					break;
				}
				case PROVIDER_TYPE_OUTPUT:
				{
					TProvOutputRegisterPacket *p = (TProvOutputRegisterPacket*)buffer.data();
					EthernetOutputProvider *prov = new EthernetOutputProvider(this, p->cnt);
					addProvider(prov);
					logInfo(str(Format("Added output provider to device #{} with {} outputs") << 0 << (int)p->cnt));
					break;
				}
				case PROVIDER_TYPE_IR:
				{
					TProvIRRegisterPacket *p = (TProvIRRegisterPacket*)buffer.data();
					EthernetIRProvider *prov = new EthernetIRProvider(this);
					addProvider(prov);
					logInfo(str(Format("Added IR provider to device #{}") << 0));
					break;
				}
				case PROVIDER_TYPE_TEMP:
				{
					TProvTempRegisterPacket *p = (TProvTempRegisterPacket*)buffer.data();
					EthernetTempProvider *prov = new EthernetTempProvider(this, p->cnt);
					addProvider(prov);
					logInfo(str(Format("Added temp provider to device #{} with {} sensors") << 0 << (int)p->cnt));
					break;
				}
				}
				// logInfo(str(Format("Registering provider of type: {}") << (int)header->type));
			}
			else
			{
				for (size_t i = 0; i < m_providers.size(); i++)
				{
					IProvider *provider = m_providers[i];
					if (provider->getType() == header->type)
					{
						provider->processData(buffer);
						break;
					}
				}
			}
		}
	}
}
void EthernetDevice::process()
{
	checkConnection();
}
void EthernetDevice::checkConnection()
{
	uint32_t ticks = getTicks();
	if (m_connected)
	{
		if (ticks - m_lastPacketTime >= PACKETLOSS_DISCONNECT_DELAY)
		{
			markDisconnected();
		}
	}
	if (m_connected)
	{
		for (size_t i = 0; i < m_providers.size(); i++)
		{
			IProvider *provider = m_providers[i];
			provider->process();
		}
	}
}

void EthernetDevice::prepareBuffer(ByteBuffer& buffer)
{
	buffer.append(m_sendPacketId);
	
	m_sendPacketId++;
}
void EthernetDevice::preparePacket(TSrvHeader* packet)
{
	packet->packetId = m_sendPacketId;
	
	m_sendPacketId++;
}
void EthernetDevice::sendData(ByteBuffer& buffer)
{
	m_server->sendData(getIP(), buffer);
	logInfo(str(Format("Packet sent")));
}
void EthernetDevice::sendData(const void* data, int len)
{
	m_server->sendData(getIP(), data, len);
	logInfo(str(Format("Packet sent")));
}

void EthernetDevice::markDisconnected()
{
	m_connected = false;
	m_sessKey = 0;
	logInfo("Disconnected");
	
	for (size_t i = 0; i < m_providers.size(); i++)
	{
		IProvider *provider = m_providers[i];
		provider->deinit();
	}
}
void EthernetDevice::markConnected()
{
	m_connected = true;
	logInfo("Connected");
	m_lastRecvPacketId = 0;
	
	for (size_t i = 0; i < m_providers.size(); i++)
	{
		IProvider *provider = m_providers[i];
		provider->init();
	}
}
void EthernetDevice::registerDevice()
{
	m_sessKey = rand() % (0xffff - 1) + 1;
	m_lastRecvPacketId = 0;
	
	ByteBuffer buf;
	prepareBuffer(buf);
	
	uint16_t type = PROVIDER_TYPE_CONTROL;
	uint8_t cmd = CONTROL_CMD_REGISTER;
	buf.append(type);
	buf.append(cmd);
	buf.append(m_sessKey);
	logInfo(str(Format("Sending registration data... - sessKey: {}") << m_sessKey));
	sendData(buf);
	m_registrationDataSendTime = getTicks();
}

void EthernetDevice::logInfo(const string& msg)
{
	logger->logInfo(Format("[{}] {}") << getName() << msg);
}
