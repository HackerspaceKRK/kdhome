#ifndef __ETHERNET_OUTPUT_PROVIDER_H__
#define __ETHERNET_OUTPUT_PROVIDER_H__

#include "../EthernetDevice.h"
#include "IOutputProvider.h"

class OutputProvider : public IOutputProvider
{
public:
	OutputProvider(EthernetDevice* device, int amount);
	virtual ~OutputProvider()
	{
		logger->logInfo("Deleting OutputProvider");
	}
	
	// IProvider
	void init() { }
	void deinit() { }
	void processData(const void* buffer, int len);
	void process();
	EthernetDevice* getDevice()
	{
		return m_device;
	}
	
	// IOutputProvider
	int getAmount()
	{
		return m_outputs.size();
	}
	bool getOutputState(int num)
	{
		return m_outputs[num];
	}
	void setOutputState(int num, bool on);
	void toggleOutputState(int num);
	
	string getOutputID(int num);

private:
	EthernetDevice *m_device;
	
	bool m_hasInitialStates;
	vector<char> m_outputs;
	
	uint32_t m_lastUpdateTime;
	
	void update();
	void preparePacket(TSrvHeader* packet, uint8_t command);
	void sendData(const void* data, int len)
	{
		m_device->sendData(data, len);
	}

	void logInfo(const string& msg);
};

#endif
