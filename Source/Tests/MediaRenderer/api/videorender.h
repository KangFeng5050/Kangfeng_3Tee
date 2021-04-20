#ifndef RTC_VideoRender_H_
#define RTC_VideoRender_H_

#include "common.h"

namespace tee3 {
	namespace avd {
		class IVideoRender;
		class IVideoFrame;
		class IMAnnotation;
		class IRemoteCtrl;

		/**
		* IVideoRender attach到某个视频后，接收到首帧视频的回调
		*/
		class RT_API_EXPORT IFirstFrameListener{
		public:
			/** 接收到首帧视频的回调
			*
			* @param[in] render 渲染对象指针。
			*/
			virtual void onFirstFrameArrived(IVideoRender* render) = 0;
		protected:
			virtual ~IFirstFrameListener() {
			}
		};

		/**
		* 外部对象销毁通知接口
		*
		* 当对象被销毁的时候，通知所有引用的对象，取消引用关系。
		*/
		class RT_API_EXPORT DestroyNotify {
		public:
			class IListener {
			public:
				virtual void OnDestroy(DestroyNotify* destroy) = 0;
		  protected:
				virtual ~IListener() {}
			};
			typedef std::vector<IListener*> ListenersType;

			/** 设置渲染对象销毁回调指针
			*
			* @param[in] pNotify 销毁回调指针。
			*/
			virtual Result AddDestroyNotify(IListener* pListener) {
				for (ListenersType::size_type i = 0; i < pListeners_.size(); ++i) {
					if (pListener == pListeners_[i]) {
						return AVD_OK;
					}
				}
				pListeners_.push_back(pListener);
				return AVD_OK;
			}

			/** 删除渲染对象销毁回调指针
			*
			* @param[in] pNotify 销毁回调指针。
			*/
			virtual Result RemoveDestroyNotify(IListener* pListener) {
				for (ListenersType::iterator it = pListeners_.begin(); it != pListeners_.end(); it++) {
					if ((*it) == pListener) {
						pListeners_.erase(it);
						break;
					}
				}
				return AVD_OK;
			}
			virtual ~DestroyNotify() {
				ListenersType listeners = pListeners_;
				pListeners_.clear();
				for (ListenersType::const_iterator it = listeners.begin(); it != listeners.end(); it++) {
					(*it)->OnDestroy(this);
				}
				listeners.clear();
			}
		protected:
			ListenersType	pListeners_;
		};

		/**
		* 视频渲染接口类
		*
		* 视频渲染定义了视频的渲染中涉及的视频参数变化、视频帧数据、视频抓图等相关接口。
		*/
		class RT_API_EXPORT IVideoRender : public DestroyNotify {
		public:
			/** 视频宽高变化调用
			*
			* @param[in] width  视频宽度。
			* @param[in] height 视频高度。
			*/
			virtual void SetSize(int width, int height) = 0;
			/** 视频帧数据到达调用
			*
			* @param[in] frame IVideoFrame图像对象指针。
			*/
            virtual void RenderFrame(const IVideoFrame& frame) = 0;
			//
			/** 启停图像抓取功能
			*
			* @param[in] enable 开启或者停止。
			* @return 返回错误代码。
			* @note 渲染的时候，正常情况下不缓存图像； 若需要capture才做缓存；因EnableCapture后，要下帧图像放到缓存中才能做Save2Image。
			* 故若需要capture时，在attachRender后就直接EnableCapture较好。
			*/
			virtual Result EnableCapture(bool enable) {
				return Err_Not_Implemented;
			}
			/** 抓取图像
			*
			* @param[in] fileName 图像保存地址，绝对地址, 支持".bmp和.jpg"。
			* @param[in] quality  图像质量：0-100, 0最差，100最好。
			* @return 返回错误代码。
			*/
			virtual Result CaptureImage(const String& fileName, int quality) {
				return Err_Not_Implemented;
			}

			/** 获取render所在的WindowId
			*
			* @param[in,out] id  显示对象句柄。
			* @return 返回错误代码。
			*/
			virtual Result GetWindowId(WindowId& id) const {
				return Err_Not_Implemented;
			}

			/** 获取图像大小
			*
			* @param[out] width  图像宽度。
			* @param[out] height  图像高度。
			* @return 返回错误代码。
			*/
			virtual Result GetSize(int& width, int& height) const {
				return Err_Not_Implemented;
			}

			/** 设置注释接口
			*
			* @param[out] atn  注释模块接口。
			* @return 返回错误代码。
			*/
			virtual Result SetAnnotation(tee3::avd::IMAnnotation* atn) {
				return Err_Not_Implemented;
			}

			/** 获取注释接口
			*
			* @param[out] atn  注释模块接口。
			* @return 返回错误代码。
			*/
			virtual Result GetAnnotation(tee3::avd::IMAnnotation** atn) {
				return Err_Not_Implemented;
			}

			/** 设置首帧数据回调指针
			*
			* @param[in] listener  首帧数据回调指针。
			* @return 返回错误代码。
			* @note 此函数设置后，VideoRender在首帧视频数据接收到时会回调onFirstFrameArrived；
			*		注意：每次重新attachRender后需要重新设置此函数，即设置此函数后，只有一次回调，下次即使同一VideoRender需要再次获取首帧回调，需要重新设置。
			*/
			virtual Result setFirstFrameListener(IFirstFrameListener* listener) {
				return Err_Not_Implemented;
			}
			/** IVideoFrame图像保存为jpeg图像功能
			*
			* @param[in] frame IVideoFrame图像对象指针。
			* @param[in] jpegorbmpFileName 图像保存地址，绝对地址, 支持".bmp和.jpg"。
			* @param[in] quality  图像质量：0-100, 0最差，100最好。
			* @return 返回错误代码。
			*/
			static Result Save2Image(const IVideoFrame* frame, const String& jpegorbmpFileName, int quality/*0-100, higher is better*/);

			/** 设置远程控制模块,对方userid
			*
			*/
			virtual int32 SetRemoteCtrlModule(IRemoteCtrl* remote,std::string othersideuid){
				return Err_Not_Implemented;
			}

			/** 获取该render是否用gdi模式创建的
			*
			*/
			virtual bool getIsGdiView() {
				return false;
			}
		protected:
			// The destructor is protected to prevent deletion via the interface.
			// This is so that we allow reference counted classes, where the destructor
			// should never be public, to implement the interface
		protected:
			virtual ~IVideoRender() {
			}
		};

		class RT_API_EXPORT IVideoRenderView {
		public:
			enum ScalingType {
				Scale_Fill = 1, // video frame is scaled to to fill the size of the view. Video aspect ratio is changed if necessary.
				Scale_Aspect_Fit = 2, // video frame is scaled to fit the size of the view by maintaining the aspect ratio (black borders may be displayed).
				Scale_Aspect_Full = 3, // video frame is scaled to fill the size of the view by maintaining the aspect ratio. Some portion of the video frame may be clipped.
			};
		public:
			static IVideoRenderView* CreateVideoRenderView(const int32 id,
				void* window,
				const bool fullscreen, bool isgdi = false);
			static void DestroyVideoRenderView(IVideoRenderView* module);

			virtual IVideoRender*
				CreateVideoRender(const uint32 renderId,
				const uint32 zOrder,
				const float left, const float top,
				const float right, const float bottom) = 0;

			virtual IVideoRender* CreateVideoRender(const uint32 renderId,
				const uint32 zOrder,
				ScalingType scalingType,
				const float left, const float top,
				const float right, const float bottom) = 0;

			virtual int32
				DeleteVideoRender(const uint32 renderId) = 0;

			virtual int32 StartRender(const uint32 renderId) = 0;

			virtual int32 StopRender(const uint32 renderId) = 0;

			/*
			 *   Set the renderer ScalingType
			 */
			virtual bool SetScalingType(const uint32 renderId, ScalingType type) = 0;

			/*
			 *   Get the renderer ScalingType
			 */
			virtual ScalingType	RenderScalingType(const uint32 renderId) = 0;

			virtual int32 UpdateWindowSize() = 0;

			virtual int32 SetBitmap(const void* bitMap,
				const uint8 pictureId,
				const void* colorKey,
				const float left, const float top,
				const float right, const float bottom) = 0;

			virtual int32 SetText(const uint8 textId,
				const char* text,
				const int32 textLength,
				const uint32 textColorRef,
				const uint32 backgroundColorRef,
				const float left, const float top,
				const float right, const float bottom) = 0;

			virtual int32 SetText(const uint8 textId,
				const char* text,
				const int32 textLength,
				const uint32 textColor,
				const float left, const float top,
				const float rigth, const float bottom,
				const int32 format,
				const int32 height,
				const uint32 width,
				const uint32 weight,
				const bool italic,
				const char* faceName,
				const int32 faceNameSize) = 0;
		protected:
			virtual ~IVideoRenderView() {};

		};
		enum {
			ROTATION_0 = 0,
			ROTATION_90 = 90,
			ROTATION_180 = 180,
			ROTATION_270 = 270
		};

		//// Represents a YUV420 (a.k.a. I420) video frame.
		class RT_API_EXPORT IVideoFrame {
		public:
			virtual ~IVideoFrame() {}

			// Size of an I420 image of given dimensions when stored as a frame buffer.
			static size_t SizeOf(size_t w, size_t h) {
				return w * h + ((w + 1) / 2) * ((h + 1) / 2) * 2;
			}
            static const IVideoFrame& getBlackFrame();
            virtual int32 size() const { return width() * height() + ((width() + 1) / 2) * ((height() + 1) / 2) * 2; }
            virtual int32 width() const = 0;
            virtual int32 height() const = 0;
			virtual bool is_texture() const = 0;

            virtual const uint8 *dataY() const = 0;
            virtual const uint8 *dataU() const = 0;
            virtual const uint8 *dataV() const = 0;

            virtual int32 strideY() const = 0;
            virtual int32 strideU() const = 0;
            virtual int32 strideV() const = 0;
            
            virtual int32 rotation() const = 0;

            virtual IVideoFrame* copy() const = 0;
            virtual int32 convertToRgb(uint32 to_fourcc, uint8 *buffer,
                                              size_t size, int stride_rgb) const = 0;

		};

	} // namespace avd
} // namespace tee3
#endif//RTC_VideoRender_H_
