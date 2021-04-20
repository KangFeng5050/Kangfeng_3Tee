#ifndef AVD_Module_Exporter_H__
#define AVD_Module_Exporter_H__

#include "common.h"
#include "videorender.h"
#include "room.h"
#include <functional>

NAMESPACE_TEE3_AVD_BEGIN

class RT_API_EXPORT IVideoMixer {
public:
	virtual Result addMetaData(const String& mtag, const String& value) = 0;

	virtual Result setScaleType(IVideoRenderView::ScalingType scale) = 0;

	virtual Result setAutoLayout(bool useAuto) = 0;

	virtual Result addSubVideo(const DeviceId& vId, int zorder, float x, float y, float w, float h) = 0;

	virtual Result modSubVideo(const DeviceId& vId, int zorder, float x, float y, float w, float h) = 0;

	virtual bool   rmvSubVideo(const DeviceId& vId) = 0;

	virtual void   clearSubVideos() = 0;
protected:
	virtual ~IVideoMixer() {}
};

/**
* 录制数据结构.
*
* 录制数据定义了一个录制的基本信息。
*/
struct ExporterInfo {
	RecordId id;					/**< 录制(直播频道)ID */
	RoomId roomId;					/**< 关联房间号 */
	UserId creatorId;				/**< 创建者用户ID */
	String name;					/**< 名称 */
	String appdata;					/**< 应用自定义数据, json */
	String createTime;				/**< 创建起始时间 */
	RecordStatus status;			/**< 状态 */
	uint64 size;					/**< 录制：录制文件大小
									 *   直播：直播当前时长
									 */
	String desturl;					/**< 录制：文件名称 
									 *	 直播：推流地址；
									 */
	String playurls;				/**< 录制：播放录制链接列表,可能多个文件,如录制mp4视频分辨率变化会产生多个文件 
									 *	 直播：推流地址，和播放地址；根据url协议确定地址用途：如rtmp是推流和播放地址；htl是播放地址；
									 */
public:
	ExporterInfo() :status(rs_created), size(0){}
	bool isValid() const { return !id.empty(); }
};
typedef ExporterInfo* LPExporterInfo;
const String toString(const ExporterInfo&  obj);
typedef std::vector<ExporterInfo> ExporterInfosType;


class RT_API_EXPORT IMStreamExporter : public IModule {
public:
	class RT_API_EXPORT StreamOut {
	public:
		virtual void videoStreamOut(const String& eId, uint64 timestamp_ns, tee3::avd::VideoCodec c, unsigned int w, unsigned int h, bool isKeyFrame, const uint8 *data, unsigned int len) = 0;

		virtual void audioStreamOut(const String& eId, uint64 timestamp_ns, tee3::avd::AudioCodec c, int sampleRate, int channels, const uint8 *data, unsigned int len) = 0;
	protected:
		virtual ~StreamOut() {}
	};

	static IMStreamExporter* getExporter(IRoom* room);
	//
	typedef std::function<void(const String& eId, Result error, const String& msg)> Cb_OnError;
	virtual void setCb4OnError(Cb_OnError cb) = 0;
	//
	struct ViewInfo {
		uint32 mixWidth;				//混流输出视频分辨率宽
		uint32 mixHeight;				//混流输出视频分辨率高
		uint32 backgroundColor;		    //如："#C0C0C0" RGB
		uint32 outBitrate;				//混流输出码率
		uint32 outFps;					//混流输出帧率
		ViewInfo() :mixWidth(0), mixHeight(0), backgroundColor(0), outBitrate(0), outFps(0){}
		ViewInfo(uint32 aw, uint32 ah, uint32 outbt) :mixWidth(aw), mixHeight(ah), backgroundColor(0), outBitrate(outbt), outFps(15){}
	};
	virtual Result createLocalRecorder(String& eId, const String& filePath, const ViewInfo& view) = 0;

	virtual Result createLocalRealtimeRecorder(String& eId, const String& filePath, const ViewInfo& view) = 0;

	virtual Result createLocalRealtimeRecorder(String& eId, StreamOut* out, const ViewInfo& view) = 0;

	virtual Result createLocalLivecast(String& eId, const String& pushUrl, const ViewInfo& view) = 0;
	//
	typedef std::function<void(Result result, const ExporterInfo& info)> Cb_CreateResult;
	virtual Result createServerRecorder(String& eId, const String& filePath, const ViewInfo& view, Cb_CreateResult cb) = 0;

	virtual Result createServerRealtimeRecorder(String& eId, const String& filePath, const ViewInfo& view, Cb_CreateResult cb) = 0;

	virtual Result createServerLivecast(String& eId, const String& pushUrl, const String& name, const ViewInfo& view, Cb_CreateResult cb) = 0;
	//
	virtual Result updateExporterInfo(const String& eId, const String& appdata) = 0;

	//
	virtual Result selectAudio4Exporter(const String& eId, const UserId& audioUserId) = 0;
	virtual Result selectSingleVideo4Exporter(const String& eId, const DeviceId& singleVideo) = 0;
	struct VideoInfo {
		DeviceId id;				//摄像头Id，唯一标示一路共享的摄像头视频。
		float x, y;					//此视频在左上角左边位置（上边位置），即x坐标，取值0.0~1.0，内部取值为：width * X（height * Y）
		float width, height;		//此视频宽度（高度）比例，取值0.0~1.0，内部取值为：width * W（height * H）
		uint16 zorder;				//多个视频覆盖时，前后位置，zorder越小，视频越在下层，会被覆盖；取值0-10
		uint16 alpha;				//（暂不使用）透明度
		IVideoRenderView::ScalingType render;	//（暂不使用）视频渲染到混流视频上的渲染模式

		VideoInfo() 
			: x(0.0f), y(0.0f), width(0.0f), height(0.0f), zorder(0), alpha(0), render(IVideoRenderView::Scale_Aspect_Fit) {}
		VideoInfo(const DeviceId& aId, float ax, float ay, float aw, float ah, uint16 az)
			: id(aId), x(ax), y(ay), width(aw), height(ah), zorder(az), alpha(0), render(IVideoRenderView::Scale_Aspect_Fit) {}
		bool update(float ax, float ay, float aw, float ah, uint16 az) {
			if (infosEqual(VideoInfo(id, ax, ay, aw, ah, az))) {
				return false;
			}
			x = ax; y = ay; width = aw; height = ah;
			zorder = az;
			return true;
		}
		bool infosEqual(const VideoInfo& r) const {
			if (r.x == x && r.y == y && r.width == width && r.height == height && r.zorder == zorder) {
				return true;
			}
			else {
				return false;
			}
		}
		bool isValid() const {
			return !id.empty();
		}
	};
	typedef std::vector<VideoInfo> VideosType;
	virtual Result selectMixedVideos4Exporter(const String& eId, const VideosType& mixedVideos) = 0;
	virtual IVideoMixer* getVideoMixer(const String& eId) = 0;

	//
	virtual Result pauseResume(const String& eId, bool isResume) = 0;

	virtual Result stopExporter(const String& eId) = 0;

	virtual Result stopExporterAll() = 0;

	//
	virtual Result getExporterInfo(const String& eId) = 0;

	virtual Result findExporterInfos(uint32& callId, uint32 begindex, uint32 ncount, const String& filter) = 0;

	virtual Result deleteExporter(const String& eId) = 0;

	virtual void* getCurExportItem(String eId) = 0;
protected:
	virtual ~IMStreamExporter() {}
};

RT_API_EXPORT const String toString(const IMStreamExporter::ViewInfo& obj);
RT_API_EXPORT const String toString(const IMStreamExporter::VideoInfo& obj);
RT_API_EXPORT const String toString(const IMStreamExporter::VideosType& obj);

NAMESPACE_TEE3_AVD_END

#endif//AVD_Module_Exporter_H__
