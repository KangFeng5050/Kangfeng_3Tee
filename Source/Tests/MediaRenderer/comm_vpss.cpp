/******************************************************************************
  Some simple Hisilicon Hi35xx video input functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

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

/******************************************************************************
* function : Set vpss system memory location
******************************************************************************/
HI_S32 HI3531D_COMM_VPSS_MemConfig()
{
    HI_CHAR * pcMmzName;
    MPP_CHN_S stMppChnVpss;
    HI_S32 s32Ret, i;

    /*vpss group max is VPSS_MAX_GRP_NUM, not need config vpss chn.*/
    for(i=0;i<VPSS_MAX_GRP_NUM;i++)
    {
        stMppChnVpss.enModId  = HI_ID_VPSS;
        stMppChnVpss.s32DevId = i;
        stMppChnVpss.s32ChnId = 0;

        if(0 == (i%2))
        {
            pcMmzName = NULL;  
        }
        else
        {
            pcMmzName = "ddr1";
        }

        /*vpss*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVpss, pcMmzName);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Vpss HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}
/*****************************************************************************
* function : start vpss. VPSS chn with frame
*****************************************************************************/
HI_S32 HI3531D_COMM_VPSS_StartEx(HI_S32 s32GrpCnt, SIZE_S *pstSize, HI_S32 s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stGrpAttr = {HI_FALSE};
    VPSS_CHN_ATTR_S stChnAttr = {HI_FALSE};
    VPSS_GRP_PARAM_S stVpssParam = {0};
    HI_S32 s32Ret;
    HI_S32 i, j;

    /*** Set Vpss Grp Attr ***/

    if(NULL == pstVpssGrpAttr)
    {
        stGrpAttr.u32MaxW = pstSize->u32Width;
        stGrpAttr.u32MaxH = pstSize->u32Height;
        stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
        
        stGrpAttr.bIeEn = HI_FALSE;
        stGrpAttr.bNrEn = HI_TRUE;
        stGrpAttr.bDciEn = HI_FALSE;
        stGrpAttr.bHistEn = HI_FALSE;
        stGrpAttr.bEsEn = HI_FALSE;
        stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
    }
    else
    {
        memcpy(&stGrpAttr,pstVpssGrpAttr,sizeof(VPSS_GRP_ATTR_S));
    }
    

    //for(i=0; i<s32GrpCnt; i++)
   // {
        VpssGrp = s32GrpCnt;
        /*** create vpss group ***/
        s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stGrpAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        /*** set vpss param ***/
        s32Ret = HI_MPI_VPSS_GetGrpParam(VpssGrp, &stVpssParam);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
        
        stVpssParam.u32IeStrength = 0;
        s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        /*** enable vpss chn, with frame ***/
        for(j=0; j<s32ChnCnt; j++)
        {
            VpssChn = j;
            /* Set Vpss Chn attr */
            stChnAttr.bSpEn = HI_FALSE;
            stChnAttr.bUVInvert = HI_FALSE;
            stChnAttr.bBorderEn = HI_TRUE;
            stChnAttr.stBorder.u32Color = 0xffffff;
            stChnAttr.stBorder.u32LeftWidth = 2;
            stChnAttr.stBorder.u32RightWidth = 2;
            stChnAttr.stBorder.u32TopWidth = 2;
            stChnAttr.stBorder.u32BottomWidth = 2;
            
            s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("HI_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }
    
            s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("HI_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }
       // }
        
        /*** start vpss group ***/
        s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }

    }
    return HI_SUCCESS;
}
/*****************************************************************************
* function : start vpss. VPSS chn with frame
*****************************************************************************/
HI_S32 HI3531D_COMM_VPSS_Start(VPSS_GRP VpssGrp,  HI_S32 VpssChnCnt, vi_venc_par& vp_m, vi_venc_par& vp_e, VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
	VPSS_GRP_ATTR_S stGrpAttr = {0};
	VPSS_CHN_ATTR_S stChnAttr;
	VPSS_GRP_PARAM_S stVpssParam = {0};
	HI_S32 s32Ret;
	HI_S32 i, j;
	
	VPSS_CHN_MODE_S stVpssMode;
	vi_venc_par vp;

	/*** Set Vpss Grp Attr ***/

	if(NULL == pstVpssGrpAttr)
	{
	#if 1
		if(vp_m.vi_cap_width >= vp_e.vi_cap_width){
			stGrpAttr.u32MaxW = vp_m.vi_cap_width;
			stGrpAttr.u32MaxH = vp_m.vi_cap_height;
		}else{
			stGrpAttr.u32MaxW = vp_e.vi_cap_width;
			stGrpAttr.u32MaxH = vp_e.vi_cap_height;
		}
	#endif	
		stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;

		stGrpAttr.bIeEn = HI_FALSE;
		stGrpAttr.bNrEn = HI_TRUE;
		stGrpAttr.bDciEn = HI_FALSE;
		stGrpAttr.bHistEn = HI_FALSE;
		stGrpAttr.bEsEn = HI_FALSE;
		stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
	}
	else
	{
		memcpy(&stGrpAttr,pstVpssGrpAttr,sizeof(VPSS_GRP_ATTR_S));
	}


	    /*** create vpss group ***/
	    s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stGrpAttr);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	    /*** set vpss param ***/
	s32Ret = HI_MPI_VPSS_GetGrpParam(VpssGrp, &stVpssParam);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
	    
	stVpssParam.u32IeStrength = 0;
	s32Ret = HI_MPI_VPSS_SetGrpParam(VpssGrp, &stVpssParam);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	for(int chn = 0; chn < VpssChnCnt;chn++){
#if 1	
		if(chn == 0)
			memcpy(&vp,&vp_m,sizeof(vi_venc_par));
		else
			memcpy(&vp,&vp_e,sizeof(vi_venc_par));
		s32Ret = HI_MPI_VPSS_GetChnMode(VpssGrp, chn, &stVpssMode);
		if (s32Ret != HI_SUCCESS)
		{
			printf("HI_MPI_VPSS_GetChnMode fail, s32Ret: 0x%x.\n",s32Ret);
			return HI_FAILURE;
		}

		stVpssMode.enChnMode		= VPSS_CHN_MODE_USER;
		stVpssMode.enCompressMode	= COMPRESS_MODE_NONE;
		stVpssMode.bDouble			= HI_FALSE;
		stVpssMode.u32Width 		= vp.venc_width;
		stVpssMode.u32Height		 = vp.venc_height;
		stVpssMode.stFrameRate.s32DstFrmRate = -1;//60;
		stVpssMode.stFrameRate.s32SrcFrmRate =	-1;//60;
		stVpssMode.stAspectRatio.enMode = ASPECT_RATIO_NONE;//ASPECT_RATIO_MANUAL;
		stVpssMode.stAspectRatio.u32BgColor = 0;
		stVpssMode.stAspectRatio.stVideoRect.u32Width=vp.venc_width;// vp.vi_cap_width;
		stVpssMode.stAspectRatio.stVideoRect.u32Height = vp.venc_height;//vp.vi_cap_height;
		stVpssMode.enPixelFormat	= PIXEL_FORMAT_YUV_SEMIPLANAR_420;
		s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, chn, &stVpssMode);
		if (s32Ret != HI_SUCCESS)
		{
			printf("set vpss grp%d chn%d mode fail, s32Ret: 0x%x.\n", VpssGrp, chn, s32Ret);
			return HI_FAILURE;
		}
#endif
		/* Set Vpss Chn attr */
		stChnAttr.bSpEn = HI_FALSE;
		stChnAttr.bUVInvert = HI_FALSE;
		stChnAttr.bBorderEn = HI_FALSE;
		stChnAttr.stBorder.u32Color = 0x0;
		stChnAttr.stBorder.u32LeftWidth = 2;
		stChnAttr.stBorder.u32RightWidth = 2;
		stChnAttr.stBorder.u32TopWidth = 2;
		stChnAttr.stBorder.u32BottomWidth = 2;

		s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, chn, &stChnAttr);
		if (s32Ret != HI_SUCCESS)
		{
			SAMPLE_PRT("HI_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
			return HI_FAILURE;
		}

		s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, chn);
		if (s32Ret != HI_SUCCESS)
		{
			SAMPLE_PRT("HI_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
			return HI_FAILURE;
		}
	}
	    
	/*** start vpss group ***/
	s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
		return HI_FAILURE;
	}
	
	return HI_SUCCESS;
}

/*****************************************************************************
* function : disable vi dev
*****************************************************************************/
HI_S32 HI3531D_COMM_VPSS_Stop(VPSS_GRP VpssGrp, HI_S32 VpssChnCnt)
{
	HI_S32 i, j;
	HI_S32 s32Ret = HI_SUCCESS;
	VPSS_CHN VpssChn;
	s32Ret = HI_MPI_VPSS_StopGrp(VpssGrp);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}
		for(j=0; j<VpssChnCnt; j++)
		{
		VpssChn = j;
		s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
		if (s32Ret != HI_SUCCESS)
		{
			SAMPLE_PRT("failed with %#x!\n", s32Ret);
			return HI_FAILURE;
		}
	}
	
	s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
	if (s32Ret != HI_SUCCESS)
	{
		SAMPLE_PRT("failed with %#x!\n", s32Ret);
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}

HI_S32 HI3531D_COMM_DisableVpssPreScale(VPSS_GRP VpssGrp,SIZE_S stSize)
{
    HI_S32 s32Ret;
    VPSS_PRESCALE_INFO_S stPreScaleInfo;
        
    stPreScaleInfo.bPreScale = HI_FALSE;
    stPreScaleInfo.stDestSize.u32Width = stSize.u32Width;
    stPreScaleInfo.stDestSize.u32Height = stSize.u32Height;
    s32Ret = HI_MPI_VPSS_SetPreScale(VpssGrp, &stPreScaleInfo);
    if (s32Ret != HI_SUCCESS)
	{
	    SAMPLE_PRT("HI_MPI_VPSS_SetPreScale failed with %#x!\n", s32Ret);
	    return HI_FAILURE;
	}

    return s32Ret;
}
HI_S32 HI3531D_COMM_EnableVpssPreScale(VPSS_GRP VpssGrp,SIZE_S stSize)
{
    HI_S32 s32Ret;
    VPSS_PRESCALE_INFO_S stPreScaleInfo;
        
    stPreScaleInfo.bPreScale = HI_TRUE;
    stPreScaleInfo.stDestSize.u32Width = stSize.u32Width;
    stPreScaleInfo.stDestSize.u32Height = stSize.u32Height;
    s32Ret = HI_MPI_VPSS_SetPreScale(VpssGrp, &stPreScaleInfo);
    if (s32Ret != HI_SUCCESS)
	{
	    SAMPLE_PRT("HI_MPI_VPSS_SetPreScale failed with %#x!\n", s32Ret);
	    return HI_FAILURE;
	}

    return s32Ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
