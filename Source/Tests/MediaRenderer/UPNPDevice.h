#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "Get_local.h"
#include "Tee3Client.h"

#include "PltDeviceHost.h"
#include "PltUPnP.h"
#include "Neptune.h"
#include "PltService.h"



void MQTT_JoinRoom(header::signal_header& header);
void MQTT_LeaveRoom();
void HandleMessage(void* msg,int size);
void cl_Callback(void* context, char* cause);
int Message(void* context, char* topicName, int topicLen, MQTTClient_message* message);
void delivery(void* context, MQTTClient_deliveryToken dt);
void HandleInvite(header::signal_header& header);
void* RunToMQTT(void *);

class upnpDevice;
class MyThread :public NPT_Thread{

public:
	MyThread(upnpDevice* m) :m_device(m), m_running(true), m_interrupt(false) { ; }
	~MyThread(){ ; }
	virtual void Run();
	void quit();
	void setDuration(int milsc);
	void setInterrput(bool interrput = true){ m_interrupt = interrput; }
	bool IsRunning(){return m_interrupt;}
private:
	upnpDevice* m_device;
	NPT_TimeInterval m_duration;
	bool m_running;
	bool m_interrupt;
};


class upnpDevice : public PLT_DeviceHost{
public:
	upnpDevice(const char*  friendly_name,
                      bool         show_ip = false,
                      const char*  uuid = NULL,
                      unsigned int port = 0,
                      bool         port_rebind = false);
	~upnpDevice();
	virtual NPT_Result SetupServices();
	virtual Result  OnAction(PLT_ActionReference &action, const PLT_HttpRequestContext &context, NPT_HttpResponse&  response);
	void SetDEVparameter(cJSON* root, cJSON* item, NPT_HttpResponse&  response);
	void RejectResponse(NPT_HttpResponse&  response);
	void ResponseParameters(string type, NPT_HttpResponse&  response);
	void ResponseDevStatus(NPT_HttpResponse&  response);
	void Updatetimer();
	void DisconnectControl();
	bool StartTimer(int ms);             
	void StopTimer();
public:
	char 				Local_IP[32];
	char				Local_MAC[20];
	std::string 			m_uuid;
	std::string 			m_name;	
	bool				IsControlled;
	bool 				IsDisconnect;
	MyThread* 			m_thread;
};
