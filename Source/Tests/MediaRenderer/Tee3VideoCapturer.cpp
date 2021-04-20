#include "Tee3VideoCapturer.h"

long timestamp()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

/*********************EncodedCaptureFromFile*********************************/

EncodedCaptureFromFile::EncodedCaptureFromFile(FourCC format)
        : m_capture(NULL)
        , m_isStart(false)
        , m_format(format) {
        m_capture = FakeVideoCapturer::Create(this, m_format);
	cout << "EncodedCaptureFromFile";
      
}

EncodedCaptureFromFile::~EncodedCaptureFromFile() {
        cout << "~EncodedCaptureFromFile";
        m_isStart = false;      
        FakeVideoCapturer::Destroy(m_capture);
	m_capture = NULL;
        
}

void EncodedCaptureFromFile::setStartImported(bool enalbe) {
        m_isStart = enalbe;
}

bool EncodedCaptureFromFile::OnStart() {
        cout << "EncodedCaptureFromFile::OnStart";
        m_isStart = true;
        return true;
}

void EncodedCaptureFromFile::OnStop() {
        cout << "EncodedCaptureFromFile::OnStop";
        m_isStart = false;
}

void EncodedCaptureFromFile::IncomingH264Data(uint8* data, uint32 len) {
        if(m_capture){
		m_time = timestamp();
        	int re = m_capture->inputEncodedFrame(m_time, 160, 128, data, len);
		printf("inputEncodedFrame return %d\n",re);
	}
}

FakeVideoCapType EncodedCaptureFromFile::GetCapture() {
        return m_capture;
}

