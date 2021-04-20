#include "deployfile.h"

DeplyPar::DeplyPar()
{
	IsStartMQTT = false;
	AudioIntSign = false;
	AudioImportdata = "";
	conn_opts = MQTTClient_connectOptions_initializer;
	pubmsg = MQTTClient_message_initializer;
	conn_opts.keepAliveInterval = 5;	
	conn_opts.cleansession = 1;
	conn_opts.cleansession = 0;
	pubmsg.qos = 1;
	pubmsg.retained = 0;
	mqttconnectRes = 0;
	m_pipe = NULL;
	m_pipe2 = NULL;
   	mqttconnectRes = -1;
	Tee3EngineRes = -1;
	JoinRoomRes = -1;	
}
DeplyPar::~DeplyPar()
{
	if(m_pipe){
		m_pipe->setStartImported(false);
		delete m_pipe;
		m_pipe = NULL;
	}
	if(m_pipe2){
		m_pipe2->setStartImported(false);
		delete m_pipe2;
		m_pipe2 = NULL;
	}
}

DeplyPar* DeplyPar_instance = NULL;
DeplyPar* DeplyPar::Instance() {
	if (NULL == DeplyPar_instance) {
		DeplyPar_instance = new DeplyPar();
	}
	return DeplyPar_instance;
}
bool DeplyPar::loadProfileFile()
{
	int size;
	char *SetBuf = NULL;
	FILE* SetFd = fopen("/Tee3/Setting.json","rb+");
	fseek(SetFd,0,SEEK_END);
	size = ftell(SetFd);
	if(size < 1)
		return false;
	rewind(SetFd);
	SetBuf = (char*)malloc(size);
	fread(SetBuf, 1, size, SetFd);
	fclose(SetFd);
	printf("start cJSON file\n");
	cJSON * pJson = cJSON_Parse(SetBuf);
	if (NULL != pJson){
		cJSON* serverAdr = cJSON_GetObjectItem(pJson, "serverAdr");
		cJSON* mqttAdr   = cJSON_GetObjectItem(pJson, "mqttAdr");
		cJSON* deviceName = cJSON_GetObjectItem(pJson,"deviceName");
		cJSON* devicePwd = cJSON_GetObjectItem(pJson,"devicePwd");
		cJSON* HDMI1 = cJSON_GetObjectItem(pJson, "HDMI1");
		cJSON* HDMI2 = cJSON_GetObjectItem(pJson, "HDMI2");

		//配置设备参数
		Tee3_url = serverAdr->valuestring;
		MqttAddress = mqttAdr->valuestring;
		ClientName = deviceName->valuestring;
		ClientPasswd = devicePwd->valuestring;
		HDMI1_name = HDMI1->valuestring;
		HDMI2_name = HDMI2->valuestring;
		Client_Topic = "/u/" + ClientName;
	}
}

bool DeplyPar::SaveProInfo()
{
	int SaveFd = open("/Tee3/Setting.json",O_RDWR|O_TRUNC);
	cJSON* SaveJson = cJSON_CreateObject();
	cJSON_AddStringToObject(SaveJson, "serverAdr", Tee3_url.c_str());
	cJSON_AddStringToObject(SaveJson, "mqttAdr", MqttAddress.c_str());
	cJSON_AddStringToObject(SaveJson, "deviceName", ClientName.c_str());
	cJSON_AddStringToObject(SaveJson, "devicePwd", ClientPasswd.c_str());
	cJSON_AddStringToObject(SaveJson, "HDMI1", HDMI1_name.c_str());
	cJSON_AddStringToObject(SaveJson, "HDMI2", HDMI2_name.c_str());
	char* cjson_str = cJSON_Print(SaveJson);
	printf("Save: %s\n",cjson_str);
	write(SaveFd, cjson_str, strlen(cjson_str));
	close(SaveFd);
	return true;
}
void DeplyPar::Tee3_SetCamera(){
	Tee3_user.userId = ClientName.c_str();
	Tee3_user.userName = ClientName.c_str();
	fakeCamera.id = "DHMI1";
	fakeCamera.name = HDMI1_name.c_str();
	fakeCamera2.id = "HDMI2";
	fakeCamera2.name = HDMI2_name.c_str();
}

void DeplyPar::SetPduId(int id)
{
	pduId = id;
}
void DeplyPar::SetTee3Addr(string url)
{
	Tee3_url = url;
}
void DeplyPar::SetMqttFromId(string Fromid)
{
	MQTT_FromId = Fromid;
}
void DeplyPar::SetMqttAddress(string addr)
{
	MqttAddress = addr;
}
void DeplyPar::SetClienname(string name)
{
	ClientName = name;
	Client_Topic = "/u/" + name;
}
void DeplyPar::SetClienpasswd(string passwd)
{
	ClientPasswd = passwd;
}
void DeplyPar::SetHDMI_1_nmae(string HDMI1)
{
	HDMI1_name = HDMI1;
}
void DeplyPar::SetHDMI_2_nmae(string HDMI2)
{
	HDMI2_name = HDMI2;
}
void DeplyPar::SetRoomid(string roomid)
{
	Tee3_RoomID = roomid;
}
void DeplyPar::SetTee3User(string userId,string userName)
{
	Tee3_user.userId = userId.c_str();
	Tee3_user.userName = userName.c_str();
}
void DeplyPar::SetfakeCamera_1(string id, string name)
{
	fakeCamera.id = id.c_str();
	fakeCamera.name = name.c_str();
}
void DeplyPar::SetfakeCamera_2(string id, string name)
{
	fakeCamera2.id = id.c_str();
	fakeCamera2.name = name.c_str();
}
void DeplyPar::SetMqttconnectOpt()
{
	conn_opts.username = ClientName.c_str();
	conn_opts.password = ClientPasswd.c_str();
}
void DeplyPar::SendMqttInfo(string data, header::signal_group_type group_type, header::signal_cmd_type cmd_type)
{
	header::signal_header m_signalheader;
	signal_invite::InviteReject m_signalaccept;
	char Reject_buffer[30];
	sprintf(Reject_buffer, "{\"reson\":\"%s\"}", data.c_str());
	m_signalaccept.set_id(pduId);
	m_signalaccept.set_reson(Reject_buffer);
	int size = m_signalaccept.ByteSize();
	void *buffer = malloc(size);
	m_signalaccept.SerializeToArray(buffer, size);
	m_signalheader.set_version("1.0.0");
	m_signalheader.set_group_type(group_type);
	m_signalheader.set_cmd_type(cmd_type);
	m_signalheader.set_timestamp((google::protobuf::uint32)time(NULL));
	m_signalheader.set_from(ClientName.c_str());
	m_signalheader.set_to(MQTT_FromId);
	m_signalheader.set_content((char*)buffer, size);
	int size2 = m_signalheader.ByteSize();
	void *buffer2 = malloc(size2);
	m_signalheader.SerializeToArray(buffer2, size2);
	pubmsg.payload = buffer2;
	pubmsg.payloadlen = size2;
	pubmsg.qos = 1;
	pubmsg.retained = 0;
	std::string O_MQTT_FromId = "/u/" + MQTT_FromId;
	MQTTClient_publishMessage(Myclient, O_MQTT_FromId.c_str(), &pubmsg, &token);
	MQTTClient_waitForCompletion(Myclient, token, 10000);
	free(buffer);
	free(buffer2);
}
void DeplyPar::SelfcheckMQTT(){
	header::signal_header m_signalheader;
	signal_invite::InviteReject m_signalaccept;
	char Reject_buffer[30];
	sprintf(Reject_buffer, "{\"reson\":\"Self-check\"}");
	m_signalaccept.set_id(pduId);
	m_signalaccept.set_reson(Reject_buffer);
	int size = m_signalaccept.ByteSize();
	void *buffer = malloc(size);
	m_signalaccept.SerializeToArray(buffer, size);
	m_signalheader.set_version("1.0.0");
	m_signalheader.set_group_type(header::SIGNALE_GROUP_TYPE_SYS);
	m_signalheader.set_cmd_type(header::SIGNALE_CMD_TYPE_MESSAGE_DATA);
	m_signalheader.set_timestamp((google::protobuf::uint32)time(NULL));
	m_signalheader.set_from(ClientName.c_str());
	m_signalheader.set_to(ClientName);
	m_signalheader.set_content((char*)buffer, size);
	int size2 = m_signalheader.ByteSize();
	void *buffer2 = malloc(size2);
	m_signalheader.SerializeToArray(buffer2, size2);
	pubmsg.payload = buffer2;
	pubmsg.payloadlen = size2;
	pubmsg.qos = 1;
	pubmsg.retained = 0;
	MQTTClient_publishMessage(Myclient, Client_Topic.c_str(), &pubmsg, &token);
	MQTTClient_waitForCompletion(Myclient, token, 10000);
	free(buffer);
	free(buffer2);
}
void DeplyPar::PrintProInfo()
{
	printf("--------------------------------Client_Name:%s\n",ClientName.c_str());
	printf("--------------------------------ClientPasswd:%s\n",ClientPasswd.c_str());
	printf("--------------------------------HDMI1_name:%s\n",HDMI1_name.c_str());
	printf("--------------------------------HDMI2_name:%s\n",HDMI2_name.c_str());
	printf("--------------------------------M_Address:%s\n",MqttAddress.c_str());	
	printf("--------------------------------Tee3_url:%s\n",Tee3_url.c_str());
}
