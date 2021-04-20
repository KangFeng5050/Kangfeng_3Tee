#include "Hi3531DMedia.h"

unsigned int netlink_group_mask(unsigned int group)
{
        return group ? 1 << (group - 1) : 0;
}

unsigned int trans_fps( unsigned int s )
{
        if( s > 65 ) return 75;
        if( s > 55 ) return 60;
        if( s > 35 ) return 50;
        if( s > 25 ) return 30;
        if( s == 25) return s;
        if( s > 20 ) return 24;
}

void HI3531D_Init(void)
{
	VIDEO_NORM_E gs_enNorm = VIDEO_ENCODING_MODE_PAL;
	PIC_SIZE_E enSize[2] = {PIC_UHD4K, PIC_HD1080};
	VB_CONF_S stVbConf = {0};
	HI_U32 u32BlkSize;
	HI_U32 s32Ret;
	memset(&stVbConf,0,sizeof(VB_CONF_S));	
    	stVbConf.u32MaxPoolCnt = VB_MAX_POOLS;

    	/*video buffer*/   
    	u32BlkSize = HI3531D_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
                enSize[0], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH,COMPRESS_MODE_SEG);
    		stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    		stVbConf.astCommPool[0].u32BlkCnt = 60;

#if 0
   u32BlkSize = HI3531D_COMM_SYS_CalcPicVbBlkSize(gs_enNorm,\
               enSize[1], SAMPLE_PIXEL_FORMAT, SAMPLE_SYS_ALIGN_WIDTH,COMPRESS_MODE_NONE);
	  stVbConf.astCommPool[1].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[1].u32BlkCnt = 50;
#endif
	/******************************************
     step 2: mpp system init. 
    ******************************************/
	s32Ret = HI3531D_COMM_SYS_Init(&stVbConf);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("system init failed with %d!\n", s32Ret);
		HI3531D_COMM_SYS_Exit();
	}
	//HI3531A_COMM_SYS_MemConfig();
}

void HI3531D_Exit(void)
{
	HI3531D_COMM_SYS_Exit();
}


HI_S32 start_AIO(HI_VOID)
{
	HI3531D_AUDIO_Ai((AUDIO_SAMPLE_RATE_E)48000, PT_LPCM);//for hdmi/line in for i2s1
	HI3531D_AUDIO_Aenc(PT_LPCM, (AUDIO_SAMPLE_RATE_E)48000);//g_a1_bitrate);
	HI3531D_AUDIO_AO_START();
	return HI_SUCCESS;

}

HI_S32 stop_AIO(HI_VOID)
{
	HI3531D_AUDIO_Aenc_Stop();
	HI3531D_AUDIO_Ai_Stop(1);
	HI3531D_AUDIO_AO_STOP();
	return HI_SUCCESS;

}
HI_S32 start_VO_H264()
{
    VB_CONF_S stVbConf, stModVbConf;
    HI_S32 i, s32Ret = HI_SUCCESS;
    VDEC_CHN_ATTR_S stVdecChnAttr[VDEC_MAX_CHN_NUM];
    VdecThreadParam stVdecSend[VDEC_MAX_CHN_NUM];
    VPSS_GRP_ATTR_S stVpssGrpAttr[VDEC_MAX_CHN_NUM];
    SIZE_S stSize, stRotateSize;
    VO_DEV VoDev;
    VO_LAYER VoLayer;
    VO_PUB_ATTR_S stVoPubAttr;
    VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
    HI_U32 u32VdCnt = 1, u32GrpCnt = 2;
    pthread_t   VdecThread[2*VDEC_MAX_CHN_NUM];

    stSize.u32Width = HD_WIDTH;
    stSize.u32Height = HD_HEIGHT;
	
    /************************************************
    step1:  init mod common VB
    *************************************************/
    HI3531D_COMM_VDEC_ModCommPoolConf(&stModVbConf, PT_H264, &stSize, u32VdCnt, HI_FALSE);	
    s32Ret = HI3531D_COMM_VDEC_InitModCommVb(&stModVbConf);
    if(s32Ret != HI_SUCCESS)
    {	    	
        SAMPLE_PRT("init mod common vb fail for %#x!\n", s32Ret);
        
    }

    /************************************************
    step2:  start VDEC
    *************************************************/
    HI3531D_COMM_VDEC_ChnAttr(u32VdCnt, &stVdecChnAttr[0], PT_H264, &stSize);
    s32Ret = HI3531D_COMM_VDEC_Start(u32VdCnt, &stVdecChnAttr[0]);
    if(s32Ret != HI_SUCCESS)
    {	
        SAMPLE_PRT("start VDEC fail for %#x!\n", s32Ret);
        
    }

    /************************************************
    step3:  start VPSS
    *************************************************/
    stRotateSize.u32Width = stRotateSize.u32Height = MAX2(stSize.u32Width, stSize.u32Height);
    HI3531D_COMM_VDEC_VpssGrpAttr(u32GrpCnt, &stVpssGrpAttr[0], &stRotateSize);
    s32Ret = HI3531D_COMM_VPSS_StartEx(u32GrpCnt, &stRotateSize, 1, &stVpssGrpAttr[0]);
    if(s32Ret != HI_SUCCESS)
    {	    
        SAMPLE_PRT("start VPSS fail for %#x!\n", s32Ret);
        
    }
    /************************************************
    step4:  start VO
    *************************************************/	
    VoDev = SAMPLE_VO_DEV_DHD0;
    VoLayer = SAMPLE_VO_DEV_DHD0;
    
#if HI_FPGA
    stVoPubAttr.enIntfSync = VO_OUTPUT_1080P30;
    stVoPubAttr.enIntfType = VO_INTF_VGA;
#else
    stVoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    stVoPubAttr.enIntfType = VO_INTF_VGA | VO_INTF_HDMI;
#endif
    s32Ret = HI3531D_COMM_VO_StartDev(VoDev, &stVoPubAttr);
    if(s32Ret != HI_SUCCESS)
    {		
        SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
        
    }

#ifndef HI_FPGA
    if (HI_SUCCESS != HI3531D_COMM_VO_HdmiStart(stVoPubAttr.enIntfSync))
    {
        SAMPLE_PRT("Start SAMPLE_COMM_VO_HdmiStart failed!\n");
        
    }
#endif

    stVoLayerAttr.bClusterMode = HI_FALSE;
    stVoLayerAttr.bDoubleFrame = HI_FALSE;
    stVoLayerAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;    

    s32Ret = HI3531D_COMM_VO_GetWH(stVoPubAttr.enIntfSync, \
        &stVoLayerAttr.stDispRect.u32Width, &stVoLayerAttr.stDispRect.u32Height, &stVoLayerAttr.u32DispFrmRt);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        
    }
    stVoLayerAttr.stImageSize.u32Width = stVoLayerAttr.stDispRect.u32Width;
    stVoLayerAttr.stImageSize.u32Height = stVoLayerAttr.stDispRect.u32Height;
    s32Ret = HI3531D_COMM_VO_StartLayer(VoLayer, &stVoLayerAttr,HI_FALSE);
    if(s32Ret != HI_SUCCESS)
    {		
        SAMPLE_PRT("SAMPLE_COMM_VO_StartLayer fail for %#x!\n", s32Ret);
        
    }	

    s32Ret = HI3531D_COMM_VO_StartChn(VoLayer, VO_MODE_1MUX);
    if(s32Ret != HI_SUCCESS)
    {		
        SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
        
    }

    /************************************************
    step5:  VDEC bind VPSS
    *************************************************/	
    for(i=0; i<u32VdCnt; i++)
    {
    	s32Ret = HI3531D_COMM_VDEC_BindVpss(0, 2);
    	if(s32Ret != HI_SUCCESS)
    	{	    
            SAMPLE_PRT("vdec bind vpss fail for %#x!\n", s32Ret);
            
    	}	
    }
    	
    /************************************************
    step6:  VPSS bind VO
    *************************************************/
    for(i=0; i<u32VdCnt; i++)
    {
        s32Ret = HI3531D_COMM_VO_BindVpss(VoLayer, 0, 2, VPSS_CHN0);
        if(s32Ret != HI_SUCCESS)
        {	    
            SAMPLE_PRT("vpss bind vo fail for %#x!\n", s32Ret);
           
        }	
    }	
    return s32Ret;
}
