#ifndef RT_IMSvrLive_H_
#define RT_IMSvrLive_H_

#include "common.h"
#include "room.h"
#include "videorender.h"

namespace tee3 {
	namespace avd {

		/**
		* 房间旁路直播管理接口.
		*
		* 房间旁路直播定义房间中直播频道的创建、删除、信息获取和频道控制等功能。
		* 直播频道：类似电视频道，能够承载一路视频或一路混流视频和音频的一个逻辑通道；直播源通过直播频道发布视频，直播播放端通过直播频道订阅和观看直播音视频。
		*/
		class RT_API_EXPORT IMSvrLive {
		public:
			/**
			* 直播频道回调接口类
			*
			* 直播频道回调接口定义了直播频道管理操作的异步返回。
			*/
			class IListener {
			public:
				/// 异步返回
				/** 创建直播频道的异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] info 直播频道信息。
				*
				* @sa createLive
				*/
				virtual void onCreateLive(Result result, const LiveInfo& info) = 0;
				/** 为特定直播频道设置当前视频源的异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] lvbc_id 频道号。
				* @param[in] selectedVideo 已发布的视频Id。
				*
				* @sa selectMedia4Live
				*/
				virtual void onSelectMedia4Live(Result result, const LiveId& lvbc_id) = 0;
				/** 为特定直播频道设置混流视频源的异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] lvbc_id 频道号。
				* @sa selectMedia4Live
				*/
				virtual void onMixMedia4Live(Result result, const LiveId& lvbc_id) = 0;
				/** 停止特定直播频道的异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] lvbc_id 频道号。
				*
				* @sa pauseResumeLive
				*/
				virtual void onPauseResumeLive(Result result, const LiveId& lvbc_id) = 0;
				/** 获取特定频道详细信息的异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] info 直播频道信息。
				*
				* @sa getLiveInfo
				*/
				virtual void onLiveInfo(Result result, const LiveInfo& info) = 0;
				/** 删除特定频道的异步返回
				*
				* @param[in] result 错误代码。
				* @param[in] lvbc_id 频道号。
				*
				* @sa deleteLive
				*/
				virtual void onDeleteLive(Result result, const LiveId& lvbc_id) = 0;
			protected:
				virtual ~IListener() {}
			};
		public:
			/** 获取引擎的直播功能接口
			*
			* @return 返回直播接口。
			*/
			static IMSvrLive* getSvrLive(IRoom *room);
			/** 设置直播功能回调
			*
			* @param[in] *listener 回调指针。
			* @return 返回错误代码。
			*/
			virtual Result setListener(IListener* listener) = 0;
			/** 创建直播频道
			*
			* @param[in,out] info 中roomId，name必须设置；创建成功时id会设置成新创建的直播频道Id。
			* @note 为异步操作，操作结果在 onCreateLive 中返回。
			* @sa onCreateLive
			*/
			virtual Result createLive(LiveInfo& info) = 0;
			/** 为特定直播频道设置当前视频源
			*
			* @param[in]  lvbc_id 频道号。
			* @param[in]  selectedVideo 已发布的视频Id,直播的视频源。
			* @param[in]  audioUserId 直播的音频用户Id，可选房间中所有用户音频 AVD_userId_allUser。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onSelectVideo4Live 中返回。
			* 函数调用后，服务器立即开启单路视频流推流模式。与接口mixMedia4Live相互影响，以最后一个调用为当前旁路直播的媒体源。
			* @sa onSelectMedia4Live
			* @sa AVD_userId_allUser
			*/
			virtual Result selectMedia4Live(const LiveId& lvbc_id, const DeviceId& selectedVideo, const UserId& audioUserId) = 0;

			/**
			* 视频混流信息结构.
			*
			* 视频混流信息结构定义了一个视频混流的基本信息。
			*/
			struct MixVideos {
				uint32 mixWidth;				//混流输出视频分辨率宽
				uint32 mixHeight;				//混流输出视频分辨率高
				uint32 backgroundColor;		    //如："#C0C0C0" RGB
				uint32 outBitrate;				//混流输出码率
				uint32 outFps;					//混流输出帧率

				struct VideoItem {
					DeviceId id;				//摄像头Id，唯一标示一路共享的摄像头视频。
					float x, y;					//此视频在左上角左边位置（上边位置），即x坐标，取值0.0~1.0，内部取值为：width * X（height * Y）
					float width, height;		//此视频宽度（高度）比例，取值0.0~1.0，内部取值为：width * W（height * H）
					uint16 zorder;				//多个视频覆盖时，前后位置，zorder越小，视频越在下层，会被覆盖；取值0-10
					uint16 alpha;				//（暂不使用）透明度
					IVideoRenderView::ScalingType render;	//（暂不使用）视频渲染到混流视频上的渲染模式

					VideoItem() : x(0.0f), y(0.0f), width(0.0f), height(0.0f), zorder(0), alpha(0), render(IVideoRenderView::Scale_Aspect_Fit) {}
					VideoItem(const DeviceId& aId, float ax, float ay, float aw, float ah, uint16 az) : id(aId), x(ax), y(ay), width(aw), height(ah) {}
				};
				typedef std::vector<VideoItem> VideosType;
				VideosType videos;
			public:
				MixVideos() :mixWidth(0), mixHeight(0), backgroundColor(0), outBitrate(0), outFps(0){}
				MixVideos(uint32 aw, uint32 ah, uint32 outbt) :mixWidth(aw), mixHeight(ah), backgroundColor(0), outBitrate(outbt), outFps(0){}
			};
			/** 为特定直播频道设置混流视频源
			*
			* @param[in]  lvbc_id 频道号。
			* @param[in]  mixs 混流视频源信息结构。
			* @param[in]  audioUserId 直播的音频用户Id，可选房间中所有用户音频 AVD_userId_allUser。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onMixMedia4Live 中返回。
			* 函数调用后，服务器立即开启混流推流模式。与接口selectMedia4Live相互影响，以最后一个调用为当前旁路直播的媒体源。
			* @sa onMixMedia4Live
			* @sa AVD_userId_allUser
			*/
			virtual Result mixMedia4Live(const LiveId& lvbc_id, const MixVideos& mixs, const UserId& audioUserId) = 0;
			/** 恢复和暂停特定直播频道
			*
			* @param[in]  lvbc_id 频道号。
			* @param[in]  isResume 是否恢复。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onPauseResumeLive 中返回。
			* @sa onPauseResumeLive
			*/
			virtual Result pauseResumeLive(const LiveId& lvbc_id, bool isResume) = 0;
			/** 获取特定频道详细信息
			*
			* @param[in]  lvbc_id 频道号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onLiveInfo 中返回。
			* @sa onLiveInfo
			*/
			virtual Result getLiveInfo(const LiveId& lvbc_id) = 0;
			/** 删除特定频道
			*
			* @param[in]  lvbc_id 频道号。
			* @return 返回错误代码。
			* @note 为异步操作，操作结果在 onDeleteLive 中返回。
			* @sa onDeleteLive
			*/
			virtual Result deleteLive(const LiveId& lvbc_id) = 0;
		protected:
			virtual ~IMSvrLive() {}
		};

		RT_API_EXPORT const String toString(const IMSvrLive::MixVideos& obj);
	} // namespace avd
} // namespace tee3
#endif//RT_IMSvrLive_H_
