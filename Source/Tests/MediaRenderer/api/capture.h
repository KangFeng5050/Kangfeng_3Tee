#ifndef RT_Capture_H_
#define RT_Capture_H_

#include "common.h"

namespace tee3 {
    namespace avd {
        class IVideoRender;
        class RT_API_EXPORT IVideoCapture {
        public:
            static bool EnumerateCammeras(CamerasType& cameras);
            static IVideoCapture*  Create(const Camera& cam);
            static void Destroy(IVideoCapture* capture);
            static std::string GetVideoDeviceRealId(const DeviceId& id);
            virtual Result Start() = 0;
            virtual void Stop() = 0;

            virtual Result SetResolution(int fps, int width, int height) = 0;
            virtual void AttachRender(IVideoRender* render) = 0;
            virtual void DetachRender(IVideoRender* render) = 0;

            virtual const Camera& GetDevice() = 0;

        protected:
            virtual ~IVideoCapture() {}
        };
    } // namespace avd
} // namespace tee3
#endif//RT_Capture_H_
