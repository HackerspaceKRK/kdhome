#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <UdpSocket.h>
#include <kdutils.h>
#include <LinuxTimer.h>

#include <stdint.h>
#include <string>
using namespace std;

#include "Providers.h"
#include "EthernetDevice.h"
#include "providers/OutputProvider.h"
#include "providers/InputProvider.h"
#include "providers/IRProvider.h"
#include "providers/TempProvider.h"
#include "providers/CounterProvider.h"

#include <zmq.h>

class MyLogger;
class lua_State;
class StorageEngine;

template<typename T>
struct TProviderEntry
{
	T *provider;
	int startNumber;
};

class IProviderValue
{
public:
	string id, name;
	uint32_t lastChange;
	bool initialized;
};

template<typename T>
class TProviderValue : public IProviderValue
{
public:
	virtual ~TProviderValue() { };
	T value;

	void setValue(const T& value)
	{
		this->value = value;
		this->initialized = true;
		this->lastChange = getTicks();
	}
};

typedef TProviderValue<bool> TOutputProviderValue;
typedef TProviderValue<bool> TInputProviderValue;
typedef TProviderValue<float> TTempProviderValue;
typedef TProviderValue<uint32_t> TCounterProviderValue;

class Controller : public IInputProviderListener,
	public IEthernetDataListener,
	public IIRProviderListener,
	public IOutputProviderListener,
	public ICounterProviderListener
{
public:
	Controller();
	~Controller();
	
	void setLogger(MyLogger* logger)
	{
		m_logger = logger;
	}
	void setUserLogger(MyLogger* logger)
	{
		m_userLogger = logger;
	}
	bool init();
	void reload();
	void run();
	void savePersistentState(const string& id);
	
	void updateNames();
	
	template<typename T>
	bool findProvider(const string& id, EthernetDevice*& dev, T*& provider, int& num);
	EthernetDevice* findDevice(const string& name);
	
	// Expander
	// void freezeExpander() { m_expander->freeze(); }
	// void unfreezeExpander() { m_expander->unfreeze(); }
	
	int getTime()
	{
		return getTicks() - m_startTime;
	}
	
	// Devices
	int registerEthernetDevice(uint32_t id, const string& ip, uint16_t port, const string& name);
	// void addOutputProvider(int dev, int outputsCount);
	// void addInputProvider(int dev, int inputsCount);
	// void addIRProvider(int dev);
	// void addTempProvider(int dev, int sensorsCount);
	
	// Inputs
	bool getInput(const string& name);
	
	// Outputs
	void setOutput(const string& name, bool state);
	bool getOutput(const string& name);
	void toggleOutput(const string& name);
	void setOutputAsPersistent(const string& name);
	
	// Temperature
	bool isTempValid(const string& name);
	float getTemp(const string& name);
	
	// Handlers
	bool onExternalCommand(const vector<string>& parts, string& res);
	void onIRCode(uint32_t code);
	void onIRButtonPressed(uint32_t code);
	void onIRButtonReleased(uint32_t code);
	
	StorageEngine* getStorage()
	{
		return m_storage;
	}
	
	// IEthernetDataListener
	void onEthernetDataReceived(const string& ip, const void* buffer, int len);
	
	// IInputProviderListener
	void onInputInitState(IInputProvider* provider, const string& id, int state);
	void onInputChanged(IInputProvider* provider, const string& id, int state);
	
	// IIRProviderListener
	void onIRCodeReceived(IIRProvider* provider, uint32_t code);
	void onIRButtonPressed(IIRProvider* provider, uint32_t code);
	void onIRButtonReleased(IIRProvider* provider, uint32_t code);
	
	// IOutputProviderListener
	void onInitialStatesRequest(IOutputProvider* provider);
	
	// ICounterProviderListener
	void onCounterChanged(ICounterProvider* provider, const string& id, uint32_t value);
	
private:
	uint32_t m_lastTicks;
	uint32_t m_startTime;
	
	LinuxTimer m_checkTimer;
	UdpSocket m_server;
	MyLogger *m_logger, *m_userLogger;
	StorageEngine *m_storage;
	
	vector<EthernetDevice*> m_devices;
	
	// map<string, string> m_inputsNames, m_outputsNames, m_tempsNames, m_countersNames;
	// map<string, string> m_inputNameToId, m_outputNameToId, m_tempNameToId, m_counterNameToId;
	
	// map<string, int> m_inputValues, m_outputValues;
	map<string, int> m_persistentOutputs;
	
	vector<TInputProviderValue> m_inputs;
	vector<TOutputProviderValue> m_outputs;
	vector<TTempProviderValue> m_temps;
	vector<TCounterProviderValue> m_counters;
	
	template<typename T>
	TProviderValue<T>& findValue(const string& id, vector<TProviderValue<T>>& container);

	// zmq
	uint32_t m_sessKey;
	void *zcontext, *zsocket, *zsocketREP;
	bool m_initialized;
	
	void publish(string msg);
	string processREQ(string msg);
	
	void setOutputInProvider(const string& id, bool state);
};

template<typename T>
TProviderValue<T>& Controller::findValue(const string& id, vector<TProviderValue<T>>& container)
{
	for (auto & v : container)
		if (v.id == id || v.name == id)
			return v;
	TProviderValue<T> val;
	val.id = id;
	val.name = id;
	val.initialized = false;
	container.push_back(val);
	return findValue(id, container);
}

#endif
