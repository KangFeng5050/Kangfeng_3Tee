#ifndef _TEE3VIDEOCAPTURER_H_
#define _TEE3VIDEOCAPTURER_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "comm.h"
#include "api/combase.h"
#include "api/common.h"
#include "api/room.h"
#include "api/avdengine.h"
#include "api/maudio.h"
#include "api/mvideo.h"
#include "api/musermanager.h"
#include "api/videocapture.h"
#include "api/devicemanager.h"

#include "MQTTClient.h"

using namespace std;
using namespace tee3;
using namespace tee3::avd;
long timestamp();

typedef tee3::avd::FakeVideoCapturer*	FakeVideoCapType;
class EncodedCaptureFromFile : public tee3::avd::FakeVideoCapturer::Listener {

public:
    EncodedCaptureFromFile(FourCC format);
    ~EncodedCaptureFromFile();
    void setStartImported(bool enalbe);
    virtual bool OnStart();
    virtual void OnStop();
    void IncomingH264Data(uint8* data, uint32 len);
    FakeVideoCapType GetCapture();

private:
	FakeVideoCapType	m_capture = NULL;    
    	bool			m_isStart;
    	FourCC              	m_format;
	long			m_time;
};

#endif 

