#ifndef _TEE3CLIENT_H
#define _TEE3CLIENT_H


#include "Tee3Audio.h"

using namespace std;
using namespace tee3;
using namespace tee3::avd;

class MClient :public tee3::avd::IAVDEngine::IListener, 
				public tee3::avd::IRoom::IListener,
				public tee3::avd::IMAudio::IListener,
				public tee3::avd::IMVideo::IListener,
				public tee3::avd::IMUserManager::IListener{
					
private:
	MClient();
	~MClient();
public:
	static MClient* Instance();
	int InitEngine();
	void UninitEngine();
	void LeaveRoom();
	int JoinRoom(tee3::String roomID);
	//初始化引擎后的回调
	void onInitResult(Result result);
	void onUninitResult(Result reason);
	void 	onGetRoomResult(uint32, Result, const tee3::avd::RoomInfo&);
	void 	onFindRoomsResult(uint32 callId, Result result, const tee3::avd::RoomInfosType &rooms);
	void 	onScheduleRoomResult(uint32 callId, Result result, const tee3::RoomId &roomId);
	void 	onCancelRoomResult(uint32 callId, Result result, const tee3::RoomId &roomId);
	void 	onGetUsersCountResult(uint32 callId, Result result, uint32 usersCount, const tee3::RoomId &roomId);
	void 	onCallOutgoingDeviceResult(uint32 callId, Result result, const tee3::UserId &userId);
	
	void  onJoinResult(Result result);
	void  onLeaveIndication(Result reason, const tee3::UserId &fromId);
	void  onPublicData(const char *data, uint32 len, const tee3::UserId &fromId);
	void  onPrivateData(const char *data, uint32 len, const tee3::UserId &fromId);
	void  onAppDataNotify(const std::string &key, const std::string &value);
	void  onRoomStatusNotify(tee3::avd::RoomStatus status);
	void  onConnectionStatus(tee3::avd::ConnectionStatus status);
	void  onOutgoingInviteStatusNotify(tee3::avd::OutgoingInviteType type, std::string roomId, std::string addr, tee3::avd::OutgoingStatus status, std::string msg);
	///////auido listener
	virtual void onMicrophoneStatusNotify(tee3::avd::MicrophoneStatus status, const tee3::UserId& fromUserId);
	virtual void onAudioLevelMonitorNotify(const tee3::avd::AudioInfo& info);
	virtual void onOpenMicrophoneResult(Result result);
	virtual void onCloseMicrophoneResult(Result result);
	//////video listerner
	virtual void onCameraStatusNotify(CameraStatus status, const DeviceId& fromId);
	virtual void onCameraDataNotify(int level, const String& description, const DeviceId& fromId);
	virtual void onPublishCameraNotify(const Camera& camera);
	virtual void onUnpublishCameraNotify(const Camera& camera);
	virtual void onSubscribeResult(Result result, const DeviceId& fromId);
	virtual void onUnsubscribeResult(Result result, const DeviceId& fromId);
	virtual void onPublishLocalResult(Result result, const DeviceId& fromId);
	virtual void onUnpublishLocalResult(Result result, const DeviceId& fromId);
	/////IMUserManager IListener
	virtual void onUserJoinNotify(const User& user);
	virtual void onUserLeaveNotify(const User& user);
	virtual void onUserLeaveNotify(Result reason, const User& user);
	virtual void onUserUpdateNotify(const User& user);
	virtual void onUserStatusNotify(UserStatus status, const UserId& fromId);
	virtual void onUserDataNotify(const String& userData, const UserId& fromId);
public:
	tee3::avd::IRoom* m_roomobj = NULL;
	tee3::avd::IMAudio* m_audio = NULL;
	tee3::avd::IMVideo* m_video = NULL;
	tee3::avd::IMUserManager* m_userManager = NULL;
	AudioInProxy* in;
	AudioOutProxy* out;
	bool		m_IsInitEngine;
	bool 		InRoom;
	int 		g_videoOprate;
	tee3::String _appkey;
	tee3::String _secretkey;
};
#endif
