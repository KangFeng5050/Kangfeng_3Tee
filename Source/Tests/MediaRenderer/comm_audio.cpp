/******************************************************************************
  A simple program of Hisilicon HI3516 audio input/output/encoder/decoder implementation.
  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include "comm.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define ACODEC_FILE     "/dev/acodec"

#define AUDIO_ADPCM_TYPE ADPCM_TYPE_DVI4/* ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4*/
#define G726_BPS MEDIA_G726_40K         /* MEDIA_G726_16K, MEDIA_G726_24K ... */

typedef struct tagSAMPLE_AENC_S
{
    HI_BOOL bStart;
    pthread_t stAencPid;
    HI_S32  AeChn;
    HI_S32  AdChn;
    FILE    *pfd;
    HI_BOOL bSendAdChn;
} SAMPLE_AENC_S;

typedef struct tagSAMPLE_AI_S
{
    HI_BOOL bStart;
    HI_S32  AiDev;
    HI_S32  AiChn;
    HI_S32  AencChn;
    HI_S32  AoDev;
    HI_S32  AoChn;
    HI_BOOL bSendAenc;
    HI_BOOL bSendAo;
    pthread_t stAiPid;
} SAMPLE_AI_S;

typedef struct tagSAMPLE_ADEC_S
{
    HI_BOOL bStart;
    HI_S32 AdChn; 
    FILE *pfd;
    pthread_t stAdPid;
} SAMPLE_ADEC_S;

typedef struct tagSAMPLE_AO_S
{
	AUDIO_DEV AoDev;
	HI_BOOL bStart;
	pthread_t stAoPid;
}SAMPLE_AO_S;

static SAMPLE_AI_S gs_stSampleAi[AI_DEV_MAX_NUM*AIO_MAX_CHN_NUM];
static SAMPLE_AENC_S gs_stSampleAenc[AENC_MAX_CHN_NUM];
static SAMPLE_ADEC_S gs_stSampleAdec[ADEC_MAX_CHN_NUM];
static SAMPLE_AO_S   gs_stSampleAo[AO_DEV_MAX_NUM];


/******************************************************************************
* function : get frame from Ai, send it  to Aenc or Ao
******************************************************************************/
void *HI3531D_COMM_AUDIO_AiProc(void *parg)
{
    HI_S32 s32Ret;
    HI_S32 AiFd;
    SAMPLE_AI_S *pstAiCtl = (SAMPLE_AI_S *)parg;
    AUDIO_FRAME_S stFrame; 
	AEC_FRAME_S   stAecFrm;
    fd_set read_fds;
    struct timeval TimeoutVal;
    AI_CHN_PARAM_S stAiChnPara;

    s32Ret = HI_MPI_AI_GetChnParam(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return NULL;
    }
    
    stAiChnPara.u32UsrFrmDepth = 30;
    
    s32Ret = HI_MPI_AI_SetChnParam(pstAiCtl->AiDev, pstAiCtl->AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return NULL;
    }
    
    FD_ZERO(&read_fds);
    AiFd = HI_MPI_AI_GetFd(pstAiCtl->AiDev, pstAiCtl->AiChn);
    FD_SET(AiFd,&read_fds);

    while (pstAiCtl->bStart)
    {     
        TimeoutVal.tv_sec = 1;
        TimeoutVal.tv_usec = 0;
        
        FD_ZERO(&read_fds);
        FD_SET(AiFd,&read_fds);
        
        s32Ret = select(AiFd+1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0) 
        {
            break;
        }
        else if (0 == s32Ret) 
        {
            printf("%s: get ai frame select time out\n", __FUNCTION__);
            break;
        }        
        
        if (FD_ISSET(AiFd, &read_fds))
        {
            /* get frame from ai chn */
			memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));
            s32Ret = HI_MPI_AI_GetFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm, HI_FALSE);
            if (HI_SUCCESS != s32Ret )
            {
            	#if 0
                printf("%s: HI_MPI_AI_GetFrame(%d, %d), failed with %#x!\n",\
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = HI_FALSE;
                return NULL;
				#else
				continue;
				#endif
            }

            /* send frame to encoder */
            if (HI_TRUE == pstAiCtl->bSendAenc)
            {
                s32Ret = HI_MPI_AENC_SendFrame(pstAiCtl->AencChn, &stFrame, &stAecFrm);
                if (HI_SUCCESS != s32Ret )
                {
                    printf("%s: HI_MPI_AENC_SendFrame(%d), failed with %#x!\n",\
                           __FUNCTION__, pstAiCtl->AencChn, s32Ret);
                    pstAiCtl->bStart = HI_FALSE;
                    return NULL;
                }
            }
            
            /* send frame to ao */
            if (HI_TRUE == pstAiCtl->bSendAo)
            {
                s32Ret = HI_MPI_AO_SendFrame(pstAiCtl->AoDev, pstAiCtl->AoChn, &stFrame, 1000);
                if (HI_SUCCESS != s32Ret )
                {
                    printf("%s: HI_MPI_AO_SendFrame(%d, %d), failed with %#x!\n",\
                           __FUNCTION__, pstAiCtl->AoDev, pstAiCtl->AoChn, s32Ret);
                    pstAiCtl->bStart = HI_FALSE;
                    return NULL;
                }
                
            }

            /* finally you must release the stream */
            s32Ret = HI_MPI_AI_ReleaseFrame(pstAiCtl->AiDev, pstAiCtl->AiChn, &stFrame, &stAecFrm);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n",\
                       __FUNCTION__, pstAiCtl->AiDev, pstAiCtl->AiChn, s32Ret);
                pstAiCtl->bStart = HI_FALSE;
                return NULL;
            }
            
        }
    }
    
    pstAiCtl->bStart = HI_FALSE;
    return NULL;
}

/******************************************************************************
* function : get stream from Aenc, send it  to Adec & save it to file
******************************************************************************/
void *HI3531D_COMM_AUDIO_AencProc(void *parg)
{
    HI_S32 s32Ret;
    HI_S32 AencFd;
    SAMPLE_AENC_S *pstAencCtl = (SAMPLE_AENC_S *)parg;
    AUDIO_STREAM_S stStream;
    fd_set read_fds;
    struct timeval TimeoutVal;
    
    FD_ZERO(&read_fds);    
    AencFd = HI_MPI_AENC_GetFd(pstAencCtl->AeChn);
    FD_SET(AencFd, &read_fds);
    
    while (pstAencCtl->bStart)
    {     
        TimeoutVal.tv_sec = 1;
        TimeoutVal.tv_usec = 0;
        
        FD_ZERO(&read_fds);
        FD_SET(AencFd,&read_fds);
        
        s32Ret = select(AencFd+1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0) 
        {
            break;
        }
        else if (0 == s32Ret) 
        {
            printf("%s: get aenc stream select time out\n", __FUNCTION__);
            break;
        }
        
        if (FD_ISSET(AencFd, &read_fds))
        {
            /* get stream from aenc chn */
            s32Ret = HI_MPI_AENC_GetStream(pstAencCtl->AeChn, &stStream, HI_FALSE);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AENC_GetStream(%d), failed with %#x!\n",\
                       __FUNCTION__, pstAencCtl->AeChn, s32Ret);
                pstAencCtl->bStart = HI_FALSE;
                return NULL;
            }

            /* send stream to decoder and play for testing */
            if (HI_TRUE == pstAencCtl->bSendAdChn)
            {
                s32Ret = HI_MPI_ADEC_SendStream(pstAencCtl->AdChn, &stStream, HI_TRUE);
                if (HI_SUCCESS != s32Ret )
                {
                    printf("%s: HI_MPI_ADEC_SendStream(%d), failed with %#x!\n",\
                           __FUNCTION__, pstAencCtl->AdChn, s32Ret);
                    pstAencCtl->bStart = HI_FALSE;
                    return NULL;
                }
            }
			
		if(pstAencCtl->AeChn < 2){
			if(NULL != stStream.pStream){
				;//need to do
			}else{
				printf("HI_MPI_AENC_GetStream get NULL chn:%d",pstAencCtl->AeChn);
			}
		}
            
            /* save audio stream to file */
//            fwrite(stStream.pStream,1,stStream.u32Len, pstAencCtl->pfd);

            /* finally you must release the stream */
            s32Ret = HI_MPI_AENC_ReleaseStream(pstAencCtl->AeChn, &stStream);
            if (HI_SUCCESS != s32Ret )
            {
                printf("%s: HI_MPI_AENC_ReleaseStream(%d), failed with %#x!\n",\
                       __FUNCTION__, pstAencCtl->AeChn, s32Ret);
                pstAencCtl->bStart = HI_FALSE;
                return NULL;
            }
        }    
    }
    
    fclose(pstAencCtl->pfd);
    pstAencCtl->bStart = HI_FALSE;
    return NULL;
}

/******************************************************************************
* function : get stream from file, and send it  to Adec
******************************************************************************/
void *HI3531D_COMM_AUDIO_AdecProc(void *parg)
{
    HI_S32 s32Ret;
    AUDIO_STREAM_S stAudioStream;    
    HI_U32 u32Len = 640;
    HI_U32 u32ReadLen;
    HI_S32 s32AdecChn;
    HI_U8 *pu8AudioStream = NULL;
    SAMPLE_ADEC_S *pstAdecCtl = (SAMPLE_ADEC_S *)parg;    
    FILE *pfd = pstAdecCtl->pfd;
    s32AdecChn = pstAdecCtl->AdChn;
    
    pu8AudioStream = (HI_U8*)malloc(sizeof(HI_U8)*MAX_AUDIO_STREAM_LEN);
    if (NULL == pu8AudioStream)
    {
        printf("%s: malloc failed!\n", __FUNCTION__);
        return NULL;
    }

    while (HI_TRUE == pstAdecCtl->bStart)
    {
        /* read from file */
        stAudioStream.pStream = pu8AudioStream;
        u32ReadLen = fread(stAudioStream.pStream, 1, u32Len, pfd);
        if (u32ReadLen <= 0)
        {            
            fseek(pfd, 0, SEEK_SET);/*read file again*/
            continue;
        }

        /* here only demo adec streaming sending mode, but pack sending mode is commended */
        stAudioStream.u32Len = u32ReadLen;
        s32Ret = HI_MPI_ADEC_SendStream(s32AdecChn, &stAudioStream, HI_TRUE);
        if(HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_ADEC_SendStream(%d) failed with %#x!\n",\
                   __FUNCTION__, s32AdecChn, s32Ret);
            break;
        }
    }
    
    free(pu8AudioStream);
    pu8AudioStream = NULL;
    fclose(pfd);
    pstAdecCtl->bStart = HI_FALSE;
    return NULL;
}

/******************************************************************************
* function : set ao volume 
******************************************************************************/
void *HI3531D_COMM_AUDIO_AoVolProc(void *parg)
{
    HI_S32 s32Ret;
    HI_S32 s32Volume;
    AUDIO_DEV AoDev;
    AUDIO_FADE_S stFade;
    SAMPLE_AO_S *pstAoCtl = (SAMPLE_AO_S *)parg;
    AoDev = pstAoCtl->AoDev;
	
    while(pstAoCtl->bStart)
    {	
        for(s32Volume = 0; s32Volume <=6; s32Volume++)
        {
            s32Ret = HI_MPI_AO_SetVolume( AoDev, s32Volume);
            if(HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n",\
                    __FUNCTION__, AoDev, s32Ret);
            }
            printf("\rset volume %d          ", s32Volume);
            fflush(stdout);
            sleep(2);		
        }

        for(s32Volume = 5; s32Volume >=-15; s32Volume--)
        {
            s32Ret = HI_MPI_AO_SetVolume( AoDev, s32Volume);
            if(HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n",\
                    __FUNCTION__, AoDev, s32Ret);
            }
            printf("\rset volume %d          ", s32Volume);
            fflush(stdout);
            sleep(2);		
        }

        for(s32Volume = -14; s32Volume >=0; s32Volume++)
        {
            s32Ret = HI_MPI_AO_SetVolume( AoDev, s32Volume);
            if(HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n",\
                   __FUNCTION__, AoDev, s32Ret);
            }
            printf("\rset volume %d          ", s32Volume);
            fflush(stdout);
            sleep(2);		
        }

        stFade.bFade         = HI_TRUE;
        stFade.enFadeInRate  = AUDIO_FADE_RATE_128;
        stFade.enFadeOutRate = AUDIO_FADE_RATE_128;

        s32Ret = HI_MPI_AO_SetMute(AoDev, HI_TRUE, &stFade);
        if(HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n",\
            __FUNCTION__, AoDev, s32Ret);
        }
        printf("\rset Ao mute            ");
        fflush(stdout);
        sleep(2);

        s32Ret = HI_MPI_AO_SetMute(AoDev, HI_FALSE, NULL);
        if(HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_SetVolume(%d), failed with %#x!\n",\
            __FUNCTION__, AoDev, s32Ret);
        }
        printf("\rset Ao unmute          ");
        fflush(stdout);
        sleep(2);
    }
    return NULL;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to ao
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    SAMPLE_AI_S *pstAi = NULL;
    
    pstAi = &gs_stSampleAi[AiDev*AIO_MAX_CHN_NUM + AiChn];
    pstAi->bSendAenc = HI_FALSE;
    pstAi->bSendAo = HI_TRUE;
    pstAi->bStart= HI_TRUE;
    pstAi->AiDev = AiDev;
    pstAi->AiChn = AiChn;
    pstAi->AoDev = AoDev;
    pstAi->AoChn = AoChn;

    pthread_create(&pstAi->stAiPid, 0, HI3531D_COMM_AUDIO_AiProc, pstAi);
    
    return HI_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get frame from ai and send to aenc
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    SAMPLE_AI_S *pstAi = NULL;
    
    pstAi = &gs_stSampleAi[AiDev*AIO_MAX_CHN_NUM + AiChn];
    pstAi->bSendAenc = HI_TRUE;
    pstAi->bSendAo = HI_FALSE;
    pstAi->bStart= HI_TRUE;
    pstAi->AiDev = AiDev;
    pstAi->AiChn = AiChn;
    pstAi->AencChn = AeChn;
    pthread_create(&pstAi->stAiPid, 0, HI3531D_COMM_AUDIO_AiProc, pstAi);
    
    return HI_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from aenc and send to adec
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE *pAecFd)
{
    SAMPLE_AENC_S *pstAenc = NULL;

    if (NULL == pAecFd)
    {
//        return HI_FAILURE;
    }
    
    pstAenc = &gs_stSampleAenc[AeChn];
    pstAenc->AeChn = AeChn;
    pstAenc->AdChn = AdChn;
    pstAenc->bSendAdChn = HI_TRUE;
    pstAenc->pfd = pAecFd;    
    pstAenc->bStart = HI_TRUE;    
    pthread_create(&pstAenc->stAencPid, 0, HI3531D_COMM_AUDIO_AencProc, pstAenc);
    
    return HI_SUCCESS;
}

/******************************************************************************
* function : Create the thread to get stream from file and send to adec
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE *pAdcFd)
{
    SAMPLE_ADEC_S *pstAdec = NULL;

    if (NULL == pAdcFd)
    {
        return HI_FAILURE;
    }

    pstAdec = &gs_stSampleAdec[AdChn];
    pstAdec->AdChn = AdChn;
    pstAdec->pfd = pAdcFd;
    pstAdec->bStart = HI_TRUE;
    pthread_create(&pstAdec->stAdPid, 0, HI3531D_COMM_AUDIO_AdecProc, pstAdec);
    
    return HI_SUCCESS;
}


/******************************************************************************
* function : Create the thread to set Ao volume 
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_CreatTrdAoVolCtrl(AUDIO_DEV AoDev)
{
	SAMPLE_AO_S *pstAoCtl = NULL;
	
	pstAoCtl =  &gs_stSampleAo[AoDev];
	pstAoCtl->AoDev =  AoDev;
	pstAoCtl->bStart = HI_TRUE;
	pthread_create(&pstAoCtl->stAoPid, 0, HI3531D_COMM_AUDIO_AoVolProc, pstAoCtl);
	
	return HI_SUCCESS;
}


/******************************************************************************
* function : Destory the thread to get frame from ai and send to ao or aenc
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn)
{
    SAMPLE_AI_S *pstAi = NULL;
    
    pstAi = &gs_stSampleAi[AiDev*AIO_MAX_CHN_NUM + AiChn];

	if (pstAi->bStart)
	{
		pstAi->bStart= HI_FALSE;
	    //pthread_cancel(pstAi->stAiPid);
	    pthread_join(pstAi->stAiPid, 0);

	}
    
    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from aenc and send to adec
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn)
{
    SAMPLE_AENC_S *pstAenc = NULL;

    pstAenc = &gs_stSampleAenc[AeChn];

	if (pstAenc->bStart)
	{
		pstAenc->bStart = HI_FALSE;
    	//pthread_cancel(pstAenc->stAencPid);
    	pthread_join(pstAenc->stAencPid, 0);
	}
    
    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to get stream from file and send to adec
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn)
{
    SAMPLE_ADEC_S *pstAdec = NULL;

    pstAdec = &gs_stSampleAdec[AdChn];

	if (pstAdec->bStart)
	{
		pstAdec->bStart = HI_FALSE;
    	//pthread_cancel(pstAdec->stAdPid);
    	pthread_join(pstAdec->stAdPid, 0);
	}    
    
    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the thread to set Ao volume 
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_DestoryTrdAoVolCtrl(AUDIO_DEV AoDev)
{
    SAMPLE_AO_S *pstAoCtl = NULL;

    pstAoCtl =  &gs_stSampleAo[AoDev];

	if (pstAoCtl->bStart)
	{
		pstAoCtl->bStart = HI_FALSE;
    	//pthread_cancel(pstAoCtl->stAoPid);
    	pthread_join(pstAoCtl->stAoPid, 0);
	}    

    return HI_SUCCESS;
}

/******************************************************************************
* function : Destory the all thread
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_DestoryAllTrd()
{
    HI_U32 u32DevId, u32ChnId;

    for (u32DevId = 0; u32DevId < AI_DEV_MAX_NUM; u32DevId ++)
    {
        for (u32ChnId = 0; u32ChnId < AIO_MAX_CHN_NUM; u32ChnId ++)
        {
            HI3531D_COMM_AUDIO_DestoryTrdAi(u32DevId, u32ChnId);
        }      
    }
 
    for (u32ChnId = 0; u32ChnId < AENC_MAX_CHN_NUM; u32ChnId ++)
    {
        HI3531D_COMM_AUDIO_DestoryTrdAencAdec(u32ChnId);
    }    

    for (u32ChnId = 0; u32ChnId < ADEC_MAX_CHN_NUM; u32ChnId ++)
    {
        HI3531D_COMM_AUDIO_DestoryTrdFileAdec(u32ChnId);
    }  

    for (u32ChnId = 0; u32ChnId < AO_DEV_MAX_NUM; u32ChnId ++)
    {
        HI3531D_COMM_AUDIO_DestoryTrdAoVolCtrl(u32ChnId);
    }
    
    
    return HI_SUCCESS;
}



/******************************************************************************
* function : Ao bind Ai
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;
    
    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn); 
}

/******************************************************************************
* function : Ao unbind Ai
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32ChnId = AiChn;
    stSrcChn.s32DevId = AiDev;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;
    
    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn); 
}

/******************************************************************************
* function : Aenc bind Ai
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32DevId = AiDev;
    stSrcChn.s32ChnId = AiChn;
    stDestChn.enModId = HI_ID_AENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = AeChn;
    
    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Aenc unbind Ai
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn)
{
    MPP_CHN_S stSrcChn,stDestChn;

    stSrcChn.enModId = HI_ID_AI;
    stSrcChn.s32DevId = AiDev;
    stSrcChn.s32ChnId = AiChn;
    stDestChn.enModId = HI_ID_AENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = AeChn;
    
    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);      
}


/******************************************************************************
* function : Start Ai
******************************************************************************/
//extern int g_audio_AGC[CHANEL_NUM];

HI_S32 HI3531D_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enOutSampleRate, HI_BOOL bResampleEn, AI_VQE_CONFIG_S* pstAiVqeAttr,HI_U32 u32AiVqeType)
{

	HI_S32 i;
	HI_S32 s32Ret;

	if (pstAioAttr->u32ClkChnCnt == 0)
	{
		pstAioAttr->u32ClkChnCnt = pstAioAttr->u32ChnCnt;
	}

	s32Ret = HI_MPI_AI_SetPubAttr(AiDevId, pstAioAttr);
	printf("audio sample rate : %d\n",pstAioAttr->enSamplerate);
	if (s32Ret)
	{
		printf("%s: HI_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	s32Ret = HI_MPI_AI_Enable(AiDevId);
	if (s32Ret)
	{
		printf("%s: HI_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
		return s32Ret;
	}

	for (i = 0; i < s32AiChnCnt; i++)
	{
		s32Ret = HI_MPI_AI_EnableChn(AiDevId, i);
		if (s32Ret)
		{
			printf("%s: HI_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
			return s32Ret;
		}

		if (HI_TRUE == bResampleEn)
		{
			s32Ret = HI_MPI_AI_EnableReSmp(AiDevId, i, enOutSampleRate);
			if (s32Ret)
			{
				printf("%s: HI_MPI_AI_EnableReSmp(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
				return s32Ret;
			}
		}
		if (NULL != pstAiVqeAttr)
		{
			s32Ret= HI_MPI_AI_SetVqeAttr(AiDevId, i, SAMPLE_AUDIO_AO_DEV, i, (AI_VQE_CONFIG_S *)pstAiVqeAttr);
			if (s32Ret)
			{
				printf("%s: HI_MPI_AI_SetVqeAttr(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
				return s32Ret;
			}

			s32Ret = HI_MPI_AI_EnableVqe(AiDevId, i);
			if (s32Ret)
			{
				printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
				return s32Ret;
			}

		}


	}

return HI_SUCCESS;
}


/******************************************************************************
* function : Stop Ai
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt,
                                HI_BOOL bResampleEn, HI_BOOL bVqeEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i = 0; i < s32AiChnCnt; i++)
    {
        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AI_DisableReSmp(AiDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return s32Ret;
            }
        }

        if (HI_TRUE == bVqeEn)
        {
            s32Ret = HI_MPI_AI_DisableVqe(AiDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return s32Ret;
            }
        }

        s32Ret = HI_MPI_AI_DisableChn(AiDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
            return s32Ret;
        }
    }

    s32Ret = HI_MPI_AI_Disable(AiDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return s32Ret;
    }

    return HI_SUCCESS;
}

#ifdef HI_ACODEC_TYPE_HDMI
HI_S32 HI3531D_COMM_AUDIO_StartHdmi(AIO_ATTR_S *pstAioAttr)
{
    HI_S32 s32Ret;
    HI_HDMI_ATTR_S stHdmiAttr;
    HI_HDMI_ID_E enHdmi = HI_HDMI_ID_0;
    VO_PUB_ATTR_S stPubAttr;
    VO_DEV VoDev = 0;
    
    stPubAttr.u32BgColor = 0x000000ff;
    stPubAttr.enIntfType = VO_INTF_HDMI; 
    stPubAttr.enIntfSync = VO_OUTPUT_1080P30;

    if(HI_SUCCESS != SAMPLE_COMM_VO_StartDev(VoDev, &stPubAttr))
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    s32Ret = SAMPLE_COMM_VO_HdmiStart(stPubAttr.enIntfSync);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_HDMI_SetAVMute(enHdmi, HI_TRUE);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }
    
    s32Ret = HI_MPI_HDMI_GetAttr(enHdmi, &stHdmiAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }
    
    stHdmiAttr.bEnableAudio = HI_TRUE;        /**< if enable audio */
    stHdmiAttr.enSoundIntf = HI_HDMI_SND_INTERFACE_I2S; /**< source of HDMI audio, HI_HDMI_SND_INTERFACE_I2S suggested.the parameter must be consistent with the input of AO*/
    stHdmiAttr.enSampleRate = pstAioAttr->enSamplerate;        /**< sampling rate of PCM audio,the parameter must be consistent with the input of AO */
    stHdmiAttr.u8DownSampleParm = HI_FALSE;    /**< parameter of downsampling  rate of PCM audio, default :0 */
    
    stHdmiAttr.enBitDepth = 8 * (pstAioAttr->enBitwidth+1);   /**< bitwidth of audio,default :16,the parameter must be consistent with the config of AO */
    stHdmiAttr.u8I2SCtlVbit = 0;        /**< reserved, should be 0, I2S control (0x7A:0x1D) */
    
    stHdmiAttr.bEnableAviInfoFrame = HI_TRUE; /**< if enable  AVI InfoFrame*/
    stHdmiAttr.bEnableAudInfoFrame = HI_TRUE;; /**< if enable AUDIO InfoFrame*/

    s32Ret = HI_MPI_HDMI_SetAttr(enHdmi, &stHdmiAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }
    
    s32Ret = HI_MPI_HDMI_SetAVMute(enHdmi, HI_FALSE);
    if(HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

inline HI_S32 HI3531D_COMM_AUDIO_StopHdmi(HI_VOID)
{
    HI_S32 s32Ret;
    VO_DEV VoDev = 0;
    
    s32Ret =  HI3531D_COMM_VO_HdmiStop();
    s32Ret |= HI_MPI_VO_Disable(VoDev);
    if(HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_VO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }
    
    return s32Ret;
}
#endif


/******************************************************************************
* function : Start Aenc
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_StartAenc(HI_S32 s32AencChnCnt, HI_U32 u32AencPtNumPerFrm, PAYLOAD_TYPE_E enType)
{
    AENC_CHN AeChn;
    HI_S32 s32Ret, i;
    AENC_CHN_ATTR_S stAencAttr;
    AENC_ATTR_ADPCM_S stAdpcmAenc;
    AENC_ATTR_G711_S stAencG711;
    AENC_ATTR_G726_S stAencG726;
    AENC_ATTR_LPCM_S stAencLpcm;
    //AENC_ATTR_AAC_S stAencAac;
    
    /* set AENC chn attr */
    
    stAencAttr.enType = enType;
    stAencAttr.u32BufSize = 50;
    stAencAttr.u32PtNumPerFrm = u32AencPtNumPerFrm;
    
    if (PT_ADPCMA == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAdpcmAenc;
        stAdpcmAenc.enADPCMType = AUDIO_ADPCM_TYPE;
    }
    else if (PT_G711A == stAencAttr.enType || PT_G711U == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAencG711;
    }
    else if (PT_G726 == stAencAttr.enType)
    {
        stAencAttr.pValue       = &stAencG726;
        stAencG726.enG726bps    = G726_BPS;
    }
    else if (PT_LPCM == stAencAttr.enType)
    {
        stAencAttr.pValue = &stAencLpcm;
    }
    else
    {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAencAttr.enType);
        return HI_FAILURE;
    }    

    for (i=0; i<s32AencChnCnt; i++)
    {            
        AeChn = i;
        
        /* create aenc chn*/
        s32Ret = HI_MPI_AENC_CreateChn(AeChn, &stAencAttr);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,
                   AeChn, s32Ret);
            return s32Ret;
        }        
    }
    
    return HI_SUCCESS;
}


/******************************************************************************
* function : Stop Aenc
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_StopAenc(HI_S32 s32AencChnCnt)
{
	HI_S32 i;
	HI_S32 s32Ret;
	
	for (i=0; i<s32AencChnCnt; i++)
	{
	    s32Ret = HI_MPI_AENC_DestroyChn(i);
	    if (HI_SUCCESS != s32Ret)
	    {
	        printf("%s: HI_MPI_AENC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
	               i, s32Ret);
	        return s32Ret;
	    }
	    
	}
    
    return HI_SUCCESS;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif 
#endif /* End of #ifdef __cplusplus */

