/******************************************************************************
  Some simple Hisilicon Hi35xx video input functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-8 Created
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#include "comm.h"


VI_DEV_ATTR_S DEV_ATTR_BT656D1_4MUX =
{
    /*interface mode*/
    VI_MODE_BT656,
    /*work mode, 1/2/4 multiplex*/
    VI_WORK_MODE_4Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* for single/double edge, must be set when double edge*/
	VI_CLK_EDGE_SINGLE_UP,
	
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, just support yuv*/
    VI_INPUT_DATA_YVYU,
    /*sync info*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing info*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*whether use isp*/
    VI_PATH_BYPASS,
    /*data type*/
    VI_DATA_TYPE_YUV
};

VI_DEV_ATTR_S DEV_ATTR_TP2823_720P_2MUX_BASE =
{
    /*interface mode*/
    VI_MODE_BT656,
    /*work mode, 1/2/4 multiplex*/
    VI_WORK_MODE_2Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* for single/double edge, must be set when double edge*/
	VI_CLK_EDGE_SINGLE_UP,
	
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, just support yuv*/
    VI_INPUT_DATA_YVYU,
    /*sync info*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing info*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*whether use isp*/
    VI_PATH_BYPASS,
    /*data type*/
    VI_DATA_TYPE_YUV
};


VI_DEV_ATTR_S DEV_ATTR_TP2823_720P_1MUX_BASE =
{
    /*interface mode*/
    VI_MODE_BT656,
    //VI_MODE_BT1120_INTERLEAVED,
    //VI_MODE_BT1120_STANDARD,
    /*work mode, 1/2/4 multiplex*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},

	/* for single/double edge, must be set when double edge*/
	VI_CLK_EDGE_DOUBLE,
	//VI_CLK_EDGE_SINGLE_UP,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, just support yuv*/
    VI_INPUT_DATA_UYVY,
   //VI_INPUT_DATA_VYUY,
    /*sync info*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*timing info*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*whether use isp*/
    VI_PATH_BYPASS,
    /*data type*/
    VI_DATA_TYPE_YUV
};

VI_DEV_ATTR_S DEV_ATTR_7441_BT1120_STANDARD_BASE =
{
    /*interface mode*/
    VI_MODE_BT1120_STANDARD,
    /*work mode, 1/2/4 multiplex*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0xFF0000},

	/* for single/double edge, must be set when double edge*/
	VI_CLK_EDGE_SINGLE_UP,
	//VI_CLK_EDGE_DOUBLE,
	
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, just support yuv*/
    VI_INPUT_DATA_UVUV,

    /*sync info*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,

    /*timing info*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            3840,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            2160,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*whether use isp*/
    VI_PATH_BYPASS,
    /*data type*/
    VI_DATA_TYPE_YUV

};

VI_DEV g_as32ViDev[VIU_MAX_DEV_NUM];
VI_CHN g_as32MaxChn[VIU_MAX_CHN_NUM];
VI_CHN g_as32SubChn[VIU_MAX_CHN_NUM];
/*****************************************************************************
* function : set vi mask.
*****************************************************************************/
HI_VOID HI3531D_COMM_VI_SetMask(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr)
{

    switch (ViDev % 2)
      {
          case 0: 
            pstDevAttr->au32CompMask[0] = 0xFF000000;
            if (VI_MODE_BT1120_STANDARD == pstDevAttr->enIntfMode)
            {
                pstDevAttr->au32CompMask[0] = 0xFF000000;
                pstDevAttr->au32CompMask[1] = 0xFF000000>>8;
            }
            else 
            {
                pstDevAttr->au32CompMask[1] = 0x0;
            }
            break;
            
          case 1:
             pstDevAttr->au32CompMask[0] = 0xFF000000>>8;
             pstDevAttr->au32CompMask[1] = 0x0;              
              break;
          default:
              HI_ASSERT(0);
      }

}

HI_S32 HI3531D_COMM_VI_Mode2Param(SAMPLE_VI_MODE_E enViMode, SAMPLE_VI_PARAM_S *pstViParam)
{
    switch (enViMode)
    {
        case SAMPLE_VI_MODE_32_D1:
        case SAMPLE_VI_MODE_32_960H:
        case SAMPLE_VI_MODE_32_1280H:
        case SAMPLE_VI_MODE_32_HALF720P:
            pstViParam->s32ViDevCnt = 4;
            pstViParam->s32ViDevInterval = 1;
            pstViParam->s32ViChnCnt = 16;
            pstViParam->s32ViChnInterval = 1;
            break;
        case SAMPLE_VI_MODE_16_720P:
        case SAMPLE_VI_MODE_16_1080P:
            /* use chn 0,2,4,6,8,10,12,14,16,18,20,22,24,28 */
            pstViParam->s32ViDevCnt = 8;
            pstViParam->s32ViDevInterval = 1;
            pstViParam->s32ViChnCnt = 16;
            pstViParam->s32ViChnInterval = 2;
            break;
        case SAMPLE_VI_MODE_8_720P:
        case SAMPLE_VI_MODE_8_1080P:        
            /* use chn 0,4,8,12,16,20,24,28 */
            pstViParam->s32ViDevCnt = 8;
            pstViParam->s32ViDevInterval = 1;
            pstViParam->s32ViChnCnt = 8;
            pstViParam->s32ViChnInterval = 4;
            break;
        default:
            SAMPLE_PRT("ViMode invaild!\n");
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* function : get vi parameter, according to vi type
*****************************************************************************/


/*****************************************************************************
* function : get vi parameter, according to vi type
*****************************************************************************/
HI_S32 HI3531D_COMM_VI_Mode2Size(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm, RECT_S *pstCapRect, SIZE_S *pstDestSize)
{
    pstCapRect->s32X = 0;
    pstCapRect->s32Y = 0;
    switch (enViMode)
    {
        case SAMPLE_VI_MODE_32_D1:
            pstDestSize->u32Width = D1_WIDTH;
            pstDestSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            pstCapRect->u32Width = D1_WIDTH;
            pstCapRect->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;
        case SAMPLE_VI_MODE_32_960H:
            pstDestSize->u32Width = 960;
            pstDestSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            pstCapRect->u32Width = 960;
            pstCapRect->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;
        case SAMPLE_VI_MODE_8_720P:
        case SAMPLE_VI_MODE_16_720P:
            pstDestSize->u32Width  = _720P_WIDTH;
            pstDestSize->u32Height = _720P_HEIGHT;
            pstCapRect->u32Width  = _720P_WIDTH;
            pstCapRect->u32Height = _720P_HEIGHT;
            break;

        case SAMPLE_VI_MODE_8_1080P:
        case SAMPLE_VI_MODE_16_1080P:
            pstDestSize->u32Width  = HD_WIDTH;
            pstDestSize->u32Height = HD_HEIGHT;
            pstCapRect->u32Width  = HD_WIDTH;
            pstCapRect->u32Height = HD_HEIGHT;
            break;           
        case SAMPLE_VI_MODE_4_4Kx2K:
            pstDestSize->u32Width  = _4K_WIDTH;
            pstDestSize->u32Height = _4K_HEIGHT;
            pstCapRect->u32Width  = _4K_WIDTH;
            pstCapRect->u32Height = _4K_HEIGHT;
            break;

        default:
            SAMPLE_PRT("vi mode invaild!\n");
            return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

/*****************************************************************************
* function : Get Vi Dev No. according to Vi_Chn No.
*****************************************************************************/
VI_DEV HI3531D_COMM_VI_GetDev(SAMPLE_VI_MODE_E enViMode, VI_CHN ViChn)
{
    HI_S32 s32Ret, s32ChnPerDev;
    SAMPLE_VI_PARAM_S stViParam;

    s32Ret = HI3531D_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("vi get param failed!\n");
        return (VI_DEV)-1;
    }

    s32ChnPerDev = stViParam.s32ViChnCnt / stViParam.s32ViDevCnt;
    return (VI_DEV)(ViChn /stViParam.s32ViChnInterval / s32ChnPerDev * stViParam.s32ViDevInterval);
}

/*****************************************************************************
* function : star vi dev (cfg vi_dev_attr; set_dev_cfg; enable dev)
*****************************************************************************/
HI_S32 HI3531D_COMM_VI_StartDev(VI_DEV ViDev, SAMPLE_VI_MODE_E enViMode)
{
	HI_S32 s32Ret;
	VI_DEV_ATTR_S stViDevAttr;
	memset(&stViDevAttr,0,sizeof(stViDevAttr));

	switch (enViMode)
	{
		case SAMPLE_VI_MODE_32_D1:
			memcpy(&stViDevAttr,&DEV_ATTR_BT656D1_4MUX,sizeof(stViDevAttr));
			HI3531D_COMM_VI_SetMask(ViDev,&stViDevAttr);
		break;
		case SAMPLE_VI_MODE_32_960H:
			memcpy(&stViDevAttr,&DEV_ATTR_BT656D1_4MUX,sizeof(stViDevAttr));
			HI3531D_COMM_VI_SetMask(ViDev,&stViDevAttr);
		break;
		case SAMPLE_VI_MODE_8_720P:
		case SAMPLE_VI_MODE_8_1080P:
			memcpy(&stViDevAttr,&DEV_ATTR_TP2823_720P_1MUX_BASE,sizeof(stViDevAttr));
			HI3531D_COMM_VI_SetMask(ViDev,&stViDevAttr);
		break;
		case SAMPLE_VI_MODE_16_720P:      
			memcpy(&stViDevAttr,&DEV_ATTR_TP2823_720P_2MUX_BASE,sizeof(stViDevAttr));
			HI3531D_COMM_VI_SetMask(ViDev,&stViDevAttr);
		break;
		case SAMPLE_VI_MODE_16_1080P:
		case SAMPLE_VI_MODE_4_1080P:
		case SAMPLE_VI_MODE_4_4Kx2K:
			memcpy(&stViDevAttr,&DEV_ATTR_7441_BT1120_STANDARD_BASE,sizeof(stViDevAttr));
			HI3531D_COMM_VI_SetMask(ViDev,&stViDevAttr);
		break;    
		default:
			SAMPLE_PRT("vi input type[%d] is invalid!\n", enViMode);
			return HI_FAILURE;
		}

	s32Ret = HI_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);
	if (s32Ret != HI_SUCCESS)
	{
	SAMPLE_PRT("HI_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
	return HI_FAILURE;
	}

	s32Ret = HI_MPI_VI_EnableDev(ViDev);
	if (s32Ret != HI_SUCCESS)
	{
	SAMPLE_PRT("HI_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
	return HI_FAILURE;
	}

	return HI_SUCCESS;
}

/*****************************************************************************
* function : star vi chn
*****************************************************************************/
HI_S32 HI3531D_COMM_VI_StartChn(VI_CHN ViChn,SAMPLE_VI_CHN_SET_E enViChnSet,vi_venc_par& vp)
{
	HI_S32 s32Ret;
	VI_CHN_ATTR_S stChnAttr;

	/* step  5: config & start vicap dev */
	stChnAttr.stCapRect.s32X = vp.vi_cap_x;
	stChnAttr.stCapRect.s32Y = vp.vi_cap_y;
	stChnAttr.stCapRect.u32Width = vp.vi_cap_width;
	stChnAttr.stCapRect.u32Height = vp.vi_cap_height;
	/* to show scale. this is a sample only, we want to show dist_size = D1 only */
	stChnAttr.stDestSize.u32Width = vp.vi_cap_width;//vp.venc_width;
	stChnAttr.stDestSize.u32Height = vp.vi_cap_height;//vp.venc_height;
	stChnAttr.enCapSel = VI_CAPSEL_BOTH;
	stChnAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;   /* sp420 or sp422 */
	stChnAttr.bMirror = (VI_CHN_SET_MIRROR == enViChnSet)?HI_TRUE:HI_FALSE;
	stChnAttr.bFlip = (VI_CHN_SET_FILP == enViChnSet)?HI_TRUE:HI_FALSE;
	stChnAttr.s32SrcFrameRate = -1;
	stChnAttr.s32DstFrameRate = -1;
//	if(vp.vi_interlaced == 1)
//		stChnAttr.enScanMode = VI_SCAN_INTERLACED;
//	else
		stChnAttr.enScanMode = VI_SCAN_PROGRESSIVE;

	s32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	s32Ret = HI_MPI_VI_EnableChn(ViChn);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	return HI_SUCCESS;
	}

/*****************************************************************************
* function : star vi according to product type
*            if vi input is hd, we will start sub-chn for cvbs preview
*****************************************************************************/
HI_S32 HI3531D_COMM_VI_Start(VI_DEV ViDev,VI_CHN ViChn,SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E gs_enNorm,vi_venc_par& vp)
{
	HI_S32 i;
	HI_S32 s32Ret;
	SAMPLE_VI_PARAM_S stViParam;
	SIZE_S stTargetSize;
	RECT_S stCapRect;

#if 0  
	/*** get parameter from Sample_Vi_Mode ***/
	s32Ret = HI3531D_COMM_VI_Mode2Param(enViMode, &stViParam);
	if (HI_SUCCESS !=s32Ret)
	{
	    SAMPLE_PRT("vi get param failed!\n");
	    return HI_FAILURE;
	}
	s32Ret = HI3531D_COMM_VI_Mode2Size(enViMode, enNorm, &stCapRect, &stTargetSize);
	if (HI_SUCCESS !=s32Ret)
	{
	    SAMPLE_PRT("vi get size failed!\n");
	    return HI_FAILURE;
	}
	/*** Start AD ***/
	s32Ret = HI3531D_COMM_VI_ADStart(enViMode, enNorm);
	if (HI_SUCCESS !=s32Ret)
	{
	    SAMPLE_PRT("Start AD failed!\n");
	    return HI_FAILURE;
	}
#endif   
	/*** Start VI Dev ***/
	//printf("---------ViDev=%d,ViChn=%d\n",ViDev,ViChn);
	s32Ret = HI3531D_COMM_VI_StartDev(ViDev, enViMode);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI3531D_COMM_VI_StartDev failed with %#x\n", s32Ret);
		return HI_FAILURE;
	}

	/*** Start VI Chn ***/
	s32Ret = HI3531D_COMM_VI_StartChn(ViChn, VI_CHN_SET_NORMAL,vp);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("call HI3531D_COMM_VI_StarChn failed with %#x\n", s32Ret);
		return HI_FAILURE;
	}
	return HI_SUCCESS;
	}
/*****************************************************************************
* function : stop vi accroding to product type
*****************************************************************************/
HI_S32 HI3531D_COMM_VI_Stop(VI_DEV ViDev)
{
    VI_CHN ViChn;
    HI_S32 i;
    HI_S32 s32Ret;
//printf("---HI3531D_COMM_VI_Stop---ViDev=%d\n",ViDev);
    /*** Stop VI Chn ***/
	for(i = 0; i < 4;i++)
	{
		ViChn = 4*ViDev + i;
		s32Ret = HI_MPI_VI_DisableChn(ViChn);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("HI3531A_COMM_VI_StopChn failed with %#x\n",s32Ret);
			//return HI_FAILURE;
		}
	}
    /*** Stop VI Dev ***/
	s32Ret = HI_MPI_VI_DisableDev(ViDev);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI3531A_COMM_VI_StopDev ViDev=%d--failed with %#x\n", ViDev,s32Ret);
		return HI_FAILURE;
	}

    return HI_SUCCESS;
}

/*****************************************************************************
* function : Vi chn bind vpss group
*****************************************************************************/
HI_S32 HI3531D_COMM_VI_BindVpss(VI_DEV ViDev, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
	HI_S32  s32Ret;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;
      
	stSrcChn.enModId = HI_ID_VIU;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = ViChn;

	stDestChn.enModId = HI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
        
	return HI_SUCCESS;
}


/*****************************************************************************
* function : Vi chn unbind vpss group
*****************************************************************************/
HI_S32 HI3531D_COMM_VI_UnBindVpss(VI_DEV ViDev, VI_CHN ViChn, VPSS_GRP VpssGrp)
{
	HI_S32 s32Ret;
	MPP_CHN_S stSrcChn;
	MPP_CHN_S stDestChn;

            
	stSrcChn.enModId = HI_ID_VIU;
	stSrcChn.s32DevId = ViDev;
	stSrcChn.s32ChnId = ViChn;

	stDestChn.enModId = HI_ID_VPSS;
	stDestChn.s32DevId = VpssGrp;
	stDestChn.s32ChnId = 0;

	s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
            
	return HI_SUCCESS;
}

HI_S32 HI3531D_COMM_VI_BindVo(VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VIU;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = ViChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
}

HI_S32 HI3531D_COMM_VI_UnBindVo(VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId = HI_ID_VIU;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = ViChn;

    stDestChn.enModId = HI_ID_VOU;
    stDestChn.s32DevId = VoLayer;
    stDestChn.s32ChnId = VoChn;

    return HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
}

/******************************************************************************
* function : read frame
******************************************************************************/
HI_VOID HI3531D_COMM_VI_ReadFrame(FILE * fp, HI_U8 * pY, HI_U8 * pU, HI_U8 * pV, HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2)
{
    HI_U8 * pDst;

    HI_U32 u32Row;

    pDst = pY;
    for ( u32Row = 0; u32Row < height; u32Row++ )
    {
        fread( pDst, width, 1, fp );
        pDst += stride;
    }

    pDst = pU;
    for ( u32Row = 0; u32Row < height/2; u32Row++ )
    {
        fread( pDst, width/2, 1, fp );
        pDst += stride2;
    }

    pDst = pV;
    for ( u32Row = 0; u32Row < height/2; u32Row++ )
    {
        fread( pDst, width/2, 1, fp );
        pDst += stride2;
    }
}
 
/******************************************************************************
* function : Plan to Semi
******************************************************************************/
HI_S32 HI3531D_COMM_VI_PlanToSemi(HI_U8 *pY, HI_S32 yStride,
                       HI_U8 *pU, HI_S32 uStride,
                       HI_U8 *pV, HI_S32 vStride,
                       HI_S32 picWidth, HI_S32 picHeight)
{
    HI_S32 i;
    HI_U8* pTmpU, *ptu;
    HI_U8* pTmpV, *ptv;
    HI_S32 s32HafW = uStride >>1 ;
    HI_S32 s32HafH = picHeight >>1 ;
    HI_S32 s32Size = s32HafW*s32HafH;

    if(!s32Size)
    {
        return HI_FAILURE;
    }
    pTmpU = (HI_U8*)malloc( s32Size ); ptu = pTmpU;
    pTmpV = (HI_U8*)malloc( s32Size ); ptv = pTmpV;

    if(NULL == pTmpU || NULL == pTmpV)
    {
        free( ptu );
        free( ptv );
        return HI_FAILURE;
    }
    
    memcpy(pTmpU,pU,s32Size);
    memcpy(pTmpV,pV,s32Size);

    for(i = 0;i<s32Size>>1;i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;

    }
    for(i = 0;i<s32Size>>1;i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    free( ptu );
    free( ptv );

    return HI_SUCCESS;
}

/******************************************************************************
* function : Get from YUV
******************************************************************************/
HI_S32 HI3531D_COMM_VI_GetVFrameFromYUV(FILE *pYUVFile, HI_U32 u32Width, HI_U32 u32Height,HI_U32 u32Stride, VIDEO_FRAME_INFO_S *pstVFrameInfo)
{
    HI_U32             u32LStride;
    HI_U32             u32CStride;
    HI_U32             u32LumaSize;
    HI_U32             u32ChrmSize;
    HI_U32             u32Size;
    VB_BLK VbBlk;
    HI_U32 u32PhyAddr;
    HI_U8 *pVirAddr;

    u32LStride  = u32Stride;
    u32CStride  = u32Stride;

    u32LumaSize = (u32LStride * u32Height);
    u32ChrmSize = (u32CStride * u32Height) >> 2;/* YUV 420 */
    u32Size = u32LumaSize + (u32ChrmSize << 1);

    /* alloc video buffer block ---------------------------------------------------------- */
    VbBlk = HI_MPI_VB_GetBlock(VB_INVALID_POOLID, u32Size, NULL);
    if (VB_INVALID_HANDLE == VbBlk)
    {
        SAMPLE_PRT("HI_MPI_VB_GetBlock err! size:%d\n",u32Size);
        return -1;
    }
    u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
    if (0 == u32PhyAddr)
    {
        return -1;
    }

    pVirAddr = (HI_U8 *) HI_MPI_SYS_Mmap(u32PhyAddr, u32Size);
    if (NULL == pVirAddr)
    {
        return -1;
    }

    pstVFrameInfo->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
    if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
    {   
        HI_MPI_SYS_Munmap(pVirAddr, u32Size);
        return -1;
    }
    SAMPLE_PRT("pool id :%d, phyAddr:%x,virAddr:%x\n" ,pstVFrameInfo->u32PoolId,u32PhyAddr,(int)pVirAddr);

    pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
    pstVFrameInfo->stVFrame.u32PhyAddr[1] = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.u32PhyAddr[2] = pstVFrameInfo->stVFrame.u32PhyAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
    pstVFrameInfo->stVFrame.pVirAddr[1] = pstVFrameInfo->stVFrame.pVirAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.pVirAddr[2] = pstVFrameInfo->stVFrame.pVirAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;
    pstVFrameInfo->stVFrame.u32Stride[0] = u32LStride;
    pstVFrameInfo->stVFrame.u32Stride[1] = u32CStride;
    pstVFrameInfo->stVFrame.u32Stride[2] = u32CStride;
    pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    pstVFrameInfo->stVFrame.u32Field = VIDEO_FIELD_INTERLACED;/* Intelaced D1,otherwise VIDEO_FIELD_FRAME */

    /* read Y U V data from file to the addr ----------------------------------------------*/
    HI3531D_COMM_VI_ReadFrame(pYUVFile, (HI_U8 *)pstVFrameInfo->stVFrame.pVirAddr[0],
       (HI_U8 *)pstVFrameInfo->stVFrame.pVirAddr[1], (HI_U8 *)pstVFrameInfo->stVFrame.pVirAddr[2],
       pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height,
       pstVFrameInfo->stVFrame.u32Stride[0], pstVFrameInfo->stVFrame.u32Stride[1] >> 1 );

    /* convert planar YUV420 to sem-planar YUV420 -----------------------------------------*/
    HI3531D_COMM_VI_PlanToSemi((HI_U8 *)pstVFrameInfo->stVFrame.pVirAddr[0], pstVFrameInfo->stVFrame.u32Stride[0],
      (HI_U8 *)pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.u32Stride[1],
      (HI_U8 *)pstVFrameInfo->stVFrame.pVirAddr[2], pstVFrameInfo->stVFrame.u32Stride[1],
      pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height);

    HI_MPI_SYS_Munmap(pVirAddr, u32Size);
    return 0;
}

HI_S32 HI3531D_COMM_VI_ChangeCapSize(VI_CHN ViChn, HI_U32 u32CapWidth, HI_U32 u32CapHeight,HI_U32 u32Width, HI_U32 u32Height)
{
    VI_CHN_ATTR_S stChnAttr;
	HI_S32 S32Ret = HI_SUCCESS;
	S32Ret = HI_MPI_VI_GetChnAttr(ViChn, &stChnAttr);
	if(HI_SUCCESS!= S32Ret)
	{
	    SAMPLE_PRT( "HI_MPI_VI_GetChnAttr failed\n");
	}
    stChnAttr.stCapRect.u32Width = u32CapWidth;
    stChnAttr.stCapRect.u32Height = u32CapHeight;
    stChnAttr.stDestSize.u32Width = u32Width;
    stChnAttr.stDestSize.u32Height = u32Height;

    S32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
	if(HI_SUCCESS!= S32Ret)
	{
	    SAMPLE_PRT( "HI_MPI_VI_SetChnAttr failed\n");
	}

    return HI_SUCCESS;
}

HI_S32 HI3531D_COMM_VI_ChangeDestSize(VI_CHN ViChn, HI_U32 u32Width, HI_U32 u32Height)
{
    VI_CHN_ATTR_S stChnAttr;
	HI_S32 S32Ret = HI_SUCCESS;
	S32Ret = HI_MPI_VI_GetChnAttr(ViChn, &stChnAttr);
	if(HI_SUCCESS!= S32Ret)
	{
	    SAMPLE_PRT( "HI_MPI_VI_GetChnAttr failed\n");
	}
    stChnAttr.stDestSize.u32Width = u32Width;
    stChnAttr.stDestSize.u32Height = u32Height;

    S32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
	if(HI_SUCCESS!= S32Ret)
	{
	    SAMPLE_PRT( "HI_MPI_VI_SetChnAttr failed\n");
	}

    return HI_SUCCESS;
}


/*****************************************************************************
* function : star vi according to product type
*           only for  MixCap 
*****************************************************************************/
HI_S32 HI3531D_COMM_VI_MixCap_Start(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm)
{
    VI_DEV ViDev;
    VI_CHN ViChn;
    HI_S32 i;
    HI_S32 s32Ret;
    SAMPLE_VI_PARAM_S stViParam;
    VI_CHN_ATTR_S stChnAttr,stChnMinorAttr;
	
    /*** get parameter from Sample_Vi_Mode ***/
    s32Ret = HI3531D_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("vi get param failed!\n");
        return HI_FAILURE;
    }
#if 0    
    /*** Start AD ***/
    s32Ret = HI3531D_COMM_VI_ADStart(enViMode, enNorm);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("Start AD failed!\n");
        return HI_FAILURE;
    }
  #endif  
    /*** Start VI Dev ***/
    for(i=0; i<stViParam.s32ViDevCnt; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;
        s32Ret = HI3531D_COMM_VI_StartDev(ViDev, enViMode);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI3531D_COMM_VI_StartDev failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }
    
    /*** Start VI Chn ***/
    for(i=0; i<stViParam.s32ViChnCnt; i++)
    {
        ViChn = i * stViParam.s32ViChnInterval;

	    stChnAttr.stCapRect.s32X = 0;
	    stChnAttr.stCapRect.s32Y = 0;
	    stChnAttr.enCapSel = VI_CAPSEL_BOTH;                      
	    stChnAttr.enPixFormat = SAMPLE_PIXEL_FORMAT;   /* sp420 or sp422 */
	    stChnAttr.bMirror = HI_FALSE;                             
	    stChnAttr.bFlip   = HI_FALSE;                              
	    stChnAttr.s32SrcFrameRate = -1;
	    stChnAttr.s32DstFrameRate = -1;
		stChnAttr.stCapRect.u32Width= D1_WIDTH;
	    stChnAttr.stCapRect.u32Height= (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;	
	    stChnAttr.stDestSize.u32Width= D1_WIDTH;
	    stChnAttr.stDestSize.u32Height= (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
		
		memcpy(&stChnMinorAttr, &stChnAttr, sizeof(VI_CHN_ATTR_S));
	    stChnMinorAttr.stDestSize.u32Width= D1_WIDTH / 2;
	    stChnMinorAttr.stDestSize.u32Height= (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;

		stChnAttr.s32SrcFrameRate = (VIDEO_ENCODING_MODE_PAL== enNorm)?25:30;
		stChnAttr.s32DstFrameRate = 6;
		
		s32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("call HI_MPI_VI_SetChnAttr failed with %#x\n", s32Ret);
            return HI_FAILURE;
        } 
		s32Ret = HI_MPI_VI_SetChnMinorAttr(ViChn, &stChnMinorAttr);
		if (HI_SUCCESS != s32Ret)
		{
			SAMPLE_PRT("call HI_MPI_VI_SetChnMinorAttr failed with %#x\n", s32Ret);
			return HI_FAILURE;
		} 
		s32Ret = HI_MPI_VI_EnableChn(ViChn);
		if (s32Ret != HI_SUCCESS)
		{
			SAMPLE_PRT("HI_MPI_VI_EnableChn failed with %#x!\n", s32Ret);
			return HI_FAILURE;
		}

    }

    return HI_SUCCESS;
}

HI_S32 HI3531D_COMM_VI_ChangeMixCap(VI_CHN ViChn,HI_BOOL bMixCap,HI_U32 FrameRate)
{
    VI_CHN_ATTR_S stChnAttr,stChnMinorAttr;
	HI_S32 S32Ret = HI_SUCCESS;
	S32Ret = HI_MPI_VI_GetChnAttr(ViChn, &stChnAttr);
	if(HI_SUCCESS!= S32Ret)
	{
	    SAMPLE_PRT( "HI_MPI_VI_GetChnAttr failed");
	}
	
	if(HI_TRUE == bMixCap)
	{
		memcpy(&stChnMinorAttr, &stChnAttr, sizeof(VI_CHN_ATTR_S));
	    stChnMinorAttr.stDestSize.u32Width = D1_WIDTH / 2;
		
		stChnAttr.s32DstFrameRate = FrameRate;
		
		S32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
        if (HI_SUCCESS != S32Ret)
        {
            SAMPLE_PRT("call HI_MPI_VI_SetChnAttr failed with %#x\n", S32Ret);
            return HI_FAILURE;
        } 
		S32Ret = HI_MPI_VI_SetChnMinorAttr(ViChn, &stChnMinorAttr);
		if (HI_SUCCESS != S32Ret)
		{
			SAMPLE_PRT("call HI_MPI_VI_SetChnMinorAttr failed with %#x\n", S32Ret);
			return HI_FAILURE;
		} 
	}
	else
	{
		stChnAttr.s32DstFrameRate = stChnAttr.s32SrcFrameRate;
		S32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
        if (HI_SUCCESS != S32Ret)
        {
            SAMPLE_PRT("call HI_MPI_VI_SetChnAttr failed with %#x\n", S32Ret);
            return HI_FAILURE;
        } 
	}
	return HI_SUCCESS;
}
