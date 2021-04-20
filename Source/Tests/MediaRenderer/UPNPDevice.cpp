#include "UPNPDevice.h"
#include "header.pb.cc"
#include "signal_invite.pb.cc"

pthread_t	StartMQTT_Num;	//MQTT线程号

NPT_SET_LOCAL_LOGGER("platinum.media.renderer")
extern NPT_UInt8 RDR_ConnectionManagerSCPD[];
extern NPT_UInt8 RDR_AVTransportSCPD[];
extern NPT_UInt8 RDR_RenderingControlSCPD[];
//receive

const std::string GETDEVICEINFO = "getdeviceinfo";
const std::string MQTTADR = "mqttAdr";
const std::string CONTROLLINGDEV= "controllingdev";
const std::string REBOOTDEV = "rebootdev";
const std::string BOXPARAMETERS = "boxparameters";
const std::string LEAVEMEETING = "leavemeeting";
const std::string DEVICESTATUS = "devicestatus";
const std::string ENDCONTROL = "Endcontrol";
/***************************MQTT协议**********************************************/
//volatile MQTTClient_deliveryToken deliveredtoken;
void MQTT_JoinRoom(header::signal_header& header)
{
	signal_invite::Invite pdu;
	pdu.ParseFromString(header.content());
	header::signal_header m_signalheader;
	signal_invite::InviteReject m_signalaccept;
	cJSON * pJson = cJSON_Parse(pdu.options().c_str());
	if (NULL == pJson)
	{
		return;
	}
	printf("%s: %d\n", pJson->string, pJson->valuestring);
	cJSON * pSub_Name = cJSON_GetObjectItem(pJson, "roomId");
	DeplyPar::Instance()->SetRoomid(pSub_Name->valuestring);
	cout<<"RoomID = "<<DeplyPar::Instance()->GetRoomid()<<endl;
	if(DeplyPar::Instance()->Tee3EngineRes != AVD_OK)
		MClient::Instance()->InitEngine();
	if(MClient::Instance()->InRoom == true)
	{	
		DeplyPar::Instance()->SendMqttInfo("Reject",header::SIGNALE_GROUP_TYPE_MESSAGE,header::SIGNALE_CMD_TYPE_INVITE_REJECT);
		printf("------------------------Device is in the room, Failed to join\n");
	}
	else{
		MClient::Instance()->JoinRoom(DeplyPar::Instance()->GetRoomid());
	}
	
}
void MQTT_LeaveRoom()
{
	MClient::Instance()->LeaveRoom();
}
void HandleInvite(header::signal_header& header)
{
	signal_invite::Invite pdu;
	switch (header.cmd_type()) {
	case header::SIGNALE_CMD_TYPE_INVITE:
		printf("this is INVITE\n");
		pdu.ParseFromString(header.content());
		DeplyPar::Instance()->SetPduId(pdu.id());
		MQTT_JoinRoom(header);
		break;
	case header::SIGNALE_CMD_TYPE_INVITE_CANCEL:
		printf("this is CANCEL\n");
		break;
	case header::SIGNALE_CMD_TYPE_INVITE_REJECT:
		printf("this is REJECT\n");
		break;
	case header::SIGNALE_CMD_TYPE_INVITE_OK:
		printf("this is OK\n");
		break;
	case header::SIGNALE_CMD_TYPE_INVITE_RINGING:
		printf("this is RINGING\n");
		break;
	case header::SIGNALE_CMD_TYPE_INVITE_BYE:
		printf("this is BYE\n");
		MQTT_LeaveRoom();
		break;
	case header::SIGNALE_CMD_TYPE_INVITE_TIMEOUT:
		printf("this is TIMEOUT\n");
		break;
	default:
		break;
	}
}
void HandleSelfcheck(header::signal_header& header)
{
	signal_invite::Invite pdu;
	switch (header.cmd_type()) {
	case header::SIGNALE_CMD_TYPE_MESSAGE_DATA:
		printf("this is MQTT Self-check\n");
		break;
	default:
		break;
	}
}
void HandleMessage(void* msg,int size)
{
	header::signal_header header;
	if (!header.ParseFromArray(msg, size)) {
		std::cout << "recv data: ParseFromString failed. size: " << size << std::endl;
		return;
	}
	DeplyPar::Instance()->SetMqttFromId(header.from());
	switch (header.group_type()) {
	case header::SIGNALE_GROUP_TYPE_MESSAGE://即时信息
		break;
	case header::SIGNALE_GROUP_TYPE_INVITE://邀请
		HandleInvite(header);
		break;
	case header::SIGNALE_GROUP_TYPE_ADDRESS_BOOK://通讯录
		break;
	case header::SIGNALE_GROUP_TYPE_REMOTE_CONTROL://远程控制
		break;
	case header::SIGNALE_GROUP_TYPE_RPC://RPC消息
		break;
	case header::SIGNALE_GROUP_TYPE_SYS://系统消息
		HandleSelfcheck(header);
		break;
	default:
		break;
	}
}
//当客户端断开连接时执行此函数
void cl_Callback(void* context, char* cause)
{
	printf("Connection disconnected positive\n");
	printf("return cause:%s\n", cause);
	pthread_cancel(StartMQTT_Num);
	if(pthread_create(&StartMQTT_Num,NULL,RunToMQTT,NULL) == 0){
		printf("Hisipreadth success,\n");
	}
}
//客户端接受订阅主题发出的信息
int Message(void* context, char* topicName, int topicLen, MQTTClient_message* message)
{
	printf("Message received!!!\n");
	printf("Topic is   %s\n", topicName);
	printf("message:\n");
	printf("    %s\n", message->payload);
	printf("\n");
	HandleMessage(message->payload,message->payloadlen);
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}
//当客户端发布消息时调用
void delivery(void* context, MQTTClient_deliveryToken dt){
	printf("MQTTClient_deliveryToken is %d\n", dt);
	//deliveredtoken = dt;
}

//放置MQTT阻塞程序，另开线程
void* RunToMQTT(void *)
{
	int re_connect;
	bool  OKStart = false;
	while(true){
		if(OKStart == true)
			break;
		printf("---------------RunToMQTT\n");
		DeplyPar::Instance()->SetMqttconnectOpt();
		MQTTClient_create(DeplyPar::Instance()->GetMyclient(),DeplyPar::Instance()->GetMqttAddress().c_str(), DeplyPar::Instance()->GetClientname().c_str(), MQTTCLIENT_PERSISTENCE_NONE,NULL);				
		MQTTClient_setCallbacks(*DeplyPar::Instance()->GetMyclient(), NULL, cl_Callback, Message, delivery);
		DeplyPar::Instance()->mqttconnectRes = MQTTClient_connect(*DeplyPar::Instance()->GetMyclient(), DeplyPar::Instance()->GetMqttconnectOpt());
		if (DeplyPar::Instance()->mqttconnectRes != MQTTCLIENT_SUCCESS)
		{
			printf("Connection failed,return:%d\n", DeplyPar::Instance()->mqttconnectRes);
			DeplyPar::Instance()->IsStartMQTT = false;
			OKStart = false;
			sleep(3);
		}else{
			printf("---------------Client_Name:%s\n",DeplyPar::Instance()->GetClientname().c_str());
			printf("---------------ClientPasswd:%s\n",DeplyPar::Instance()->GetClientpasswd().c_str());
			printf("---------------Connection success!!!\n");
			DeplyPar::Instance()->IsStartMQTT = true;
			OKStart = true;
			MQTTClient_subscribe(*DeplyPar::Instance()->GetMyclient(), DeplyPar::Instance()->GetClienttopic().c_str(), 1);
			DeplyPar::Instance()->SelfcheckMQTT();
		}	
		
	}
			
		
}

/*************************************************************************************/
//UPNP

void MyThread::Run(){
	NPT_LOG_INFO("------------------ begin thread ----------------------.");
	m_running = true;
	while (m_running){
		sleep(5);
		if (m_interrupt){ ////停止请求
			
		}
		else{
			printf("------------------ run ----------------------\n");
			if (m_running && m_device){
				printf("IsDisconnect:%d\n",m_device->IsDisconnect);
			    if(m_device->IsDisconnect){
				
				m_device->IsDisconnect=false;				
			     }
			     else{
				m_device->DisconnectControl();
			     }				
				
			}
		}
	}
}

void MyThread::quit(){
	m_running = false;
}
void MyThread::setDuration(int milsc){
	m_duration.SetMillis(milsc);
}
upnpDevice::upnpDevice(const char*  friendly_name,
                      bool         show_ip,
                      const char*  uuid,
                      unsigned int port,
                      bool         port_rebind):
		PLT_DeviceHost("/", 
                   uuid, 
                   "urn:schemas-upnp-org:device:MediaRenderer:1", 
                   friendly_name, 
                   show_ip, 
                   port, 
                   port_rebind){
		m_ModelDescription = "Plutinosoft AV Media Renderer Device";
    		m_ModelName        = "AV Renderer Device";
    		m_ModelURL         = "http://www.plutinosoft.com/platinum";
    		m_DlnaDoc          = "DMR-1.50";
		m_uuid		   = uuid;
		m_name		   = friendly_name;
		memset(Local_IP, 0, 32);
		get_local_ip(IF_NAME,Local_IP);
		get_local_mac(Local_MAC);
		IsControlled = false;
		IsDisconnect = false;
		m_thread = new MyThread(this);
}

upnpDevice::~upnpDevice(){}
NPT_Result upnpDevice::SetupServices(){
		NPT_Reference<PLT_Service> service;
    		{
        		/* AVTransport */
        		service = new PLT_Service(
          		this,
            		"urn:schemas-upnp-org:service:AVTransport:1", 
            		"urn:upnp-org:serviceId:AVTransport",
            		"AVTransport",
            		"urn:schemas-upnp-org:metadata-1-0/AVT/");
        		NPT_CHECK_FATAL(service->SetSCPDXML((const char*) RDR_AVTransportSCPD));
        		NPT_CHECK_FATAL(AddService(service.AsPointer()));
        		service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));
        		service->SetStateVariable("A_ARG_TYPE_InstanceID", "0"); 
        		// GetCurrentTransportActions
        		service->SetStateVariable("CurrentTransportActions", "Play,Pause,Stop,Seek,Next,Previous");
        		// GetDeviceCapabilities
        		service->SetStateVariable("PossiblePlaybackStorageMedia", "NONE,NETWORK,HDD,CD-DA,UNKNOWN");
        		service->SetStateVariable("PossibleRecordStorageMedia", "NOT_IMPLEMENTED");
        		service->SetStateVariable("PossibleRecordQualityModes", "NOT_IMPLEMENTED");
        		// GetMediaInfo
        		service->SetStateVariable("NumberOfTracks", "0");
        		service->SetStateVariable("CurrentMediaDuration", "00:00:00");
        		service->SetStateVariable("AVTransportURI", "");
        		service->SetStateVariable("AVTransportURIMetadata", "");;
        		service->SetStateVariable("NextAVTransportURI", "NOT_IMPLEMENTED");
        		service->SetStateVariable("NextAVTransportURIMetadata", "NOT_IMPLEMENTED");
        		service->SetStateVariable("PlaybackStorageMedium", "NONE");
        		service->SetStateVariable("RecordStorageMedium", "NOT_IMPLEMENTED");
        		service->SetStateVariable("RecordMediumWriteStatus", "NOT_IMPLEMENTED");
        		// GetPositionInfo
        		service->SetStateVariable("CurrentTrack", "0");
        		service->SetStateVariable("CurrentTrackDuration", "00:00:00");
        		service->SetStateVariable("CurrentTrackMetadata", "");
        		service->SetStateVariable("CurrentTrackURI", "");
        		service->SetStateVariable("RelativeTimePosition", "00:00:00"); 
        		service->SetStateVariable("AbsoluteTimePosition", "00:00:00");
        		service->SetStateVariable("RelativeCounterPosition", "2147483647"); // means NOT_IMPLEMENTED
        		service->SetStateVariable("AbsoluteCounterPosition", "2147483647"); // means NOT_IMPLEMENTED
        		// disable indirect eventing for certain state variables
        		PLT_StateVariable* var;
        		var = service->FindStateVariable("RelativeTimePosition");
        		if (var) var->DisableIndirectEventing();
       			var = service->FindStateVariable("AbsoluteTimePosition");
        		if (var) var->DisableIndirectEventing();
        		var = service->FindStateVariable("RelativeCounterPosition");
        		if (var) var->DisableIndirectEventing();
        		var = service->FindStateVariable("AbsoluteCounterPosition");
        		if (var) var->DisableIndirectEventing();
        		// GetTransportInfo
        		service->SetStateVariable("TransportState", "NO_MEDIA_PRESENT");
        		service->SetStateVariable("TransportStatus", "OK");
        		service->SetStateVariable("TransportPlaySpeed", "1");
        		// GetTransportSettings
        		service->SetStateVariable("CurrentPlayMode", "NORMAL");
        		service->SetStateVariable("CurrentRecordQualityMode", "NOT_IMPLEMENTED");
        		service.Detach();
        		service = NULL;
    		}
    		{
        		/* ConnectionManager */
        		service = new PLT_Service(
            		this,
            		"urn:schemas-upnp-org:service:ConnectionManager:1", 
            		"urn:upnp-org:serviceId:ConnectionManager",
            		"ConnectionManager");
        		NPT_CHECK_FATAL(service->SetSCPDXML((const char*) RDR_ConnectionManagerSCPD));
        		NPT_CHECK_FATAL(AddService(service.AsPointer()));
        		service->SetStateVariable("CurrentConnectionIDs", "0");
        		// put all supported mime types here instead
        		service->SetStateVariable("SinkProtocolInfo", "http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_SP_G726,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE,http-get:*:audio/L16;rate=44100;channels=1:DLNA.ORG_PN=LPCM,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO,http-get:*:audio/L16;rate=44100;channels=2:DLNA.ORG_PN=LPCM,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM,http-get:*:video/x-ms-asf:DLNA.ORG_PN=VC1_ASF_AP_L1_WMA,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMDRM_WMABASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPLL_BASE,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC_XAC3,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVSPLL_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_BASE,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_ASP_L5_SO_G726,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL_XAC3,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG1,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_ASP_L4_SO_G726,http-get:*:audio/L16;rate=48000;channels=2:DLNA.ORG_PN=LPCM,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3X,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_MP3,http-get:*:video/x-ms-wmv:*");
        		service->SetStateVariable("SourceProtocolInfo", "");
        		service.Detach();
        		service = NULL;
    		}
    		{
        		/* RenderingControl */
        		service = new PLT_Service(
            		this,
            		"urn:schemas-upnp-org:service:RenderingControl:1", 
            		"urn:upnp-org:serviceId:RenderingControl",
           	 	"RenderingControl",
            		"urn:schemas-upnp-org:metadata-1-0/RCS/");
        		NPT_CHECK_FATAL(service->SetSCPDXML((const char*) RDR_RenderingControlSCPD));
        		NPT_CHECK_FATAL(AddService(service.AsPointer()));
        		service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));
        		service->SetStateVariable("Mute", "0");
        		service->SetStateVariableExtraAttribute("Mute", "Channel", "Master");
        		service->SetStateVariable("Volume", "100");
        		service->SetStateVariableExtraAttribute("Volume", "Channel", "Master");
        		service->SetStateVariable("VolumeDB", "0");
        		service->SetStateVariableExtraAttribute("VolumeDB", "Channel", "Master");
        		service->SetStateVariable("PresetNameList", "FactoryDefaults");
        		service.Detach();
        		service = NULL;
    		}
    	return NPT_SUCCESS;
}
Result  upnpDevice::OnAction(PLT_ActionReference &action, const PLT_HttpRequestContext &context, NPT_HttpResponse&  response){
	NPT_String name = action->GetActionDesc().GetName();
	NPT_HttpHeader* heard = NULL;
	heard = context.GetRequest().GetHeaders().GetHeader("3teecustom_request");
	if(heard){
		//std::string Devdate = heard->GetValue();
		cJSON * root = NULL;
		cJSON * item = NULL;
		root = cJSON_Parse(heard->GetValue().GetChars());
		if (cJSON_GetArraySize(root) > 0){
			cJSON * item = cJSON_GetArrayItem(root, 0);
			
			if (item->string == GETDEVICEINFO){
				char buf[1024] = {0};
				sprintf(buf, "{\"deviceinfo\":\"tmc_device1\",\"DEVID\":\"%s\",\"IP\":\"%s\",\"MAC\":\"%s\",\"NAME\":\"%s\"}", m_uuid.c_str(), Local_IP, Local_MAC,m_name.c_str());
				printf("-----------------cmd: DevInfo\n");
				response.GetHeaders().SetHeader("3teecustom_respon", buf);
			}
			
				if(item->string == MQTTADR){
					SetDEVparameter(root, item, response);
				}
				else if(item->string == CONTROLLINGDEV){
					if(IsControlled){
						RejectResponse(response);
						return 0;
					}
					if(!m_thread->IsRunning()){
						StartTimer(5000);
					}
					else{
						m_thread->setInterrput(false);
					}
					Updatetimer();
					ResponseParameters(CONTROLLINGDEV, response);
				}
				else if(item->string == REBOOTDEV){
					//DeplyPar::Instance()->SaveProInfo();
					DeplyPar::Instance()->RebootProcess = true;
					DeplyPar::Instance()->ISRebootDev = true;
				}
				else if(item->string == BOXPARAMETERS){
					ResponseParameters(BOXPARAMETERS, response);
				}
				else if(item->string == LEAVEMEETING){
					if(MClient::Instance()->InRoom == true){
						MClient::Instance()->LeaveRoom();
						MClient::Instance()->InRoom = false;
					}
					string str = "{\"" + LEAVEMEETING + "\"\:0}";
					response.GetHeaders().SetHeader("3teecustom_respon", str.c_str());
				}
				else if(item->string == DEVICESTATUS){
					Updatetimer();
					ResponseDevStatus(response);
				}
				else if(item->string == ENDCONTROL){
					printf("End of control\n");
					StopTimer();
					IsControlled = false;
				}	
		}
	}
	return 1;
}
void upnpDevice::ResponseDevStatus(NPT_HttpResponse&  response)
{
	char buf[1024] = {0};
	sprintf(buf, "{\"devicestatus\":\"devicestatus\",\"mqttstatus\":%d,\"Tee3Engine\":%d,\"meetingstatus\":%d,\"Roomid\":\"%s\"}", DeplyPar::Instance()->mqttconnectRes,
	DeplyPar::Instance()->Tee3EngineRes,
	DeplyPar::Instance()->JoinRoomRes,
	DeplyPar::Instance()->GetRoomid().c_str());
	response.GetHeaders().SetHeader("3teecustom_respon", buf);
}
void upnpDevice::ResponseParameters(string requesttype, NPT_HttpResponse&  response)
{
	char buf[1024] = {0};
	sprintf(buf, "{\"%s\":0,\"mqttAdr\":\"%s\",\"deviceName\":\"%s\",\"devicePwd\":\"%s\",\"serverAdr\":\"%s\",\"HDMI1\":\"%s\",\"HDMI2\":\"%s\"}", requesttype.c_str(),
	DeplyPar::Instance()->GetMqttAddress().c_str(),
	DeplyPar::Instance()->GetClientname().c_str(),
	DeplyPar::Instance()->GetClientpasswd().c_str(),
	DeplyPar::Instance()->GetTee3Addr().c_str(),
	DeplyPar::Instance()->GetHDMI_1_name().c_str(),
	DeplyPar::Instance()->GetHDMI_2_name().c_str());
	response.GetHeaders().SetHeader("3teecustom_respon", buf);
}
void upnpDevice::RejectResponse(NPT_HttpResponse&  response)
{
	string str = "{\"" + CONTROLLINGDEV + "\"\:-1}";
	response.GetHeaders().SetHeader("3teecustom_respon", str.c_str());
}
void upnpDevice::SetDEVparameter(cJSON* root, cJSON* item, NPT_HttpResponse& response)
{
	
	cJSON* serverAdr = cJSON_GetObjectItem(root, "serverAdr");
	cJSON* normalSetting = cJSON_GetObjectItem(root, "normalSetting");
	cJSON* deviceName=cJSON_GetObjectItem(normalSetting,"deviceName");
	cJSON* devicePwd = cJSON_GetObjectItem(normalSetting,"devicePwd");
	cJSON* videoSetting = cJSON_GetObjectItem(root, "videoSetting");
	cJSON* HDMI1 = cJSON_GetObjectItem(videoSetting, "HDMI1");
	cJSON* HDMI2 = cJSON_GetObjectItem(videoSetting, "HDMI2");
	if(MClient::Instance()->InRoom == false){
		printf("------------IsStartMQTT= %d\n",DeplyPar::Instance()->IsStartMQTT);
		if(DeplyPar::Instance()->IsStartMQTT == true){
			MQTTClient_unsubscribe(*DeplyPar::Instance()->GetMyclient(), DeplyPar::Instance()->GetClienttopic().c_str());
			MQTTClient_disconnect(*DeplyPar::Instance()->GetMyclient(), 1000);
			MQTTClient_destroy(DeplyPar::Instance()->GetMyclient());//释放客户端
			pthread_cancel(StartMQTT_Num);
			printf("-------------------------DeleteMQTT\n");
			DeplyPar::Instance()->IsStartMQTT = false;
		}	
		DeplyPar::Instance()->SetMqttAddress(item->valuestring);
		DeplyPar::Instance()->SetClienname(deviceName->valuestring);
		DeplyPar::Instance()->SetClienpasswd(devicePwd->valuestring);
		printf("IsStartMQTT = %s\n",DeplyPar::Instance()->IsStartMQTT);
		if(pthread_create(&StartMQTT_Num,NULL,RunToMQTT,NULL) == 0){
			printf("Hisipreadth success,\n");
		}
					
		if(DeplyPar::Instance()->GetTee3Addr() != serverAdr->valuestring){
			DeplyPar::Instance()->SetTee3Addr(serverAdr->valuestring);
			if(MClient::Instance()->m_IsInitEngine == true){
				MClient::Instance()->UninitEngine();		
			}
			MClient::Instance()->InitEngine();
		}
		DeplyPar::Instance()->SetHDMI_1_nmae(HDMI1->valuestring);
		DeplyPar::Instance()->SetHDMI_2_nmae(HDMI2->valuestring);
		DeplyPar::Instance()->Tee3_SetCamera();
		DeplyPar::Instance()->PrintProInfo();
		DeplyPar::Instance()->SaveProInfo();
		string str = "{\"" + MQTTADR + "\"\:0}";
		response.GetHeaders().SetHeader("3teecustom_respon", str.c_str());
	}
	else{
		
		string str = "{\"" + MQTTADR + "\"\:-1}";
		response.GetHeaders().SetHeader("3teecustom_respon", str.c_str());
	}
}
void upnpDevice::DisconnectControl()
{
	printf("-------------Disconnect control------------\n");
	IsControlled = false;
	StopTimer();
}
void upnpDevice::Updatetimer()
{
	IsControlled = true;
	IsDisconnect = true;
}
bool upnpDevice::StartTimer(int ms){
	if (m_thread){
		printf("------------------ StartTimer ----------------------:%d\n", ms);
		m_thread->setDuration(ms);
		m_thread->Start();
		m_thread->setInterrput(false);
	}
	return true;
}
void upnpDevice::StopTimer(){
	if (m_thread){
		m_thread->Interrupt();
		m_thread->setInterrput(true);
	}
}
