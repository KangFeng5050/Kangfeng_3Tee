#ifndef _TEE3AUDIO_H
#define _TEE3AUDIO_H

#include "deployfile.h"

using namespace std;
using namespace tee3;
using namespace tee3::avd;


/***********************AudioInProxy***********************************************/

class AudioInProxy : public tee3::avd::AudioInInterface {
public:
    AudioInProxy(int sampleRate, int channels);
    ~AudioInProxy();
    bool onInit(int& sampleRate, int& channels, int& processIntervalMS) override;
    int onReadData(int sampleRate, int channels, char* data, unsigned int len) override;

public:
	int inputsize;
	int A_pause;
   	int m_sampleRate;
    	int m_channels;
    	int m_count;
};

/*****************************AudioOutProxy*****************************************/

class AudioOutProxy : public tee3::avd::AudioOutInterface {
public:
    AudioOutProxy(int sampleRate, int channels);
    ~AudioOutProxy();
    bool onInit(int& sampleRate, int& channels, int& processIntervalMS)override;
    void onWriteData(int sampleRate, int channels, const int8* data, unsigned int len)override;
    
public:
    int m_sampleRate;
    int m_channels;
    int m_count;
};

#endif
