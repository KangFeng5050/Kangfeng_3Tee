#include "comm.h"

#define SAMPLE_DBG(s32Ret)\
do{\
    printf("s32Ret=%#x,fuc:%s,line:%d\n", s32Ret, __FUNCTION__, __LINE__);\
}while(0)

#define AUDIO_ADPCM_TYPE ADPCM_TYPE_DVI4/* ADPCM_TYPE_IMA, ADPCM_TYPE_DVI4*/
#define G726_BPS MEDIA_G726_40K         /* MEDIA_G726_16K, MEDIA_G726_24K ... */

static PAYLOAD_TYPE_E gs_enPayloadType = PT_LPCM;
static AUDIO_SAMPLE_RATE_E aoSampleRate = AUDIO_SAMPLE_RATE_48000;
#define P2P_ADEC	10
#define P2P_AO		1

static HI_BOOL gs_bAoReSample = HI_FALSE;


HI_S32 HI3531D_COMM_AUDIO_CfgAcodec(AIO_ATTR_S* pstAioAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_BOOL bCodecCfg = HI_FALSE;

#ifdef HI_ACODEC_TYPE_INNER
    /*** INNER AUDIO CODEC ***/
    s32Ret = SAMPLE_INNER_CODEC_CfgAudio(pstAioAttr->enSamplerate);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s:SAMPLE_INNER_CODEC_CfgAudio failed\n", __FUNCTION__);
        return s32Ret;
    }
    bCodecCfg = HI_TRUE;
#endif

#ifdef HI_ACODEC_TYPE_TLV320AIC31
    /*** ACODEC_TYPE_TLV320 ***/
    s32Ret = SAMPLE_Tlv320_CfgAudio(pstAioAttr->enWorkmode, pstAioAttr->enSamplerate);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: SAMPLE_Tlv320_CfgAudio failed\n", __FUNCTION__);
        return s32Ret;
    }
    bCodecCfg = HI_TRUE;
#endif
#if 0
    if (!bCodecCfg)
    {
        printf("Can not find the right codec.\n");
        return HI_FALSE;
    }
#endif
    return HI_SUCCESS;
}


HI_S32 HI3531D_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = AdChn;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : Ao unbind Adec
******************************************************************************/
HI_S32 HI3531D_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn)
{
    MPP_CHN_S stSrcChn, stDestChn;

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32ChnId = AdChn;
    stSrcChn.s32DevId = 0;
    stDestChn.enModId = HI_ID_AO;
    stDestChn.s32DevId = AoDev;
    stDestChn.s32ChnId = AoChn;

    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}


HI_S32 HI3531D_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType)
{
    HI_S32 s32Ret;
    ADEC_CHN_ATTR_S stAdecAttr;
    ADEC_ATTR_ADPCM_S stAdpcm;
    ADEC_ATTR_G711_S stAdecG711;
    ADEC_ATTR_G726_S stAdecG726;
    ADEC_ATTR_LPCM_S stAdecLpcm;

    stAdecAttr.enType = enType;
    stAdecAttr.u32BufSize = 20;
    stAdecAttr.enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */

    if (PT_ADPCMA == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdpcm;
        stAdpcm.enADPCMType = AUDIO_ADPCM_TYPE ;
    }
    else if (PT_G711A == stAdecAttr.enType || PT_G711U == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdecG711;
    }
#if 0
    else if (PT_G726 == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdecG726;
        stAdecG726.enG726bps = G726_BPS ;
    }
#endif
    else if (PT_LPCM == stAdecAttr.enType)
    {
        stAdecAttr.pValue = &stAdecLpcm;
        stAdecAttr.enMode = ADEC_MODE_PACK;/* lpcm must use pack mode */
    }
    else
    {
        printf("%s: invalid aenc payload type:%d\n", __FUNCTION__, stAdecAttr.enType);
        return HI_FAILURE;
    }

    /* create adec chn*/
    s32Ret = HI_MPI_ADEC_CreateChn(AdChn, &stAdecAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_ADEC_CreateChn(%d) failed with %#x!\n", __FUNCTION__, \
               AdChn, s32Ret);
        return s32Ret;
    }
    return 0;
}

HI_S32 HI3531D_COMM_AUDIO_StopAdec(ADEC_CHN AdChn)
{
    HI_S32 s32Ret;

    s32Ret = HI_MPI_ADEC_DestroyChn(AdChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_ADEC_DestroyChn(%d) failed with %#x!\n", __FUNCTION__,
               AdChn, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}



HI_S32 HI3531D_AUDIO_SendAo(char *buf, int size)
{
        ADEC_CHN AdChn=P2P_ADEC;
        HI_S32 S32Ret;
        AUDIO_STREAM_S stAudioStream;
        stAudioStream.pStream=(unsigned char*)buf;
        stAudioStream.u32Len=size;
        S32Ret=HI_MPI_ADEC_SendStream(AdChn,&stAudioStream,HI_TRUE);
        if(S32Ret != HI_SUCCESS)
        {
                printf("HI_MPI_ADEC_SendStream error ret=0x%x\n",S32Ret);
                return HI_FAILURE;
        }

        return HI_SUCCESS;
}

HI_S32 HI3531D_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt,
                                 AIO_ATTR_S* pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, HI_BOOL bResampleEn, HI_VOID* pstAoVqeAttr, HI_U32 u32AoVqeType)
{
    HI_S32 i;
    HI_S32 s32Ret;

    s32Ret = HI_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
               AoDevId, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_AO_Enable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
        return HI_FAILURE;
    }

    for (i = 0; i < s32AoChnCnt; i++)
    {
        s32Ret = HI_MPI_AO_EnableChn(AoDevId, i/(pstAioAttr->enSoundmode + 1));
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
            return HI_FAILURE;
        }

        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
            s32Ret |= HI_MPI_AO_EnableReSmp(AoDevId, i, enInSampleRate);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_EnableReSmp(%d,%d) failed with %#x!\n", __FUNCTION__, AoDevId, i, s32Ret);
                return HI_FAILURE;
            }
        }

                if (NULL != pstAoVqeAttr)
        {
                        HI_BOOL bAoVqe = HI_TRUE;
                        switch (u32AoVqeType)
            {
                                case 0:
                    s32Ret = HI_SUCCESS;
                                        bAoVqe = HI_FALSE;
                    break;
                case 1:
                    s32Ret = HI_MPI_AO_SetVqeAttr(AoDevId, i, (AO_VQE_CONFIG_S *)pstAoVqeAttr);
                    break;
                default:
                    s32Ret = HI_FAILURE;
                    break;
            }

            if (s32Ret)
            {
                printf("%s: SetAoVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, u32AoVqeType, AoDevId, i, s32Ret);
                return s32Ret;
            }

                    if (bAoVqe)
            {
                	s32Ret = HI_MPI_AO_EnableVqe(AoDevId, i);
                    if (s32Ret)
                    {
                        printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AoDevId, i, s32Ret);
                        return s32Ret;
                    }
            }
        }
    }

    return HI_SUCCESS;
}
HI_S32 HI3531D_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bResampleEn, HI_BOOL bVqeEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i = 0; i < s32AoChnCnt; i++)
    {
        if (HI_TRUE == bResampleEn)
        {
            s32Ret = HI_MPI_AO_DisableReSmp(AoDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("%s: HI_MPI_AO_DisableReSmp failed with %#x!\n", __FUNCTION__, s32Ret);
                return s32Ret;
            }
        }

                if (HI_TRUE == bVqeEn)
        {
            s32Ret = HI_MPI_AO_DisableVqe(AoDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return s32Ret;
            }
        }

        s32Ret = HI_MPI_AO_DisableChn(AoDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }

    s32Ret = HI_MPI_AO_Disable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}


HI_S32 HI3531D_AUDIO_AO_START()
{
        AIO_ATTR_S stAoAttr;
        AUDIO_DEV AoDev = P2P_AO;
        AO_CHN      AoChn = 0;
        ADEC_CHN    AdChn = P2P_ADEC;
        HI_S32 s32Ret;
        stAoAttr.enSamplerate = AUDIO_SAMPLE_RATE_48000;
        stAoAttr.enWorkmode = AIO_MODE_I2S_MASTER;
        stAoAttr.enBitwidth  = AUDIO_BIT_WIDTH_16;
        stAoAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;//AUDIO_SOUND_MODE_STEREO;
        stAoAttr.u32EXFlag = 1;
        stAoAttr.u32FrmNum = 50;
        stAoAttr.u32ChnCnt = 1;
	stAoAttr.u32ClkChnCnt   = 1;
        stAoAttr.u32ClkSel = 0;
        stAoAttr.u32PtNumPerFrm =1280;//1280;
        s32Ret = HI3531D_COMM_AUDIO_CfgAcodec(&stAoAttr);
        if(s32Ret != HI_SUCCESS)
        {
                SAMPLE_DBG(s32Ret);
                        return HI_FAILURE;
        }
        s32Ret = HI3531D_COMM_AUDIO_StartAdec(AdChn, gs_enPayloadType);
        if (s32Ret != HI_SUCCESS)
        {
                SAMPLE_DBG(s32Ret);
                return HI_FAILURE;
        }
        HI_S32 s32AoChnCnt = stAoAttr.u32ChnCnt;
        s32Ret = HI3531D_COMM_AUDIO_StartAo(AoDev, s32AoChnCnt, &stAoAttr,aoSampleRate,gs_bAoReSample,NULL,0);
        if(s32Ret != HI_SUCCESS)
        {
                SAMPLE_DBG(s32Ret);
                        return HI_FAILURE;
        }
        AUDIO_TRACK_MODE_E enTrackMode = AUDIO_TRACK_BOTH_LEFT;
        s32Ret = HI_MPI_AO_SetTrackMode(AoDev, enTrackMode);
        if (HI_SUCCESS != s32Ret)
        {
                SAMPLE_DBG(s32Ret);
                return s32Ret;
        }
         s32Ret = HI3531D_COMM_AUDIO_AoBindAdec(AoDev, AoChn, AdChn);
        if (s32Ret != HI_SUCCESS)
        {
                        SAMPLE_DBG(s32Ret);
                return HI_FAILURE;
        }
	printf("-----end----------HI3531D_AUDIO_AO_START--------------\n");
        return HI_TRUE;
}

void HI3531D_AUDIO_AO_STOP()
{
        AUDIO_DEV AoDev = P2P_AO;
        HI_S32 s32AoChnCnt = 1;
        ADEC_CHN    AdChn = P2P_ADEC;
        AO_CHN      AoChn = 0;
        HI3531D_COMM_AUDIO_AoUnbindAdec(AoDev, AoChn, AdChn);
        HI3531D_COMM_AUDIO_StopAo(AoDev,s32AoChnCnt,gs_bAoReSample,HI_FALSE);
        HI3531D_COMM_AUDIO_StopAdec(AdChn);
}


