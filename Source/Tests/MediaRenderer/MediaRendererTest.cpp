#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include<ctime>
#include <sstream>
#include<fstream>
#include<iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>

#include "Hi3531DMedia.h"
#include "UPNPDevice.h"

extern pthread_t	StartMQTT_Num;	//MQTT线程号

/*****************************************************************************
**************************HI3531D section funciton****************************
*****************************************************************************/
#define CHANEL_NUM                                      2 
#define STREAM_NUM                                      CHANEL_NUM * 2
extern bool g_vi_need_flush[CHANEL_NUM];
bool g_enc_reset[CHANEL_NUM]={0};
bool g_vi_need_flush[CHANEL_NUM]={0};
bool g_enc_reset_end[CHANEL_NUM]={0};
bool g_mpp_reset=0;
bool g_enc_run=0;
int g_vi_w[CHANEL_NUM];
int g_vi_h[CHANEL_NUM];
int g_vi_fs[CHANEL_NUM]={60,60};
string g_vi_input_type[CHANEL_NUM];
#define MAX_PAYLOAD 1024
class msg_bridge
{
private:
        int m_sock_fd;
        int m_chn_flag;
        bool m_thread_run;
        boost::shared_ptr<boost::thread> m_thread;
public:
        int open_bridge(int netlink_drv)
        {
                int netlink_d ;
                netlink_d = 21;
                netlink_d += netlink_drv;
                m_chn_flag = netlink_drv;
                int ret = 0;
//              printf("m_chn_flag = %d, port = %d\n", m_chn_flag, netlink_d);
                m_sock_fd = socket( PF_NETLINK, SOCK_RAW, netlink_d );
                if( m_sock_fd < 0 )
                {
                        printf("create netlink socket error\n");
                        return -1;
                }
                struct sockaddr_nl src_addr;
                memset( &src_addr, 0, sizeof(src_addr));
                src_addr.nl_family = PF_NETLINK;
                src_addr.nl_pid = 0;
                src_addr.nl_groups = netlink_group_mask(5);
                ret = bind( m_sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
                if ( ret < 0 )
                {
                        close( m_sock_fd );
                        printf("bind netlink socket failed\n");
                        return -1;
                }
                m_thread_run = true;
                m_thread.reset( new boost::thread( boost::bind( &msg_bridge::door, this ) ) );
                return 0;
        }
        int close_bridge(void)
        {
                m_thread_run = false;
                close( m_sock_fd );
                m_thread->join();
                return 0;
        }
private:
    void door()
        {
                struct iovec  iov;
                struct msghdr msg;
                msg_struct ms;
                struct nlmsghdr* nlh = (nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
                if( !nlh )
                {
                        close( m_sock_fd );
                        printf("failed\n");
                        return;
                }
                memset( nlh, 0, NLMSG_SPACE(MAX_PAYLOAD) );
                iov.iov_base = (void *)nlh;
                iov.iov_len  = NLMSG_SPACE(MAX_PAYLOAD);
                memset( &msg, 0, sizeof(msg) );
                msg.msg_iov    = &iov;
                msg.msg_iovlen = 1;
                memset(&ms, 0, sizeof(ms));
                int hdmi_status = -1;
                while (m_thread_run)
                {
                        recvmsg( m_sock_fd, &msg, 0 );
                        memcpy ( &ms, NLMSG_DATA(nlh), sizeof(ms));
                        if(ms.video_width != 0)
                        {
                                unsigned int ifps = trans_fps(ms.video_freq);
                                if(ms.video_width == 1920 && ms.video_height == 540)ms.video_interlaced = 1;
                                if(ms.video_width == 1280 && ms.video_height == 360)ms.video_interlaced = 1;
                                if(ms.video_width == 720 && ms.video_height == 288)ms.video_interlaced = 1;
                                if(ms.video_width == 720 && ms.video_height == 240)ms.video_interlaced = 1;
                                string inputtype = ms.video_interlaced?"i":"p";
                                ms.video_height = (ms.video_interlaced ? ms.video_height*2 : ms.video_height );
                                if( g_vi_w[m_chn_flag] != ms.video_width|| g_vi_h[m_chn_flag] != ms.video_height|| g_vi_fs[m_chn_flag] != ifps || g_vi_input_type[m_chn_flag] != inputtype)
                                {
                                        printf("input chn(%d) param change, will be restart  0, g_vi_need_flush = %d, g_enc_reset_end = %d\n", m_chn_flag,g_vi_need_flush[m_chn_flag],g_enc_reset_end[m_chn_flag]);
                                        if(g_vi_need_flush[m_chn_flag] == 0 && g_enc_reset_end[m_chn_flag] == 1){
                                                if(ms.video_width)
                                                        g_vi_w[m_chn_flag] = ms.video_width;
                                                if(ms.video_height)
                                                        g_vi_h[m_chn_flag] = ms.video_height;
                                                g_vi_input_type[m_chn_flag] = inputtype;
                                                if(ifps)
                                                        g_vi_fs[m_chn_flag]  = ifps;
                                                g_vi_need_flush[m_chn_flag] = 1;
                                        }
                                }
                                if(hdmi_status == 0)
                                {
                                        g_vi_need_flush[m_chn_flag] = 1;
                                }
                        }
                        hdmi_status = ms.video_width;
			usleep(1000*100);
                }
        }
};
pthread_t ThreadStartRcvStream = 0;
int g_revThread_flag = 0;
HI_S32 start_RecvStreamThread()
{
	if(g_revThread_flag == 1)
		return 0;
	printf("first pthread\n");
	if(pthread_create(&ThreadStartRcvStream,NULL,HI3531D_COMM_VENC_GetVencStreamProc,NULL) != 0)
	{
		printf("Error:start_RecvStreamThread creation failed !!! \n");
		return -1;
	}
	g_revThread_flag = 1;
	return 0;
}
HI_S32 start_venc(HI_VOID)
{
	int Ret = -1;
	SAMPLE_VI_MODE_E enViMode = SAMPLE_VI_MODE_4_4Kx2K;
	int vi_interval, vchn_interval;
	vi_interval = 2;
	vchn_interval = 4;	
	VIDEO_NORM_E gs_enNorm = VIDEO_ENCODING_MODE_PAL;
	PAYLOAD_TYPE_E enPayLoad[2]= {PT_H264,PT_H265};
	vi_venc_par v[CHANEL_NUM];
	vi_venc_par ve[CHANEL_NUM];
	for(int i = 0; i < CHANEL_NUM; i++){
		if(g_enc_reset[i] == 0){
			continue;
		}
		memset(&v[i], 0, sizeof(v[i]));
		v[i].vi_cap_x = 0;
		v[i].vi_cap_y = 0;
		v[i].vi_cap_width = g_vi_w[i];
		v[i].vi_cap_height = g_vi_h[i];
		if (v[i].vi_cap_width == 0)
			v[i].vi_cap_width = 1920;
		if (v[i].vi_cap_height == 0)
			v[i].vi_cap_height = 1080;
		v[i].vi_framerate = g_vi_fs[i];
		v[i].venc_FluctuateLevel = 1;
		v[i].vi_interlaced = 0;
		if(g_vi_input_type[i] == "i")
		{	
			v[i].vi_interlaced = 1;
		}
		v[i].venc_h264framerate =20;
		v[i].venc_h265framerate =20;
		v[i].venc_gop = 30;
		v[i].venc_width = 1920;
		v[i].venc_height = 1080;
		if (v[i].venc_width == 0)
			v[i].venc_width = 1920;
		if (v[i].venc_height == 0)
			v[i].venc_height = 1080;
		v[i].venc_profile = 1;
		v[i].venc_rc_mode = SAMPLE_RC_CBR;
		v[i].venc_bitrate = 3200;
		v[i].venc_aspect_ration = 0;
		v[i].enType = enPayLoad[i]; 
		memcpy(&ve[i], &v[i], sizeof(v[i]));
	}	
	for(int vi_start = 0; vi_start < CHANEL_NUM; vi_start++){
		if(g_enc_reset[vi_start] == 0){
			continue;
		}
		HI3531D_COMM_VI_Start((vi_start)*vi_interval, (vi_start)*vi_interval*vchn_interval, enViMode, gs_enNorm, v[vi_start]);
		HI3531D_COMM_VPSS_Start(vi_start,1,v[vi_start],ve[vi_start],NULL);
		HI3531D_COMM_VI_BindVpss((vi_start)*vi_interval, (vi_start)*vi_interval*vchn_interval, vi_start);
	}
	for(int venc_start = 0; venc_start < CHANEL_NUM; venc_start++){
		if(g_enc_reset[venc_start] == 0){
			continue;
		}
		//HI3531D_COMM_VENC_Start(venc_start,enPayLoad[1] ,v[venc_start]);//H265
		HI3531D_COMM_VENC_Start(venc_start,enPayLoad[0] ,v[venc_start]);//H264
		HI3531D_COMM_VENC_BindVpss(venc_start,venc_start,0);
		return HI_SUCCESS;
	}
}
HI_S32 stop_venc(HI_VOID)
{
	for(int i = 0; i < CHANEL_NUM; i++){
		if(g_enc_reset[i] == 0){
			continue;
		}
	}
	int vi_interval, vchn_interval;
	vi_interval = 2;
	vchn_interval = 4;
	for(int venc_stop = 0;venc_stop < CHANEL_NUM; venc_stop++){
		if(g_enc_reset[venc_stop] == 0)
			continue;
		HI3531D_COMM_VENC_UnBindVpss(venc_stop,venc_stop,0);
		HI3531D_COMM_VENC_Stop(venc_stop);
	}
	for(int vi_stop = 0; vi_stop < CHANEL_NUM; vi_stop++){
		if(g_enc_reset[vi_stop] == 0)
			continue;
		HI3531D_COMM_VI_UnBindVpss((vi_stop)*vi_interval, (vi_stop)*vi_interval*vchn_interval, vi_stop);
		HI3531D_COMM_VPSS_Stop(vi_stop,1);
		HI3531D_COMM_VI_Stop((vi_stop)*vi_interval);
	}
	return HI_SUCCESS;
}
msg_bridge *g_msg_bridge[CHANEL_NUM];
int init_drv()
{
	int drv_index;
	for(int i = 0; i < CHANEL_NUM; i++){
		g_msg_bridge[i] = new msg_bridge();
		if (g_msg_bridge[i]->open_bridge(i) < 0)
		{
			printf("open_bridge [ %d] failed\n",i);
			return -1;
		}
	}
	return 0;
}
void exit_drv()
{
	for(int i = 0; i < CHANEL_NUM; i++){
		g_msg_bridge[i]->close_bridge();
		delete g_msg_bridge[i];
	}
}
void* Hisipreadth(void*)
{
	if(init_drv() < 0)
	{
		printf( "init_drv failed\n" );
		system("reboot");
	}
	for(int i = 0; i < CHANEL_NUM;i++){
                g_vi_need_flush[i] = 1;
        }	
	HI3531D_Init();
	start_VO_H264();
	g_mpp_reset=1;
	while(1)
	{
		if(g_mpp_reset == 1){
			start_AIO();		
		}
		start_venc();
		for(int i = 0; i < CHANEL_NUM; i++){
			if(g_enc_reset[i] && g_revThread_flag){
				g_enc_run = 0;
			}
			g_enc_reset[i] = 0;   //for stop video channel restart in thread.
			g_enc_reset_end[i] = 1;   //for stop video channel restart in thread.
		}
		g_mpp_reset = 0;
		start_RecvStreamThread(); //start recv stream
		int loop_flag = 1;
		while(loop_flag){
			usleep(1000);
			if(g_mpp_reset == 1){
				for(int i = 0; i < CHANEL_NUM; i++){
					g_enc_reset[i] = 1;
				}
			}
			for(int i = 0; i < CHANEL_NUM; i++){
				if(g_vi_need_flush[i] == 1){
					g_enc_reset[i] = 1;
					g_vi_need_flush[i] = 0;
					printf( "[main]g_vi_need_flush(%d) = %d\n", i,  g_vi_need_flush[i]);
				}
				if(g_enc_reset[i] == 1){
					g_enc_reset_end[i] = 0;
					loop_flag = 0;
				}
			}
		}
		stop_venc();
		if(g_mpp_reset == 1){
			stop_AIO();
		}
	}
}

int main(int argc, char *argv[])
{
	DeplyPar::Instance()->RebootProcess = false;
	DeplyPar::Instance()->ISRebootDev = false;
	pthread_t ThreadHaisi_Num = 0;	//H3531D线程号
	pthread_mutex_init(DeplyPar::Instance()->GetAudioInLock(), NULL);
	if(pthread_create(&ThreadHaisi_Num,NULL,Hisipreadth,NULL) != 0){
		printf("Hisipreadth faild\n");
	}
	if(DeplyPar::Instance()->loadProfileFile())
	{
		DeplyPar::Instance()->PrintProInfo();
		MClient::Instance()->InitEngine();
		DeplyPar::Instance()->Tee3_SetCamera();
		if(pthread_create(&StartMQTT_Num,NULL,RunToMQTT,NULL) == 0){
			printf("MQTTPreadth success,\n");
		}
	}
	PLT_UPnP upnp;
	PLT_DeviceHostReference* device = NULL;
	upnp.Start();
	char DeviceID[20]={0};
	srand((unsigned)time(NULL));
	sprintf(DeviceID,"Linux-Device-%d",(rand()%10000));
	printf("Device ID = %s\n",DeviceID);
	device = new PLT_DeviceHostReference(new upnpDevice(DeplyPar::Instance()->GetClientname().c_str(), false, DeviceID));
    	upnp.AddDevice(*device);
	while (1){		
		if(DeplyPar::Instance()->ISRebootDev)
		{
			upnp.RemoveDevice(*device);
			printf("******************reboot*****************\n");
			system("reboot");
		}
	}
	if(DeplyPar::Instance()->IsStartMQTT == true){
		MQTTClient_unsubscribe(*DeplyPar::Instance()->GetMyclient(), 
					DeplyPar::Instance()->GetClienttopic().c_str());
		MQTTClient_disconnect(*DeplyPar::Instance()->GetMyclient(), 1000);
		MQTTClient_destroy(DeplyPar::Instance()->GetMyclient());//释放客户端
		pthread_cancel(StartMQTT_Num);
		DeplyPar::Instance()->IsStartMQTT = false;
	}	
	HI3531D_Exit();
	exit_drv();
	upnp.Stop();
	pthread_join(ThreadHaisi_Num,NULL);
	pthread_mutex_destroy(DeplyPar::Instance()->GetAudioInLock());
	return 0;
}
