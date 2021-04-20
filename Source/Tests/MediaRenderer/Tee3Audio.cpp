#include "Tee3Audio.h"

/****************************AudioInProxy**********************************************/

AudioInProxy::AudioInProxy(int sampleRate, int channels)
        : m_sampleRate(sampleRate)
        , m_channels(channels)
        , m_count(0){
	inputsize = 0;
	A_pause = 0;
}

AudioInProxy::~AudioInProxy() {
}

bool AudioInProxy::onInit(int& sampleRate, int& channels, int& processIntervalMS){
        sampleRate = m_sampleRate;
        channels = m_channels;
        processIntervalMS = 100;
        return true;
}

int AudioInProxy::onReadData(int sampleRate,
                   int channels,
                   char* data,
                   unsigned int len)
{
	
		DeplyPar::Instance()->AudioIntSign = true;
		if(DeplyPar::Instance()->AudioImportdata.length() > len)
		{
			pthread_mutex_lock(DeplyPar::Instance()->GetAudioInLock()); 
			//printf("----wait,AudioImportdata.size = %d\n",AudioImportdata.length());
			string readbuffer = DeplyPar::Instance()->AudioImportdata.substr(0,len);
			memcpy(data,readbuffer.data(),len);
			DeplyPar::Instance()->AudioImportdata = DeplyPar::Instance()->AudioImportdata.erase(0,len);
			pthread_mutex_unlock(DeplyPar::Instance()->GetAudioInLock());
		}
	
	return 1;
}

/***************************************************************************/

AudioOutProxy::AudioOutProxy(int sampleRate, int channels)
        : m_sampleRate(sampleRate)
        , m_channels(channels)
        , m_count(0) {
}

AudioOutProxy::~AudioOutProxy() {
}

bool AudioOutProxy::onInit(int& sampleRate, int& channels, int& processIntervalMS){
        sampleRate = m_sampleRate;
        channels = m_channels;
        processIntervalMS = 100;
        return true;
}

void AudioOutProxy::onWriteData(int sampleRate,
                     int channels,
                     const int8* data,
                     unsigned int len)
{
	//printf("-------------------len = %d\n",len);
	HI3531D_AUDIO_SendAo((char*)data,len/2);
	HI3531D_AUDIO_SendAo((char*)data+len/2,len/2);   	
}
