#ifndef _DEPLOYFILE_H_
#define _DEPLOYFILE_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include "Tee3VideoCapturer.h"
#include "cJSON.h"
#include "signal_invite.pb.h"
#include "header.pb.h"

using namespace std;
using namespace tee3;
using namespace tee3::avd;

class DeplyPar{
	
public:
	bool loadProfileFile();
	bool SaveProInfo();
	void PrintProInfo();
	void SendMqttInfo(string data, header::signal_group_type group_type, header::signal_cmd_type cmd_type);
	void Tee3_SetCamera();
public:
	static DeplyPar* Instance();
	
	int GetPduId() const {return pduId;}
	tee3::String GetTee3Addr() const {return Tee3_url;}
	string GetMqttFromId() const {return MQTT_FromId;}
	string GetMqttAddress() const {return MqttAddress;}
	string GetClientname() const {return ClientName!=""?ClientName:"NewBox";}
	string GetClientpasswd() const {return ClientPasswd;}
	string GetHDMI_1_name() const {return HDMI1_name;}
	string GetHDMI_2_name() const {return HDMI2_name;}
	string GetRoomid() const {return Tee3_RoomID;}
	string GetClienttopic() const {return Client_Topic;}
	tee3::avd::User* GetTee3User()  {return &Tee3_user;} 
	tee3::avd::Camera* GetfakeCamera_1()  {return &fakeCamera;}
	tee3::avd::Camera* GetfakeCamera_2()  {return &fakeCamera2;}	
	
	
	MQTTClient* GetMyclient()  {return &Myclient;} 
	MQTTClient_deliveryToken* GetMqtttoken()  {return &token;}
	MQTTClient_connectOptions* GetMqttconnectOpt()  {return &conn_opts;}
	MQTTClient_message* GetMqttMessage()  {return &pubmsg;}
	
	pthread_mutex_t* GetAudioInLock()  {return &AudioImportdatalock;}
	
	void SetPduId(int id);
	void SetTee3Addr(string url);
	void SetMqttFromId(string Fromid);
	void SetMqttAddress(string addr);
	void SetClienname(string clienname);
	void SetClienpasswd(string clienpasswd);
	void SetHDMI_1_nmae(string HDMI1);
	void SetHDMI_2_nmae(string HDMI2);
	void SetRoomid(string roomid);
	void SetTee3User(string userId,string userName);
	void SetfakeCamera_1(string id, string name);
	void SetfakeCamera_2(string id, string name);
	void SetMqttconnectOpt();
	void SelfcheckMQTT();
private:
	DeplyPar();
	~DeplyPar();
	
public:
	string 		AudioImportdata;
	bool		ISRebootDev;
	bool		IsStartMQTT;	//MQTT???????????????
	bool 		AudioIntSign; 	//????????????????????????
	bool 		RebootProcess;
	EncodedCaptureFromFile* m_pipe;
	EncodedCaptureFromFile* m_pipe2;

	int 		mqttconnectRes;
	int 		Tee3EngineRes;
	int 		JoinRoomRes;
	string		Tee3Roomid;
private:
	tee3::String Tee3_url;	//????????????????????????
	int pduId;
	std::string 	MqttAddress;	//MQTT???????????????
	std::string	ClientName; //?????????ID
	std::string	ClientPasswd;  //?????????passwd
	std::string 	Client_Topic; //?????????????????????
	std::string 	HDMI1_name;	//?????????HDMI
	std::string 	HDMI2_name;	//?????????HDM2
	std::string MQTT_FromId;//?????????ID
	MQTTClient Myclient;	//?????????
	MQTTClient_deliveryToken token;	//????????????????????????
	MQTTClient_connectOptions conn_opts;	//?????????????????????
	MQTTClient_message pubmsg;
	
	std::string Tee3_RoomID;	//???????????????
	tee3::avd::User Tee3_user;	//????????????
	tee3::avd::Camera fakeCamera;//HDMI1?????????
	tee3::avd::Camera fakeCamera2;//HDMI2?????????
	pthread_mutex_t AudioImportdatalock; //?????????????????????

};

#endif
