#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sstream>
#include<fstream>
#include<iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>

#include "comm.h"


using namespace std;


#define CHANEL_NUM                                      2 
#define STREAM_NUM                                      CHANEL_NUM * 2



#define MAX_PAYLOAD 1024

struct msg_struct
{
        unsigned video_width;
        unsigned video_height;
        unsigned video_freq;
        unsigned video_interlaced;
        unsigned video_hdcp;
        unsigned audio_freq;
        unsigned no_video;
};

unsigned int netlink_group_mask(unsigned int group);

unsigned int trans_fps( unsigned int s );

void HI3531D_Init(void);

void HI3531D_Exit(void);

HI_S32 start_AIO(HI_VOID);

HI_S32 stop_AIO(HI_VOID);
HI_S32 start_VO_H264();


