#ifndef RT_REMOTE_CTRL_H_
#define RT_REMOTE_CTRL_H_

#include "common.h"
#include "room.h"

namespace tee3{
	namespace avd{

#define REMOTE_READY					0
#define REMOTE_RESULT_SUCCESS           1
#define REMOTE_RESULT_FAIL              2
#define REMOTE_RESULT_WAITING           3   ///等待中,正在处理远程控制
////mode
#define REMOTE_MOD_CONTRL								10       ///控制模式
#define REMOTE_MOD_CONTROL_END							11       ///被控模式
#define REMOTE_MOD_ALL									12       ///控制模式 and 被控模式

///Roler
#define REMOTE_ROLER_CONTROLLER    1000      ///控制端
#define REMOTE_ROLER_BE_CONTROLLED 1001      ///被控端

////Remote  reuquest stauts
#define  REMOTE_REQUEST_CONTROL    1002         ////请求控制
#define  REMOTE_REQUEST_NO_CONTROL 1003         ////取消控制
////Remote feedback stauts
#define  REMOTE_RESPON_ACCEPT          REMOTE_RESULT_SUCCESS   ///接受请求
#define  REMOTE_RESPON_REJECT          REMOTE_RESULT_FAIL   ///拒绝请求
#define  REMOTE_RESPON_REJECT_APLLAYING  1005               ///拒绝，当前正有其他用户在申请

////Remote control stauts 
#define REMOTE_STATUS_CONTROL_OTHER    2003   ///正在控制其他用户
#define REMOTE_STATUS_CONTROL_BY_OTHER 2004   ///正在被其他用户控制

///screen stauts
#define REMOTE_PUBLISH_SCREEN    3000  ///发布共享屏幕
#define REMOTE_ATTACH_SCREEN     3001  ///观看远端屏幕

///timeout
#define REMOTE_INVITE_TIMEOUT           4000  ////邀请超时
#define REMOTE_APPLY_TIMEOUT            4001  ////申请超时


		struct ContrllerEndScreenInfo{
			std::string pubScreenuserId;
			int screentSt;
			int width;
			int height;
			ContrllerEndScreenInfo() :screentSt(REMOTE_READY), width(0), height(0){}

		};
		struct RemoteInfo{
			std::string controllerUserId;
			std::string beControledUserId;
			std::string attachuserid;
			bool iscontroller;
			ContrllerEndScreenInfo remoteScreenInfo;
			int remtest; ////当前状态
			RemoteInfo(){
				clear();
			}
			void clear(){
				controllerUserId.clear();
				beControledUserId.clear();
				attachuserid.clear();
				iscontroller = false;
				remtest = REMOTE_READY;
			}
		};
	}
}
namespace tee3 {
	namespace avd {
	
		class IRemoteCtrllor;
		class  IRemoteEnd;
		class RT_API_EXPORT IRemoteCtrl :public IModule{
		public:
			class IListener{
			public:
				virtual ~IListener() {
				}
			};
			static IRemoteCtrl* getRemoteCtrlModule(IRoom* room);
			virtual void setRemoteMode(int mode) = 0; ///1:控制端，2:被控端，3既可以是控制端又可以是被控端
			virtual int getRemoteMode() = 0;
			virtual IRemoteCtrllor* getCtrollor() = 0;
			virtual IRemoteEnd*     getRemoteEnd() = 0;
			virtual Result setListener(IListener* listen) = 0;
			virtual ~IRemoteCtrl(){}
		};
		////控制端
		class RT_API_EXPORT IRemoteCtrllor :public IModule {  
		public:
			class IListener {
			public:
				/** 申请控制的回调通知
				*
				*@param[out] fromuserid 控制端用户ID
				*@param[out] touserid 被控制端用户ID
				*@param[out] rv 申请返回结果
				*note
				*/
				virtual void onApplyCtrlResult(std::string fromuserid,std::string touserid,Result rv) = 0;
				/** 取消控制的回调通知
				*
				*@param[out] fromuserid 控制端用户ID
				*@param[out] touserid 被控制端用户ID
				*@param[out] rv 申请返回结果
				*note
				*/
				virtual void onCancelCtrlResult(std::string fromuserid, std::string touserid, Result rv) = 0;
				/** 停止控制的回调通知
				*
				*@param[out] controluid  控制端用户ID
				*@param[out] controlendId 被控端用户ID
				*note
				*/
				virtual void onStopCtrlbyControlEnd(std::string controluid, std::string controlendId) = 0;

				/** 邀请控制的通知
				*
				*@param[out] contruid 控制端用户ID
				*@param[out] conendId 被制端用户ID
				*@param[out] rv 申请返回结果
				*note
				*/
				virtual void onInvitNotify(std::string contruid, std::string conendId, int rt) = 0;

			protected:
				virtual ~IListener() {
				}
			};

			virtual Result setListener(IRemoteCtrllor::IListener* listener) = 0;

			/** 申请远端控制
			*
			*@param[in] controledUserId 申请控制的用户ID
			*@return 返回结果
			*/
			virtual Result ApplyCtrl(std::string controledUserId) = 0;

			/** 停止远端控制
			*
			*@param[in] controledUserId 申请控制的用户ID
			*@return 返回结果
			*/
			virtual Result CancelCtrl(std::string controledUserId) = 0;

			/** 获取用户状态
			*
			*@return 状态类型 :REMOTE_REQUEST_CONTROL REMOTE_REQUEST_NO_CONTROL
			*/
			virtual int getCurStauts() = 0;

			/** 获取控制中的角色
			*
			*@return 获取当前用户角色REMOTE_ROLER_CONTROLLER   REMOTE_ROLER_BE_CONTROLLED
			*/
			virtual int getRole() = 0;

			/** 获取到唯一标示符
			*
			*@return获取token
			*/
			virtual std::string getToken() = 0;

			/** 发送鼠标命令
			*@param[in] mousetype 控制端用户ID
			*@param[in] xpos x坐标在控制端屏幕位置
			*@param[in] ypos y坐标在控制端屏幕位置
			*@param[in] wndwidth 控制端屏幕宽度 
			*@param[in] wndheight 控制端屏幕高度
			*@param[in] touserId 空值，暂时不用
			*@return 返回结果
			*/
			virtual Result SendMouseCmd(int mousetype, int xpos, int ypos, int wndwidth, int wndheight, std::string touserId = "") = 0;

			/** 发送键盘命令
			*@param[in] ctrlkey 控制按键，ctrl,alt,shift ...
			*@param[in] keyvalue 具体的键值
			*@param[in] touserId 空值，暂时不用
			*@return 返回结果
			*/
			virtual Result SendKeyBoardCmd(std::vector<uint32> ctrlkey, std::vector<uint32> keyvalue, std::string touserId = "") = 0;

			/** 清空当前状态
			*
			*note 用户退出房间或者断线时使用
			*/
			virtual void clearStatus() = 0;

			/*获取远程控制的信息
			*note 获取相关控制的信息
			*/
			virtual RemoteInfo getRemoteInfo() = 0;
			virtual ~IRemoteCtrllor() {
			}
		};

		////被控端
		class RT_API_EXPORT IRemoteEnd :public IModule{
		public:
			class IListener {
			public:
				/** 收到申请的通知
				*
				*@param[out] fromuserid 控制端ID
				*@param[out] touserid 被制端用户ID
				*@param[out] st 申请通知结果
				*note
				*/
				virtual void onApplyCtrlNotify(std::string fromuserid, std::string touserid,int st) = 0; 

				/** 邀请控制的通知
				*
				*@param[out] fromuserid 控制端ID
				*@param[out] touserid 被制端用户ID
				*@param[out] st 申请通知结果
				*note
				*/
				virtual void onCancelCtrlNotify(std::string fromuserid, std::string touserid, int st) = 0;

				/** 邀请结果回调
				*
				*@param[out] contruid 控制端ID
				*@param[out] conendId 被制端用户ID
				*@param[out] st 申请通知结果
				*note
				*/
				virtual void onInvitResult(std::string contruid, std::string conendId, int st) = 0;

			protected:
				virtual ~IListener() {
				}
			};

			virtual Result setListener(IRemoteEnd::IListener* listener) = 0;

			/** 处理远端申请控制
			*
			*@param[in] fromuserid 控制端用户ID
			*@param[in] rt 发送申请结果
			*note
			*/
			virtual Result DoApplyCtrl(std::string fromuserid,int rt) = 0;

			/** 处理取消申请
			*
			*@param[in] fromuserid 控制端用户ID
			*@param[in] rt 发送申请结果
			*note
			*/
			virtual Result DoCancelCtrl(std::string fromuserid, int rt) = 0;

			/** 主动终止被控
			*
			*return
			*/
			virtual Result terminalCtrl() = 0;
			/** 多屏情况下设置发布屏幕的尺寸和坐标信息
			*
			*
			*
			*/
			virtual void  SetPubScreenInfo(int top, int left, int width, int height) = 0;

			/** 获取用户状态
			*
			*@return 状态类型 :REMOTE_REQUEST_CONTROL REMOTE_REQUEST_NO_CONTROL
			*/
			virtual int getCurStauts() = 0;

			/** 获取控制中的角色
			*
			*@return 获取当前用户角色REMOTE_ROLER_CONTROLLER   REMOTE_ROLER_BE_CONTROLLED
			*/
			virtual int getRole() = 0;

			/** 获取到唯一标示符token
			*
			*/
			virtual std::string getToken() = 0;

			/** 停止用户的控制,但是不通知对方,用于控制方退出房间
			*
			*@param[in] userid 控制端的用户ID
			*
			*/
			virtual Result controllerLeave(std::string userid) = 0;

			/** 邀请用户控制自己
			*
			*@param[in] ctrluserid 邀请控制的用户Id
			*/
			virtual Result InviteController(std::string ctrluserid) = 0;

			/** 清空状态
			*
			*/
			virtual void clearStatus() = 0;

			/*获取远程控制的信息
			*
			*/
			virtual RemoteInfo getRemoteInfo() = 0;
			virtual ~IRemoteEnd(){}
		};

	} // namespace avd
} // namespace tee3
#endif//RT_REMOTE_CTRL_H_
