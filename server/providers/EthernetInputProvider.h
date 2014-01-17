#ifndef __ETHERNET_INPUT_PROVIDER_H__
#define __ETHERNET_INPUT_PROVIDER_H__

#include "../EthernetDevice.h"
#include "IInputProvider.h"

class EthernetInputProvider : public IInputProvider
{
public:
	EthernetInputProvider(EthernetDevice* device, int amount);
	virtual ~EthernetInputProvider() { }

	// IProvider
	// uint16_t getType() { return 0x0002; }
	void init();
	void deinit() { }
	void processData(ByteBuffer& buffer);
	void process();
	EthernetDevice* getDevice() { return m_device; }

	// IInputProvider
	int getAmount() { return m_inputs.size(); }
	int getInputState(int num) { return m_inputs[num].high > m_inputs[num].low; }
	void setListener(IInputProviderListener* listener) { m_listener = listener; }

private:
	struct TInput
	{
		uint8_t low, high;

		TInput() : low(0), high(0) { }
	};

	EthernetDevice *m_device;
	IInputProviderListener *m_listener;

	bool m_hasFirstData;
	vector<TInput> m_inputs;

	uint32_t m_lastUpdateTime;

	void update();
	void prepareCommand(ByteBuffer& buffer, uint8_t command);
	void sendData(ByteBuffer& buffer) { m_device->sendData(buffer); }
};

#endif