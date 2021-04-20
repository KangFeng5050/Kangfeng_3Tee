#ifndef RT_DeviceManager_H_
#define RT_DeviceManager_H_
#include "common.h"
namespace tee3 {
    namespace avd {
        /**
        *
		* 摄像头热插拔接口.
		*/
        class CameraListener {
        public:
            /** 摄像头插入通知
            *
            * @param[in] camera 摄像头信息。
            * @note 当系统中有摄像头设备插入时，此回调触发。
            */
            virtual void OnCameraPlugin(const tee3::avd::Camera camera) = 0;
            /** 摄像头拔出通知
            *
            * @param[in] camera 摄像头信息。
            * @note 当系统中有摄像头设备拔出时，此回调触发。
            */
            virtual void OnCameraPlugout(const tee3::avd::Camera camera) = 0;
        protected:
            virtual ~CameraListener() {}
        };
        /**
        *
		* 录音设备热插拔接口。
		*/
        class MicrophoneListener {
        public:
            /** 麦克风插入通知
            *
            * @param[in] microphone 麦克风信息。
            * @note 当系统中有麦克风设备插入时，此回调触发。
            */
            virtual void OnMicrophonePlugin(const tee3::avd::Microphone microphone) = 0;
            /** 麦克风拔出通知
            *
            * @param[in] microphone 麦克风信息。
            * @note 当系统中有麦克风设备拔出时，此回调触发。
            */
            virtual void OnMicrophonePlugout(const tee3::avd::Microphone microphone) = 0;
            /** 麦克风被改变通知
            *
            * @param[in] microphone 麦克风信息。
            * @note 当录音设备切换时，触发此回调。
            */
            virtual void OnRecordingMicrophoneChanged(const tee3::avd::Microphone microphone) = 0;
            /** 麦克风音量被改变通知
            *
            * @param[in] volume 音量值。
            * @note 当录音设备的音量大小被调整时，触发此回调。
            */
            virtual void OnMicrophoneVolumeChanged(uint32 volume) = 0;
        protected:
            virtual ~MicrophoneListener() {}
        };
        /**
        *
        * 播放设备热插拔接口。
        */
        class SpeakerListener {
        public:
            /** 扬声器插入通知
            *
            * @param[in] speaker 扬声器信息。
            * @note 当系统中有扬声器设备插入时，此回调触发。
            */
            virtual void OnSpeakerPlugin(const tee3::avd::Speaker speaker) = 0;
            /** 扬声器拔出通知
            *
            * @param[in] speaker 扬声器信息。
            * @note 当系统中有扬声器设备拔出时，此回调触发。
            */
            virtual void OnSpeakerPlugout(const tee3::avd::Speaker speaker) = 0;
            /** 扬声器被切换通知
            *
            * @param[in] speaker 扬声器信息。
            * @note 当播放设备切换时，触发此回调。
            */
            virtual void OnPlayoutSpeakerChanged(const tee3::avd::Speaker speaker) = 0;
            /** 扬声器音量被改变通知
            *
            * @param[in] volume 音量值。
            * @note 当播放设备的音量大小被调整时，触发此回调。
            */
            virtual void OnSpeakerVolumeChanged(uint32 volume) = 0;
        protected:
            virtual ~SpeakerListener() {}
        };
        /**
        *
        * PCM 16位 音频导入接口。该接口只在特定模式下使用，一般为嵌入式等有特殊音频驱动接口的平台准备。
        */
        class AudioInInterface {
        public:
            /** 音频采集，导入接口初始化
            *
            * @param[in,out] sampleRate 音频采样率。
            * @param[in,out] channels   音频通道数。
            * @param[in,out] processIntervalMS   数据读取时间间隔，默认为每10MS读取一次数据。
            * @return 返回初始化成功或者失败（true/false）。失败将导入无效，其他终端将不会听到该终端的声音。
            * @note sampleRate默认48000，channels默认为2。如果不更改将使用默认值，要求真实采集必须于此一致。
            */
            virtual bool onInit(int& sampleRate, int& channels, int& processIntervalMS) = 0;
            /** 音频采集，导入PCM数据回调接口
            *
            * @param[in] sampleRate 音频采样率。
            * @param[in] channels   音频通道数。
            * @param[in] data       音频数据导入内存地址，需要拷贝到改地址。
            * @param[in] len        音频数据导入长度，单位字节。
            * @return 真实拷贝的数据长度。
            * @note 每次传入10MS的数据量。
            * @note 长度计算公式:len = sampleRate/100*channels*2
            */
            virtual int onReadData(int sampleRate,
                                   int channels,
                                   char* data,
                                   unsigned int len) = 0;
        protected:
            virtual ~AudioInInterface() {}
        };
        /**
        *
        * PCM 16位 音频导出接口。该接口只在特定模式下使用，一般为嵌入式等有特殊音频驱动接口的平台准备。
        */
        class AudioOutInterface {
        public:
            /** 音频播放，导出接口初始化
            *
            * @param[in,out] sampleRate 音频采样率。
            * @param[in,out] channels   音频通道数。
            * @param[in,out] processIntervalMS   数据读取时间间隔，默认为每10MS读取一次数据。
            * @return 返回初始化成功或者失败（true/false）。失败将导出无效，该终端将听不到声音。
            * @note sampleRate默认48000，channels默认为2。如果不更改将使用默认值。
            */
            virtual bool onInit(int& sampleRate, int& channels, int& processIntervalMS) = 0;
            /** 音频播放，导出PCM数据回调接口
            *
            * @param[in] sampleRate 音频采样率。
            * @param[in] channels   音频通道数。
            * @param[in] data       音频数据导出内存地址，携带了真实的声音数据。
            * @param[in] len        音频数据导出长度，单位字节。
            * @note 每次传入10MS的数据量。
            * @note 长度计算公式:len = sampleRate/100*channels*2
            */
            virtual void onWriteData(int sampleRate,
                                     int channels,
                                     const int8* data,
                                     unsigned int len) = 0;
        protected:
            virtual ~AudioOutInterface() {}
        };
        /**
        *
        * 全局设备管理接口。
        */
        class RT_API_EXPORT GlobalDeviceManager {
        public:
            /** 获取设备管理接口句柄。
            *
            * @return 返回句柄。
            */
            static GlobalDeviceManager* Instance();
         
            /** 设置音频导入导出接口。
            *
            * @param[in] in     音频导入接口。
            * @param[in] out    音频导出接口。
            * @return （true/false）成功该接口有效，目前只有特定模式（嵌入式平台）下改接口有效，。
            */
            static bool SetAudioInterface(AudioInInterface* in, AudioOutInterface* out);
            static bool GetCameras(tee3::avd::CamerasType& devices);
            static bool GetMicphones(tee3::avd::MicrophonesType& devices);
            static bool GetFakeMicphone(tee3::avd::Microphone& micphone);
            static bool GetSpeakers(tee3::avd::SpeakersType& devices);
            static bool GetScreenWnds(tee3::avd::ScreensType& devices);
            static bool GetRecordingMicrophone(tee3::avd::Microphone& micphone);
            static bool SetRecordingMicrophone(const tee3::avd::Microphone&  micphone);
            static bool GetPlayoutSpeaker(tee3::avd::Speaker& speaker);
            static bool SetPlayoutSpeaker(const tee3::avd::Speaker& speaker);

            static bool SetPlayoutVolume(uint32 volume);
            static uint32 GetPlayoutVolume();
            static bool SetRecordingVolume(uint32 volume);
            static uint32 GetRecordingVolume();

            static tee3::avd::CapabilitiesType GetCapabilities(const std::string& id);

            virtual void AddCameraListener(CameraListener* cameraListener) = 0;
            virtual void AddMicrophoneListener(MicrophoneListener*	microphoneListener) = 0;
            virtual void AddSpeakerListener(SpeakerListener* cameraListener) = 0;

            virtual void RemoveCameraListener(CameraListener*	cameraListener) = 0;
            virtual void RemoveMicrophoneListener(MicrophoneListener*	microphoneListener) = 0;
            virtual void RemoveSpeakerListener(SpeakerListener*	speakerListener) = 0;






            virtual uint32 GetInputLevel() = 0;
            virtual uint32 GetOutputLevel() = 0;

            virtual bool RecordPlayStart() = 0;
            virtual bool RecordPlayStop() = 0;
            virtual bool PlayVoice(const std::string& file, bool loop, tee3::avd::FileFormats format) = 0;
            virtual bool IsPlayingVoice() = 0;
            virtual bool StopVoice() = 0;


            virtual ~GlobalDeviceManager() {};
        };

        /**
        *
        * 多份设备管理接口。
        *
        * 该接口在全局的基础上，把设备映射成了多份。允许在同一个房间了使用同一个真实的摄像头和麦克风。
        */
        class RT_API_EXPORT MultiDeviceManager {
        public:
            static MultiDeviceManager* Create();
            static void Destroy(MultiDeviceManager* mgr);
            /** 设置音频导入导出接口。
            *
            * @param[in] in     音频导入接口。
            * @param[in] out    音频导出接口。
            * @return （true/false）成功该接口有效，目前只有特定模式（嵌入式平台）下改接口有效。
            */
            static bool SetAudioInterface(AudioInInterface* in, AudioOutInterface* out);
            /** 获取视频设备。
            *
            * @param[out] devices   摄像头列表。
            * @return true获取设备成功。false获取失败。
            */
            virtual bool GetCameras(tee3::avd::CamerasType& devices) = 0;
            /** 获取录音设备。
            *
            * @param[out] devices   麦克风列表。
            * @return true获取设备成功。false获取失败。
            */
            virtual bool GetMicphones(tee3::avd::MicrophonesType& devices) = 0;
            /** 获取录音设备。
            *
            * @param[out] devices   麦克风列表。
            * @return true获取设备成功。false获取失败。
            */
            virtual bool GetFakeMicphone(tee3::avd::Microphone& micphone) = 0;
            virtual bool GetSpeakers(tee3::avd::SpeakersType& devices) = 0;
            virtual bool GetScreenWnds(tee3::avd::ScreensType& devices) = 0;
            virtual bool GetRecordingMicrophone(tee3::avd::Microphone& micphone) = 0;
            virtual bool SetRecordingMicrophone(const tee3::avd::Microphone&  micphone) = 0;
            virtual bool GetPlayoutSpeaker(tee3::avd::Speaker& speaker) = 0;
            virtual bool SetPlayoutSpeaker(const tee3::avd::Speaker& speaker) = 0;

            virtual bool SetPlayoutVolume(uint32 volume) = 0;
            virtual uint32 GetPlayoutVolume() = 0;
            virtual bool SetRecordingVolume(uint32 volume) = 0;
            virtual uint32 GetRecordingVolume() = 0;

            virtual tee3::avd::CapabilitiesType GetCapabilities(const std::string& id) = 0;

            virtual void AddCameraListener(CameraListener* cameraListener) = 0;
            virtual void AddMicrophoneListener(MicrophoneListener*	microphoneListener) = 0;
            virtual void AddSpeakerListener(SpeakerListener* cameraListener) = 0;

            virtual void RemoveCameraListener(CameraListener*	cameraListener) = 0;
            virtual void RemoveMicrophoneListener(MicrophoneListener*	microphoneListener) = 0;
            virtual void RemoveSpeakerListener(SpeakerListener*	speakerListener) = 0;






            virtual uint32 GetInputLevel() = 0;
            virtual uint32 GetOutputLevel() = 0;

            virtual bool RecordPlayStart() = 0;
            virtual bool RecordPlayStop() = 0;
            virtual bool PlayVoice(const std::string& file, bool loop, tee3::avd::FileFormats format) = 0;
            virtual bool IsPlayingVoice() = 0;
            virtual bool StopVoice() = 0;

        protected:
            virtual ~MultiDeviceManager() {};
        };
    }
}

#endif
