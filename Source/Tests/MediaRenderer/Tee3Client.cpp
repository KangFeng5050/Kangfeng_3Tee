#include "Tee3Client.h"

MClient::MClient():m_roomobj(0),m_audio(0),m_video(0),m_userManager(0),in(0),out(0)
{
		InRoom = false;
		m_IsInitEngine = false;
		_appkey = 	"demo_access";
		_secretkey = "demo_secret";
}
MClient::~MClient(){ 
		UninitEngine();
	}

MClient* MClient_instance = NULL;
MClient* MClient::Instance() {
	if (NULL == MClient_instance) {
		MClient_instance = new MClient();
	}
	return MClient_instance;
}
	int MClient::InitEngine(){
		cout << "AVDEngine start" << endl;
		int result = 0;
		tee3::avd::IAVDEngine::Instance()->uninit();
		//设置日志文件
		tee3::avd::IAVDEngine::Instance()->setLogParams("info", "mclient.log");
		in = new AudioInProxy(48000, 1);
		out = new AudioOutProxy(48000, 1);
		GlobalDeviceManager::SetAudioInterface(in, out);
		//初始化		
		result = tee3::avd::IAVDEngine::Instance()->init(this, DeplyPar::Instance()->GetTee3Addr(), _appkey, _secretkey);	
		if (result != AVD_OK)
		{
			cout << "AVDEngine registration failed!" << endl;
			cout << "appkey: " << _appkey << endl;
			cout << "secretkey: " << _secretkey << endl;
			return -1;
		}
		else
		{
			cout << "AVDEngine initialization successful !!" << endl;
		}
		cout << "AVDEngine  end!" << endl;
		return 1;
	}
	void MClient::UninitEngine(){
		tee3::avd::IAVDEngine::Instance()->release();
		delete in;
		delete out;
	}
	void MClient::LeaveRoom(){
		DeplyPar::Instance()->JoinRoomRes = -1;
		if(m_roomobj){
			m_roomobj->leave(0);
			m_roomobj = NULL;
		}
		if(m_audio)
		{
			m_audio->closeMicrophone();
			m_audio->setListener(NULL);
			m_audio = NULL;
		}
		if(m_video){
			m_video->unpublishLocalCamera(DeplyPar::Instance()->GetfakeCamera_1()->id);
			m_video->unpublishLocalCamera(DeplyPar::Instance()->GetfakeCamera_2()->id);
			m_video->setListener(NULL);
			if(DeplyPar::Instance()->m_pipe){
				DeplyPar::Instance()->m_pipe->setStartImported(false);
				delete DeplyPar::Instance()->m_pipe;
				DeplyPar::Instance()->m_pipe = NULL;
			}
			if(DeplyPar::Instance()->m_pipe2){
				DeplyPar::Instance()->m_pipe2->setStartImported(false);
				delete DeplyPar::Instance()->m_pipe2;
				DeplyPar::Instance()->m_pipe2 = NULL;
			}
			m_video = NULL;
		}
		if(m_userManager){
			m_userManager->setListener(NULL);
			m_userManager = NULL;
		}
		DeplyPar::Instance()->AudioIntSign = false;
		DeplyPar::Instance()->AudioImportdata = "";
		MClient::Instance()->InRoom = false;
		printf("Leave room!!!\n");
	}	
static int rejoin_times = -1;
	int MClient::JoinRoom(tee3::String roomID){
		cout << "join room ,roomid=" << roomID << endl;
		if (!m_roomobj){
			cout << "start join room 01" << endl;
			m_roomobj = tee3::avd::IRoom::obtain(roomID);
			m_roomobj->setListener(this);
			m_roomobj->setOption(tee3::avd::ro_room_rejoin_times, &rejoin_times);
			cout << "start join room, m_roomobj = " << m_roomobj<< endl;
			m_audio = IMAudio::getAudio(m_roomobj);
			m_audio->setListener(this);
			cout << "start join room m_audio = "<<m_audio << endl;
			m_video = IMVideo::getVideo(m_roomobj);
			m_video->setListener(this);
			cout << "start join room m_video = "<<m_video << endl;
			m_userManager = IMUserManager::getUserManager(m_roomobj);
			m_userManager->setListener(this);
			cout << "start join room m_userManager = "<<m_userManager << endl;
			m_roomobj->join(*DeplyPar::Instance()->GetTee3User(), "", 0);

		}
		cout << "END join room ,roomid=" << roomID;
	}
static bool audio_aec = true;
	void MClient::onInitResult(Result result){
		cout << "result = " << result << endl;
		DeplyPar::Instance()->Tee3EngineRes = result;
		if (result == AVD_OK)
		{			
			tee3::avd::IAVDEngine::Instance()->setOption(tee3::avd::eo_audio_aec_Enable, &audio_aec);
			
			cout << "This is the callback after InitEngine is initialized !!!" << endl;
			m_IsInitEngine = true;
		}
		else{
			InitEngine();
		}
		
	}
	void MClient::onUninitResult(Result reason){
		cout << "Deinitialization AVDEngine notification!!" << endl;
	}
	void MClient::onGetRoomResult(uint32, Result, const tee3::avd::RoomInfo&){}
	void MClient::onFindRoomsResult(uint32 callId, Result result, const tee3::avd::RoomInfosType &rooms){}
	void MClient::onCancelRoomResult(uint32 callId, Result result, const tee3::RoomId &roomId){}
	void MClient::onGetUsersCountResult(uint32 callId, Result result, uint32 usersCount, const tee3::RoomId &roomId){}
	void MClient::onCallOutgoingDeviceResult(uint32 callId, Result result, const tee3::UserId &userId){}
	void MClient::onScheduleRoomResult(uint32 callId, Result result, const tee3::RoomId &roomId){
		cout << "Room application successful!!" << endl;
		cout << "callid = " << callId << endl;
		cout << "result = " << result << endl;
		cout << "roomID = " << roomId << endl;
		//获取或创建房	
		m_roomobj = tee3::avd::IRoom::obtain(roomId);
		m_roomobj->setListener(this);
		m_roomobj->join(*DeplyPar::Instance()->GetTee3User(), "", 0);
		//设置音频，获取房间音频	
		m_audio = IMAudio::getAudio(m_roomobj);
		m_audio->setListener(this);
		m_video = IMVideo::getVideo(m_roomobj);
		m_video->setListener(this);
		m_userManager = IMUserManager::getUserManager(m_roomobj);
		m_userManager->setListener(this);
	}
	void MClient::onJoinResult(Result result){
		DeplyPar::Instance()->JoinRoomRes = result;
		if (result != 0)
		{
			DeplyPar::Instance()->SendMqttInfo("Join failed",header::SIGNALE_GROUP_TYPE_MESSAGE,header::SIGNALE_CMD_TYPE_INVITE_REJECT);
			LeaveRoom();
			InRoom = false;
			printf("----------------------------Failed to join\n");
		}
		else{
			DeplyPar::Instance()->SendMqttInfo("Join successfully",header::SIGNALE_GROUP_TYPE_INVITE,header::SIGNALE_CMD_TYPE_INVITE_OK);
			InRoom = true;
			printf("----------------------------Join successfully\n");
			//开启音频双向互动
			
			m_audio->openMicrophone();
			DeplyPar::Instance()->AudioIntSign = true;
			printf("AudioInProxy!!\n");
			
			//开启两路虚拟摄像头
			cout << "publishLocalCamera1"<<endl;
			DeplyPar::Instance()->GetfakeCamera_1()->publishedQualities.setStreamOptions(tee3::avd::stream_main, tee3::avd::quality_high, tee3::avd::codec_h264);
			DeplyPar::Instance()->GetfakeCamera_2()->publishedQualities.setStreamOptions(tee3::avd::stream_main, tee3::avd::quality_high, tee3::avd::codec_h264);
			DeplyPar::Instance()->m_pipe = new EncodedCaptureFromFile(tee3::avd::FOURCC_H264);
			DeplyPar::Instance()->m_pipe2 = new EncodedCaptureFromFile(tee3::avd::FOURCC_H264);
			m_video->publishLocalCamera(*DeplyPar::Instance()->GetfakeCamera_1(), DeplyPar::Instance()->m_pipe->GetCapture());
			m_video->publishLocalCamera(*DeplyPar::Instance()->GetfakeCamera_2(), DeplyPar::Instance()->m_pipe2->GetCapture());
			DeplyPar::Instance()->m_pipe->setStartImported(true);
			DeplyPar::Instance()->m_pipe2->setStartImported(true);
			m_video->setVideoBitrate(DeplyPar::Instance()->GetfakeCamera_1()->id, 6000 * 1000, 6000 * 1000);
			m_video->setVideoBitrate(DeplyPar::Instance()->GetfakeCamera_2()->id, 6000 * 1000, 6000 * 1000);
			
		}
	}
	void MClient::onLeaveIndication(Result reason, const tee3::UserId &fromId){
		cout<<"onLeaveIndication ,reason="<<reason<<endl;
		this->LeaveRoom();
		InRoom = false;
		DeplyPar::Instance()->AudioIntSign = false;
		DeplyPar::Instance()->AudioImportdata = "";
		printf("--------------Delete in and out\n");	
	}
	void MClient::onPublicData(const char *data, uint32 len, const tee3::UserId &fromId){}
	void MClient::onPrivateData(const char *data, uint32 len, const tee3::UserId &fromId){}
	void MClient::onAppDataNotify(const std::string &key, const std::string &value){}
	void MClient::onRoomStatusNotify(tee3::avd::RoomStatus status){}
	void MClient::onConnectionStatus(tee3::avd::ConnectionStatus status){}
	void MClient::onOutgoingInviteStatusNotify(tee3::avd::OutgoingInviteType type, std::string roomId, std::string addr, tee3::avd::OutgoingStatus status, std::string msg){}
	void MClient::onMicrophoneStatusNotify(tee3::avd::MicrophoneStatus status, const tee3::UserId& fromUserId) {
		cout<<"onMicrophoneStatusNotify ,status="<<status<<",fromuid="<<fromUserId<<endl;
	}
	void MClient::onAudioLevelMonitorNotify(const tee3::avd::AudioInfo& info)  {}
	void MClient::onOpenMicrophoneResult(Result result)  {
		cout<<"onOpenMicrophoneResult ,result="<<result<<endl;
	}
	void MClient::onCloseMicrophoneResult(Result result) {
		cout<<"onCloseMicrophoneResult ,result="<<result<<endl;
	}
	//////video listerner
	void MClient::onCameraStatusNotify(CameraStatus status, const DeviceId& fromId){
		cout<<"onCameraStatusNotify ,status="<<status<<",DeviceId="<<fromId<<endl;
	}
	void MClient::onCameraDataNotify(int level, const String& description, const DeviceId& fromId) {
		cout<<"onCameraDataNotify ,level="<<level<<",description="<<description<<",fromid="<<fromId<<endl;
	}
	void MClient::onPublishCameraNotify(const Camera& camera){		cout<<"onPublishCameraNotify ,camera.id="<<camera.id<<",camera.name="<<camera.name<<endl;
	}
	void MClient::onUnpublishCameraNotify(const Camera& camera){
		cout<<"onUnpublishCameraNotify ,camera.id="<<camera.id<<",camera.name="<<camera.name<<endl;
	}
	void MClient::onSubscribeResult(Result result, const DeviceId& fromId) {
		cout<<"onSubscribeResult ,result="<<result<<",fromId="<<fromId<<endl;
	}
	void MClient::onUnsubscribeResult(Result result, const DeviceId& fromId){
		cout<<"onUnsubscribeResult ,result="<<result<<",fromId="<<fromId<<endl;
	}
	void MClient::onPublishLocalResult(Result result, const DeviceId& fromId) {
		cout<<"onPublishLocalResult ,result="<<result<<",fromId="<<fromId<<endl;
	}
	void MClient::onUnpublishLocalResult(Result result, const DeviceId& fromId) {
		cout<<"onUnpublishLocalResult ,result="<<result<<",DeviceId="<<fromId<<endl;
	}
	/////IMUserManager IListener
	void MClient::onUserJoinNotify(const User& user) {
		cout<<"onUserJoinNotify "<<endl;
	}		
	void MClient::onUserLeaveNotify(const User& user) {
		cout<<"onUserLeaveNotify "<<endl;
	}			
	void MClient::onUserLeaveNotify(Result reason, const User& user) {
		cout<<"onUserLeaveNotify "<<endl;
	}		
	void MClient::onUserUpdateNotify(const User& user) {
		cout<<"onUserUpdateNotify "<<endl;
	}		
	void MClient::onUserStatusNotify(UserStatus status, const UserId& fromId) {
		cout<<"onUserStatusNotify "<<endl;
	}		
	void MClient::onUserDataNotify(const String& userData, const UserId& fromId) {
		cout<<"onUserDataNotify "<<endl;
	}	
