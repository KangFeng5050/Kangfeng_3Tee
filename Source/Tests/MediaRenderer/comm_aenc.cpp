#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <errno.h>


#define SAMPLE_DBG(s32Ret)\
do{\
    printf("s32Ret=%#x,fuc:%s,line:%d\n", s32Ret, __FUNCTION__, __LINE__);\
}while(0)

static HI_BOOL gs_bAiAnr = HI_FALSE;
static HI_BOOL gs_bAioReSample = HI_FALSE;
static AUDIO_RESAMPLE_ATTR_S *gs_pstAiReSmpAttr = NULL;
static AUDIO_RESAMPLE_ATTR_S *gs_pstAoReSmpAttr = NULL;
static AUDIO_SAMPLE_RATE_E enInSampleRate  = AUDIO_SAMPLE_RATE_BUTT;
static AUDIO_SAMPLE_RATE_E enOutSampleRate = AUDIO_SAMPLE_RATE_BUTT;


HI_S32 HI3531D_AUDIO_Ai(AUDIO_SAMPLE_RATE_E samplerate, PAYLOAD_TYPE_E enType)
{
	AI_VQE_CONFIG_S stAiVqeAttr;
	AIO_ATTR_S stAioAttr;
	HI_S32 s32Ret;
	stAioAttr.enSamplerate = samplerate;
	stAioAttr.enWorkmode = AIO_MODE_PCM_MASTER_STD;//AIO_MODE_I2S_MASTER;//
	stAioAttr.enBitwidth  = AUDIO_BIT_WIDTH_16;
	stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;	//单声道模式
	//stAioAttr.enSoundmode = AUDIO_SOUND_MODE_STEREO;
	stAioAttr.u32EXFlag = 1;
	stAioAttr.u32FrmNum = 50;
	stAioAttr.u32ChnCnt = 8;
	stAioAttr.u32ClkChnCnt = 8;
	stAioAttr.u32ClkSel = 0;

	stAioAttr.u32PtNumPerFrm = 1024;
	HI_S32 s32AiChnCnt = stAioAttr.u32ChnCnt;
	s32Ret = HI3531D_COMM_AUDIO_StartAi(1, s32AiChnCnt, &stAioAttr,enOutSampleRate,gs_bAioReSample,NULL,0);

	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_DBG(s32Ret);
		return HI_FAILURE;
	}
	return 0;
}

void HI3531D_AUDIO_Ai_Stop(AUDIO_DEV AiDev)
{
	HI_S32 s32AiChnCnt = 8;
	
	HI3531D_COMM_AUDIO_StopAi(AiDev, s32AiChnCnt, gs_bAiAnr, HI_TRUE);//gs_bAioReSample);
}

HI_S32 HI3531D_AUDIO_Aenc( PAYLOAD_TYPE_E enType, AUDIO_SAMPLE_RATE_E samplerate)
{
	HI_S32 count = 8;
	HI_U32 u32AencPtNumPerFrm = 1024;//SAMPLE_AUDIO_PTNUMPERFRM;
	HI_S32 s32Ret = HI3531D_COMM_AUDIO_StartAenc(count,u32AencPtNumPerFrm, enType);
	if ( s32Ret != HI_SUCCESS )
	{
	    SAMPLE_DBG(s32Ret);
	    return HI_FAILURE;
	}

	// dev 0 audio
	HI3531D_COMM_AUDIO_AencBindAi(1, 0, 0);
	HI3531D_COMM_AUDIO_AencBindAi(1, 1, 1);

	//dev 2 audio
	HI3531D_COMM_AUDIO_AencBindAi(1, 2, 2);
	HI3531D_COMM_AUDIO_AencBindAi(1, 3, 3);

	//line in 
	HI3531D_COMM_AUDIO_AencBindAi(1, 4, 4);
	HI3531D_COMM_AUDIO_AencBindAi(1, 5, 5);
	HI3531D_COMM_AUDIO_AencBindAi(1, 6, 6);
	HI3531D_COMM_AUDIO_AencBindAi(1, 7, 7);
	
	return 0;
}

void HI3531D_AUDIO_Aenc_Stop()
{    
	HI_S32 s32AencChnCnt = 8;

	// dev 0 audio
	HI3531D_COMM_AUDIO_AencUnbindAi(1, 0, 0);
	HI3531D_COMM_AUDIO_AencUnbindAi(1, 1, 1);

	//dev 2 audio
	HI3531D_COMM_AUDIO_AencUnbindAi(1, 2, 2);
	HI3531D_COMM_AUDIO_AencUnbindAi(1, 3, 3);

	//line in
	HI3531D_COMM_AUDIO_AencUnbindAi(1, 4, 4);
	HI3531D_COMM_AUDIO_AencUnbindAi(1, 5, 5);
	HI3531D_COMM_AUDIO_AencUnbindAi(1, 6, 6);
	HI3531D_COMM_AUDIO_AencUnbindAi(1, 7, 7);


	HI3531D_COMM_AUDIO_StopAenc(s32AencChnCnt);
}


