#include "InputProvider.h"

#include "common.h"
#include "kdutils.h"
#include <unistd.h>

#include <kdhome.h>

InputProvider::InputProvider(EthernetDevice* device, int amount)
	: m_device(device), m_listener(0), m_lastUpdateTime(0)
{
	for (int i = 0; i < amount; i++)
		m_inputs.push_back(TInput());
}

void InputProvider::init()
{
	m_hasFirstData = 0;
	for (int i = 0; i < getAmount(); i++)
		m_inputs[i] = TInput();
		
	TSrvHeader p;
	preparePacket((TSrvHeader*)&p, INPUT_REQ_SENDSTATE);
	sendData(&p, sizeof(p));
}
void InputProvider::processData(const void* buffer, int len)
{
	TProvHeader *header = (TProvHeader*)buffer;
	
	switch (header->cmd)
	{
	case INPUT_NOTF_NEWSTATE:
	{
		TProvInputStatePacket *p = (TProvInputStatePacket*)buffer;
		for (int i = 0; i < p->cnt; i++)
		{
			uint8_t low, high;
			low = p->inputs[i].low;
			high = p->inputs[i].high;
			
			TInput &inp = m_inputs[i];
			
			// printf("%d - %d %d\n", i, low, high);
			while (inp.low != low || inp.high != high)
			{
				if (inp.low != low)
				{
					inp.low++;
					inp.state = 0;
					if (m_listener)
					{
						if (m_hasFirstData)
							m_listener->onInputChanged(this, getInputID(i), 0);
						else
							m_listener->onInputInitState(this, getInputID(i), 0);
					}
				}
				if (inp.high != high)
				{
					inp.high++;
					inp.state = 1;
					if (m_listener)
					{
						if (m_hasFirstData)
							m_listener->onInputChanged(this, getInputID(i), 1);
						else
							m_listener->onInputInitState(this, getInputID(i), 1);
					}
				}
			}
			// printf("test: %d - %d %d\n", i, inp.low, inp.high);
		}
		m_hasFirstData = true;
	}
	break;
	}
}
void InputProvider::process()
{
}

void InputProvider::update()
{
}
void InputProvider::preparePacket(TSrvHeader* packet, uint8_t command)
{
	m_device->preparePacket(packet);
	packet->type = getType();
	packet->cmd = command;
}

string InputProvider::getInputID(int num)
{
	return str(Format("{}-{}") << getDevice()->getName() << num);
}

void InputProvider::logInfo(const string& msg)
{
	logger->logInfo(Format("[input] {}") << msg);
}
