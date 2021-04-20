#ifndef RT_DeviceManager_H_
#define RT_DeviceManager_H_
#include "common.h"
namespace tee3 {
    namespace avd {
        /**
        *
		* ����ͷ�Ȳ�νӿ�.
		*/
        class CameraListener {
        public:
            /** ����ͷ����֪ͨ
            *
            * @param[in] camera ����ͷ��Ϣ��
            * @note ��ϵͳ��������ͷ�豸����ʱ���˻ص�������
            */
            virtual void OnCameraPlugin(const tee3::avd::Camera camera) = 0;
            /** ����ͷ�γ�֪ͨ
            *
            * @param[in] camera ����ͷ��Ϣ��
            * @note ��ϵͳ��������ͷ�豸�γ�ʱ���˻ص�������
            */
            virtual void OnCameraPlugout(const tee3::avd::Camera camera) = 0;
        protected:
            virtual ~CameraListener() {}
        };
        /**
        *
		* ¼���豸�Ȳ�νӿڡ�
		*/
        class MicrophoneListener {
        public:
            /** ��˷����֪ͨ
            *
            * @param[in] microphone ��˷���Ϣ��
            * @note ��ϵͳ������˷��豸����ʱ���˻ص�������
            */
            virtual void OnMicrophonePlugin(const tee3::avd::Microphone microphone) = 0;
            /** ��˷�γ�֪ͨ
            *
            * @param[in] microphone ��˷���Ϣ��
            * @note ��ϵͳ������˷��豸�γ�ʱ���˻ص�������
            */
            virtual void OnMicrophonePlugout(const tee3::avd::Microphone microphone) = 0;
            /** ��˷类�ı�֪ͨ
            *
            * @param[in] microphone ��˷���Ϣ��
            * @note ��¼���豸�л�ʱ�������˻ص���
            */
            virtual void OnRecordingMicrophoneChanged(const tee3::avd::Microphone microphone) = 0;
            /** ��˷��������ı�֪ͨ
            *
            * @param[in] volume ����ֵ��
            * @note ��¼���豸��������С������ʱ�������˻ص���
            */
            virtual void OnMicrophoneVolumeChanged(uint32 volume) = 0;
        protected:
            virtual ~MicrophoneListener() {}
        };
        /**
        *
        * �����豸�Ȳ�νӿڡ�
        */
        class SpeakerListener {
        public:
            /** ����������֪ͨ
            *
            * @param[in] speaker ��������Ϣ��
            * @note ��ϵͳ�����������豸����ʱ���˻ص�������
            */
            virtual void OnSpeakerPlugin(const tee3::avd::Speaker speaker) = 0;
            /** �������γ�֪ͨ
            *
            * @param[in] speaker ��������Ϣ��
            * @note ��ϵͳ�����������豸�γ�ʱ���˻ص�������
            */
            virtual void OnSpeakerPlugout(const tee3::avd::Speaker speaker) = 0;
            /** ���������л�֪ͨ
            *
            * @param[in] speaker ��������Ϣ��
            * @note �������豸�л�ʱ�������˻ص���
            */
            virtual void OnPlayoutSpeakerChanged(const tee3::avd::Speaker speaker) = 0;
            /** �������������ı�֪ͨ
            *
            * @param[in] volume ����ֵ��
            * @note �������豸��������С������ʱ�������˻ص���
            */
            virtual void OnSpeakerVolumeChanged(uint32 volume) = 0;
        protected:
            virtual ~SpeakerListener() {}
        };
        /**
        *
        * PCM 16λ ��Ƶ����ӿڡ��ýӿ�ֻ���ض�ģʽ��ʹ�ã�һ��ΪǶ��ʽ����������Ƶ�����ӿڵ�ƽ̨׼����
        */
        class AudioInInterface {
        public:
            /** ��Ƶ�ɼ�������ӿڳ�ʼ��
            *
            * @param[in,out] sampleRate ��Ƶ�����ʡ�
            * @param[in,out] channels   ��Ƶͨ������
            * @param[in,out] processIntervalMS   ���ݶ�ȡʱ������Ĭ��Ϊÿ10MS��ȡһ�����ݡ�
            * @return ���س�ʼ���ɹ�����ʧ�ܣ�true/false����ʧ�ܽ�������Ч�������ն˽������������ն˵�������
            * @note sampleRateĬ��48000��channelsĬ��Ϊ2����������Ľ�ʹ��Ĭ��ֵ��Ҫ����ʵ�ɼ������ڴ�һ�¡�
            */
            virtual bool onInit(int& sampleRate, int& channels, int& processIntervalMS) = 0;
            /** ��Ƶ�ɼ�������PCM���ݻص��ӿ�
            *
            * @param[in] sampleRate ��Ƶ�����ʡ�
            * @param[in] channels   ��Ƶͨ������
            * @param[in] data       ��Ƶ���ݵ����ڴ��ַ����Ҫ�������ĵ�ַ��
            * @param[in] len        ��Ƶ���ݵ��볤�ȣ���λ�ֽڡ�
            * @return ��ʵ���������ݳ��ȡ�
            * @note ÿ�δ���10MS����������
            * @note ���ȼ��㹫ʽ:len = sampleRate/100*channels*2
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
        * PCM 16λ ��Ƶ�����ӿڡ��ýӿ�ֻ���ض�ģʽ��ʹ�ã�һ��ΪǶ��ʽ����������Ƶ�����ӿڵ�ƽ̨׼����
        */
        class AudioOutInterface {
        public:
            /** ��Ƶ���ţ������ӿڳ�ʼ��
            *
            * @param[in,out] sampleRate ��Ƶ�����ʡ�
            * @param[in,out] channels   ��Ƶͨ������
            * @param[in,out] processIntervalMS   ���ݶ�ȡʱ������Ĭ��Ϊÿ10MS��ȡһ�����ݡ�
            * @return ���س�ʼ���ɹ�����ʧ�ܣ�true/false����ʧ�ܽ�������Ч�����ն˽�������������
            * @note sampleRateĬ��48000��channelsĬ��Ϊ2����������Ľ�ʹ��Ĭ��ֵ��
            */
            virtual bool onInit(int& sampleRate, int& channels, int& processIntervalMS) = 0;
            /** ��Ƶ���ţ�����PCM���ݻص��ӿ�
            *
            * @param[in] sampleRate ��Ƶ�����ʡ�
            * @param[in] channels   ��Ƶͨ������
            * @param[in] data       ��Ƶ���ݵ����ڴ��ַ��Я������ʵ���������ݡ�
            * @param[in] len        ��Ƶ���ݵ������ȣ���λ�ֽڡ�
            * @note ÿ�δ���10MS����������
            * @note ���ȼ��㹫ʽ:len = sampleRate/100*channels*2
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
        * ȫ���豸����ӿڡ�
        */
        class RT_API_EXPORT GlobalDeviceManager {
        public:
            /** ��ȡ�豸����ӿھ����
            *
            * @return ���ؾ����
            */
            static GlobalDeviceManager* Instance();
         
            /** ������Ƶ���뵼���ӿڡ�
            *
            * @param[in] in     ��Ƶ����ӿڡ�
            * @param[in] out    ��Ƶ�����ӿڡ�
            * @return ��true/false���ɹ��ýӿ���Ч��Ŀǰֻ���ض�ģʽ��Ƕ��ʽƽ̨���¸Ľӿ���Ч����
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
        * ����豸����ӿڡ�
        *
        * �ýӿ���ȫ�ֵĻ����ϣ����豸ӳ����˶�ݡ�������ͬһ��������ʹ��ͬһ����ʵ������ͷ����˷硣
        */
        class RT_API_EXPORT MultiDeviceManager {
        public:
            static MultiDeviceManager* Create();
            static void Destroy(MultiDeviceManager* mgr);
            /** ������Ƶ���뵼���ӿڡ�
            *
            * @param[in] in     ��Ƶ����ӿڡ�
            * @param[in] out    ��Ƶ�����ӿڡ�
            * @return ��true/false���ɹ��ýӿ���Ч��Ŀǰֻ���ض�ģʽ��Ƕ��ʽƽ̨���¸Ľӿ���Ч��
            */
            static bool SetAudioInterface(AudioInInterface* in, AudioOutInterface* out);
            /** ��ȡ��Ƶ�豸��
            *
            * @param[out] devices   ����ͷ�б�
            * @return true��ȡ�豸�ɹ���false��ȡʧ�ܡ�
            */
            virtual bool GetCameras(tee3::avd::CamerasType& devices) = 0;
            /** ��ȡ¼���豸��
            *
            * @param[out] devices   ��˷��б�
            * @return true��ȡ�豸�ɹ���false��ȡʧ�ܡ�
            */
            virtual bool GetMicphones(tee3::avd::MicrophonesType& devices) = 0;
            /** ��ȡ¼���豸��
            *
            * @param[out] devices   ��˷��б�
            * @return true��ȡ�豸�ɹ���false��ȡʧ�ܡ�
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
