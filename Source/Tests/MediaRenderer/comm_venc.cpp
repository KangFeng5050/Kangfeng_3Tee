/******************************************************************************
  Some simple Hisilicon Hi35xx video encode functions.  Copyright (C), 2016-2017, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>

#include "comm.h"
#include "deployfile.h"
static pthread_t gs_VencPid;
static SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
static HI_S32 gs_s32SnapCnt = 0;

#define CHANEL_NUM	2
extern bool g_enc_run;
extern bool set_enc_run[3];
extern bool g_enc_reset[CHANEL_NUM];
extern bool g_vi_need_flush[CHANEL_NUM];
extern bool g_enc_reset_end[CHANEL_NUM];
extern bool g_mpp_reset;
/******************************************************************************
* function : Set venc memory location
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_MemConfig(HI_VOID)
{
    HI_S32 i = 0;
    HI_S32 s32Ret;
    HI_CHAR* pcMmzName;
    MPP_CHN_S stMppChnVENC;
    /* group, venc max chn is 64*/
    for (i = 0; i < 64; i++)
    {
        stMppChnVENC.enModId = HI_ID_VENC;
        stMppChnVENC.s32DevId = 0;
        stMppChnVENC.s32ChnId = i;
        pcMmzName = NULL;

        /*venc*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVENC, pcMmzName);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_SYS_SetMemConf with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

/******************************************************************************
* function : venc bind vi
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_BindVi(VENC_CHN VeChn, VI_CHN ViChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VIU;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = ViChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc unbind vi
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_UnBindVi(VENC_CHN VeChn, VI_CHN ViChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VIU;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = ViChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}


/******************************************************************************
* function : venc bind vpss
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_BindVpss(VENC_CHN VeChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc unbind vpss
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_UnBindVpss(VENC_CHN VeChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VPSS;
    stSrcChn.s32DevId = VpssGrp;
    stSrcChn.s32ChnId = VpssChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc bind vo
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_BindVo(VO_DEV VoDev, VO_CHN VoChn, VENC_CHN VeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VOU;
    stSrcChn.s32DevId = VoDev;
    stSrcChn.s32ChnId = VoChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc unbind vo
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_UnBindVo(VENC_CHN GrpChn, VO_DEV VoDev, VO_CHN VoChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VOU;
    stSrcChn.s32DevId = VoDev;
    stSrcChn.s32ChnId = VoChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = GrpChn;
    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : vdec bind venc
******************************************************************************/
HI_S32 HI3531D_COMM_VDEC_BindVenc(VDEC_CHN VdChn, VENC_CHN VeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;
    s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* function : venc unbind vo
******************************************************************************/
HI_S32 HI3531D_COMM_VDEC_UnBindVenc(VDEC_CHN VdChn, VENC_CHN VeChn)
{
    HI_S32 s32Ret = HI_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = VdChn;
    stDestChn.enModId = HI_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VeChn;

    s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* funciton : get file postfix according palyload_type.
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char* szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strncpy(szFilePostfix, ".h264", 10);
    }
    else if (PT_H265 == enPayload)
    {
        strncpy(szFilePostfix, ".h265", 10);
    }
    else if (PT_JPEG == enPayload)
    {
        strncpy(szFilePostfix, ".jpg", 10);
    }
    else if (PT_MJPEG == enPayload)
    {
        strncpy(szFilePostfix, ".mjp", 10);
    }
    else if (PT_MP4VIDEO == enPayload)
    {
        strncpy(szFilePostfix, ".mp4", 10);
    }
    else
    {
        SAMPLE_PRT("payload type err!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 HI3531D_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr,VIDEO_NORM_E enNorm)
{
	switch(enGopMode)
	{
		case VENC_GOPMODE_NORMALP:
		pstGopAttr->enGopMode  = VENC_GOPMODE_NORMALP;
		pstGopAttr->stNormalP.s32IPQpDelta = 2;
		break;

		case VENC_GOPMODE_SMARTP:
		pstGopAttr->enGopMode  = VENC_GOPMODE_SMARTP;
		pstGopAttr->stSmartP.s32BgQpDelta = 4;
		pstGopAttr->stSmartP.s32ViQpDelta = 2;
		pstGopAttr->stSmartP.u32BgInterval = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 75 : 90;
		break;
		case VENC_GOPMODE_DUALP:
		pstGopAttr->enGopMode  = VENC_GOPMODE_DUALP;
		pstGopAttr->stDualP.s32IPQpDelta  = 4;
		pstGopAttr->stDualP.s32SPQpDelta  = 2;
		pstGopAttr->stDualP.u32SPInterval = 3;
		break;
		default:
		SAMPLE_PRT("not support the gop mode !\n");
		return HI_FAILURE;
		break;
	}
	return HI_SUCCESS;
}
/******************************************************************************
* funciton : save mjpeg stream.
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SaveMJpeg(VENC_STREAM_S* pstStream)
{
#if 0
    VENC_PACK_S*  pstData;
    HI_U32 i;
    
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr + pstData->u32Offset, pstData->u32Len - pstData->u32Offset, 1, fpMJpegFile);
        fflush(fpMJpegFile);
    }
#endif
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save jpeg stream.
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SaveJpeg(FILE* fpJpegFile, VENC_STREAM_S* pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr + pstData->u32Offset, pstData->u32Len - pstData->u32Offset, 1, fpJpegFile);
        fflush(fpJpegFile);
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save H264 stream
******************************************************************************/

HI_S32 HI3531D_COMM_VENC_SaveH264(HI_S32 s32Chn, VENC_STREAM_S *pstStream)
{
	if(s32Chn == 0){
		std::string data;
		
		for(HI_S32 i = 0; i < pstStream->u32PackCount; i++){
			std::string buf;
			buf.assign((char*)pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset,pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset);
			data+= buf;
		}
		if(DeplyPar::Instance()->m_pipe){
			DeplyPar::Instance()->m_pipe->IncomingH264Data((uint8*)data.c_str(), data.length());
			printf("-----------Import HDMI1\n");
		}
		
	}	
	else if(s32Chn == 1){
		std::string data;
		
		for(HI_S32 i = 0; i < pstStream->u32PackCount; i++){
			std::string buf;
			buf.assign((char*)pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset,pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset);
			data+= buf;
			
		}
		
		if(DeplyPar::Instance()->m_pipe2){
			DeplyPar::Instance()->m_pipe2->IncomingH264Data((uint8*)data.c_str(), data.length());
			printf("-----------Import HDMI2\n");
		}
		SendStream_H264(data.c_str(),data.length());
	}
	
	return HI_SUCCESS;
}


/******************************************************************************
* funciton : save H265 stream
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SaveH265(HI_S32 s32Chn, VENC_STREAM_S *pstStream)
{
#if 0
	for(HI_S32 i = 0; i < pstStream->u32PackCount; i++)
	{
		fwrite(pstStream->pstPack[i].pu8Addr+pstStream->pstPack[i].u32Offset,
			   pstStream->pstPack[i].u32Len-pstStream->pstPack[i].u32Offset, 1, fpH265File);
		fflush(fpH265File); 

	}
#endif
	return HI_SUCCESS;
}


/******************************************************************************
* funciton : save jpeg stream
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SaveJPEG(FILE* fpJpegFile, VENC_STREAM_S* pstStream)
{
    VENC_PACK_S*  pstData;
    HI_U32 i;
    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        pstData = &pstStream->pstPack[i];
        fwrite(pstData->pu8Addr + pstData->u32Offset, pstData->u32Len - pstData->u32Offset, 1, fpJpegFile);
        fflush(fpJpegFile);
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save snap stream
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SaveSnap(VENC_STREAM_BUF_INFO_S* pstStreamBufInfo,VENC_STREAM_S* pstStream)
{
    char acFile[FILE_NAME_LEN]  = {0};
    //char acFile_dcf[FILE_NAME_LEN]  = {0};
    FILE* pFile;
    HI_S32 s32Ret;

    snprintf(acFile, FILE_NAME_LEN, "snap_%d.jpg", gs_s32SnapCnt);

    pFile = fopen(acFile, "wb");
    if (pFile == NULL)
    {
        SAMPLE_PRT("open file err\n");
        return HI_FAILURE;
    }

    s32Ret = HI3531D_COMM_VENC_SaveJPEG(pFile, pstStream);

    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("save snap picture failed!\n");
        return HI_FAILURE;
    }
    fclose(pFile);
    gs_s32SnapCnt++;
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : save stream
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SaveStream(HI_S32 s32Chn, PAYLOAD_TYPE_E enType, VENC_STREAM_S *pstStream)
{
    HI_S32 s32Ret;
    if (PT_H264 == enType)
    {
        s32Ret = HI3531D_COMM_VENC_SaveH264(s32Chn, pstStream);  
    }
    else if (PT_MJPEG == enType)
    {
        s32Ret = HI3531D_COMM_VENC_SaveMJpeg(pstStream);
    }
    else if (PT_H265 == enType)
    {
        s32Ret = HI3531D_COMM_VENC_SaveH265(s32Chn, pstStream);  
    }
    else
    {
        return HI_FAILURE;
    }
    return s32Ret;
}

/******************************************************************************
* funciton : the process of physical address retrace
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SaveFile(FILE* pFd, VENC_STREAM_BUF_INFO_S *pstStreamBuf, VENC_STREAM_S* pstStream)
{
    HI_U32 i;
    HI_U32 u32SrcPhyAddr;
    HI_U32 u32Left;
    HI_S32 s32Ret = 0;
    
    for(i=0; i<pstStream->u32PackCount; i++)
    {
        if (pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Len >=
                pstStreamBuf->u32PhyAddr + pstStreamBuf->u32BufSize)
            {
                if (pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Offset >=
                    pstStreamBuf->u32PhyAddr + pstStreamBuf->u32BufSize)
                {
                    /* physical address retrace in offset segment */
                    u32SrcPhyAddr = pstStreamBuf->u32PhyAddr +
                                    ((pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Offset) - 
                                    (pstStreamBuf->u32PhyAddr + pstStreamBuf->u32BufSize));

                    s32Ret = fwrite ((void *)u32SrcPhyAddr, pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
                    if(s32Ret<0)
                    {
                        SAMPLE_PRT("fwrite err %d\n", s32Ret);
                        return s32Ret;
                    }
                }
                else
                {
                    /* physical address retrace in data segment */
                    u32Left = (pstStreamBuf->u32PhyAddr + pstStreamBuf->u32BufSize) - pstStream->pstPack[i].u32PhyAddr;

                    s32Ret = fwrite((void *)(pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Offset),
                                 u32Left - pstStream->pstPack[i].u32Offset, 1, pFd);
                    if(s32Ret<0)
                    {
                        SAMPLE_PRT("fwrite err %d\n", s32Ret);
                        return s32Ret;
                    }
                    
                    s32Ret = fwrite((void *)pstStreamBuf->u32PhyAddr, pstStream->pstPack[i].u32Len - u32Left, 1, pFd);
                    if(s32Ret<0)
                    {
                        SAMPLE_PRT("fwrite err %d\n", s32Ret);
                        return s32Ret;
                    }
                }
            }
            else
            {
                /* physical address retrace does not happen */
                s32Ret = fwrite ((void *)(pstStream->pstPack[i].u32PhyAddr + pstStream->pstPack[i].u32Offset),
                              pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
                if(s32Ret<0)
                {
                    SAMPLE_PRT("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }
            }
			fflush(pFd);
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Start venc stream mode (h264, mjpeg)
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, vi_venc_par &vp)
{
	HI_S32 s32Ret;
	VENC_CHN_ATTR_S stVencChnAttr;
	VENC_ATTR_H264_S stH264Attr;
	VENC_ATTR_H264_CBR_S    stH264Cbr;
	VENC_ATTR_H264_VBR_S    stH264Vbr;
	VENC_ATTR_H264_AVBR_S    stH264AVbr;
	VENC_ATTR_H264_FIXQP_S  stH264FixQp;
	VENC_ATTR_H265_S        stH265Attr;
	VENC_ATTR_H265_CBR_S    stH265Cbr;
	VENC_ATTR_H265_VBR_S    stH265Vbr;
	VENC_ATTR_H265_AVBR_S    stH265AVbr;
	VENC_ATTR_H265_FIXQP_S  stH265FixQp;
	VENC_ATTR_MJPEG_S stMjpegAttr;
	VENC_ATTR_MJPEG_FIXQP_S stMjpegeFixQp;
	VENC_ATTR_JPEG_S stJpegAttr;
	SIZE_S stPicSize;

	/******************************************
	step 1:  Create Venc Channel    
	******************************************/
	stVencChnAttr.stVeAttr.enType = enType;
	switch (enType)
	{
		case PT_H264:
		{
			stH264Attr.u32MaxPicWidth = vp.venc_width;
			stH264Attr.u32MaxPicHeight = vp.venc_height;
			stH264Attr.u32PicWidth = vp.venc_width; 					/*the picture width*/
			stH264Attr.u32PicHeight = vp.venc_height;					/*the picture height*/
			stH264Attr.u32BufSize = vp.venc_width * vp.venc_height * 20; /*stream buffer size*/
			stH264Attr.u32Profile = vp.venc_profile;					/*0: baseline; 1:MP; 2:HP;	3:svc_t */
			stH264Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
			memcpy(&stVencChnAttr.stVeAttr.stAttrH264e, &stH264Attr, sizeof(VENC_ATTR_H264_S));
			if (SAMPLE_RC_CBR == vp.venc_rc_mode)
			{
				stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
				stH264Cbr.u32Gop            = vp.venc_gop;
				stH264Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
				stH264Cbr.u32SrcFrmRate      = vp.vi_framerate; /* input (vi) frame rate */
				stH264Cbr.fr32DstFrmRate = vp.venc_h264framerate; /* target frame rate */			
				stH264Cbr.u32BitRate 	= vp.venc_bitrate;                
				stH264Cbr.u32FluctuateLevel =  vp.venc_FluctuateLevel; /* average bit rate */
				memcpy(&stVencChnAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
			}
			else if (SAMPLE_RC_FIXQP == vp.venc_rc_mode)
			{
				stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
				stH264FixQp.u32Gop = vp.venc_gop;
				stH264FixQp.u32SrcFrmRate = vp.vi_framerate;
				stH264FixQp.fr32DstFrmRate = vp.venc_h264framerate;
				stH264FixQp.u32IQp = vp.venc_i_qp;
				stH264FixQp.u32PQp = vp.venc_p_qp;
				stH264FixQp.u32BQp = vp.venc_b_qp;
				memcpy(&stVencChnAttr.stRcAttr.stAttrH264FixQp, &stH264FixQp, sizeof(VENC_ATTR_H264_FIXQP_S));
			}
			else if (SAMPLE_RC_VBR == vp.venc_rc_mode)
			{
				stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
				stH264Vbr.u32Gop = vp.venc_gop;
				stH264Vbr.u32StatTime = 1;
				stH264Vbr.u32SrcFrmRate = vp.vi_framerate;
				stH264Vbr.fr32DstFrmRate =  vp.venc_h264framerate;
				stH264Vbr.u32MinQp = vp.venc_min_qp;
				stH264Vbr.u32MinIQp = vp.venc_min_i_qp;
				stH264Vbr.u32MaxQp =  vp.venc_max_qp;
				stH264Vbr.u32MaxBitRate 		= vp.venc_bitrate;
				memcpy(&stVencChnAttr.stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
			}
			else if (SAMPLE_RC_AVBR == vp.venc_rc_mode)
			{
				stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
				stH264AVbr.u32Gop = vp.venc_gop;
				stH264AVbr.u32StatTime = 1;
				stH264AVbr.u32SrcFrmRate = vp.vi_framerate;
				stH264AVbr.fr32DstFrmRate = vp.venc_h264framerate;
				stH264AVbr.u32MaxBitRate = vp.venc_bitrate;
				memcpy(&stVencChnAttr.stRcAttr.stAttrH264AVbr, &stH264AVbr, sizeof(VENC_ATTR_H264_AVBR_S));
			}			
			else
			{
				return HI_FAILURE;
			}
		}
		break;
		case PT_H265:
		{
			stH265Attr.u32MaxPicWidth = vp.venc_width;
			stH265Attr.u32MaxPicHeight = vp.venc_height;
			stH265Attr.u32PicWidth = vp.venc_width;  
			stH265Attr.u32PicHeight = vp.venc_height; 
			stH265Attr.u32BufSize  = vp.venc_width * vp.venc_height * 20; /*stream buffer size*/
			stH265Attr.u32Profile = 0; 
			stH265Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
			memcpy(&stVencChnAttr.stVeAttr.stAttrH265e, &stH265Attr, sizeof(VENC_ATTR_H265_S));
			
			if (SAMPLE_RC_CBR == vp.venc_rc_mode)
			{
				stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
				stH265Cbr.u32Gop            = vp.venc_gop;
				stH265Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
				stH265Cbr.u32SrcFrmRate      = vp.vi_framerate;
				stH265Cbr.fr32DstFrmRate = vp.venc_h265framerate;
				stH265Cbr.u32BitRate = vp.venc_bitrate;
				stH265Cbr.u32FluctuateLevel = vp.venc_FluctuateLevel; /* average bit rate */
				memcpy(&stVencChnAttr.stRcAttr.stAttrH265Cbr, &stH265Cbr, sizeof(VENC_ATTR_H265_CBR_S));
			}
			else if (SAMPLE_RC_FIXQP == vp.venc_rc_mode)
			{
				stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
				stH265FixQp.u32Gop = vp.venc_gop;
				stH265FixQp.u32SrcFrmRate = vp.vi_framerate;
				stH265FixQp.fr32DstFrmRate = vp.venc_h265framerate;
				stH265FixQp.u32IQp = vp.venc_i_qp;
				stH265FixQp.u32PQp = vp.venc_p_qp;
				stH265FixQp.u32BQp = vp.venc_b_qp;
				memcpy(&stVencChnAttr.stRcAttr.stAttrH265FixQp, &stH265FixQp, sizeof(VENC_ATTR_H265_FIXQP_S));
			}
			else if (SAMPLE_RC_VBR == vp.venc_rc_mode)
			{
				stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
				stH265Vbr.u32Gop = vp.venc_gop;
				stH265Vbr.u32StatTime = 1;
				stH265Vbr.u32SrcFrmRate = vp.vi_framerate;
				stH265Vbr.fr32DstFrmRate = vp.venc_h265framerate;
				stH265Vbr.u32MinQp  = vp.venc_min_qp;
				stH265Vbr.u32MinIQp = vp.venc_min_i_qp;
				stH265Vbr.u32MaxQp  = vp.venc_max_qp;
				stH265Vbr.u32MaxBitRate =  vp.venc_bitrate;
				
				SAMPLE_PRT("H265 VBR:minqp(%d)--miqp(%d)--maxqp(%d) \n", stH265Vbr.u32MinQp, stH265Vbr.u32MinIQp,stH265Vbr.u32MaxQp);
				memcpy(&stVencChnAttr.stRcAttr.stAttrH265Vbr, &stH265Vbr, sizeof(VENC_ATTR_H265_VBR_S));
			}
			else if (SAMPLE_RC_AVBR == vp.venc_rc_mode)
			{
				stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
				stH265AVbr.u32Gop = vp.venc_gop;
				stH265AVbr.u32StatTime = 1;
				stH265AVbr.u32SrcFrmRate = vp.vi_framerate;
				stH265AVbr.fr32DstFrmRate = vp.venc_h265framerate;
				stH265AVbr.u32MaxBitRate = vp.venc_bitrate;
				memcpy(&stVencChnAttr.stRcAttr.stAttrH264AVbr, &stH265AVbr, sizeof(VENC_ATTR_H265_AVBR_S));
			}	
			else
			{
				return HI_FAILURE;
			}
		}
		break;
		default:
		return HI_ERR_VENC_NOT_SUPPORT;
	}
	
//	HI3531D_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr,VIDEO_NORM_E enNorm);
	
	stVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
	stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 2;
	s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
		VencChn, s32Ret);
		return s32Ret;
	}    
	s32Ret = HI_MPI_VENC_StartRecvPic(VencChn);
	if (HI_SUCCESS != s32Ret)
	{
		SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
		return HI_FAILURE;
	}
	
	return HI_SUCCESS;
}
/******************************************************************************
* funciton : Start venc stream mode (h264, mjpeg)
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_StartEx(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, VIDEO_NORM_E enNorm, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, HI_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr)
{
    HI_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_ATTR_H264_S stH264Attr;
    VENC_ATTR_H264_CBR_S    stH264Cbr;
    VENC_ATTR_H264_VBR_S    stH264Vbr;
	VENC_ATTR_H264_AVBR_S    stH264AVbr;	
    VENC_ATTR_H264_FIXQP_S  stH264FixQp;
    VENC_ATTR_H265_S        stH265Attr;
    VENC_ATTR_H265_CBR_S    stH265Cbr;
    VENC_ATTR_H265_VBR_S    stH265Vbr;	
	VENC_ATTR_H265_AVBR_S    stH265AVbr;
    VENC_ATTR_H265_FIXQP_S  stH265FixQp;
    VENC_ATTR_MJPEG_S stMjpegAttr;
    VENC_ATTR_MJPEG_FIXQP_S stMjpegeFixQp;
    VENC_ATTR_JPEG_S stJpegAttr;
    SIZE_S stPicSize;
    s32Ret = HI3531D_COMM_SYS_GetPicSize(enNorm, enSize, &stPicSize);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get picture size failed!\n");
        return HI_FAILURE;
    }   
    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVeAttr.enType = enType;
    switch (enType)
    {
        case PT_H264:
        {
            stH264Attr.u32MaxPicWidth = stPicSize.u32Width;
            stH264Attr.u32MaxPicHeight = stPicSize.u32Height;
            stH264Attr.u32PicWidth = stPicSize.u32Width;/*the picture width*/
            stH264Attr.u32PicHeight = stPicSize.u32Height;/*the picture height*/
            stH264Attr.u32BufSize  = stPicSize.u32Width * stPicSize.u32Height * 2;/*stream buffer size*/
            stH264Attr.u32Profile  = u32Profile;/*0: baseline; 1:MP; 2:HP;  3:svc_t */
            stH264Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
            //stH264Attr.u32BFrameNum = 0;/* 0: not support B frame; >=1: number of B frames */
            //stH264Attr.u32RefNum = 1;/* 0: default; number of refrence frame*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrH264e, &stH264Attr, sizeof(VENC_ATTR_H264_S));
            if (SAMPLE_RC_CBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop            = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrmRate      = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30; /* target frame rate */
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH264Cbr.u32BitRate = 256; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH264Cbr.u32BitRate = 512;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
                        stH264Cbr.u32BitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH264Cbr.u32BitRate = 1024 * 2;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH264Cbr.u32BitRate = 1024 * 4;
                        break;
                    default :
                        stH264Cbr.u32BitRate = 1024 * 4;
                        break;
                }
                stH264Cbr.u32FluctuateLevel = 1; /* average bit rate */
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264Cbr, &stH264Cbr, sizeof(VENC_ATTR_H264_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264FixQp.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264FixQp.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264FixQp.u32IQp = 20;
                stH264FixQp.u32PQp = 23;
                stH264FixQp.u32BQp = 23;
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264FixQp, &stH264FixQp, sizeof(VENC_ATTR_H264_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264Vbr.u32StatTime = 1;
                stH264Vbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264Vbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264Vbr.u32MinQp = 10;
                stH264Vbr.u32MinIQp = 10;
                stH264Vbr.u32MaxQp = 40;
				
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH264Vbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH264Vbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
                        stH264Vbr.u32MaxBitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH264Vbr.u32MaxBitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH264Vbr.u32MaxBitRate = 1024 * 6;
                        break;
                    default :
                        stH264Vbr.u32MaxBitRate = 1024 * 4;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264Vbr, &stH264Vbr, sizeof(VENC_ATTR_H264_VBR_S));
            }
            else if (SAMPLE_RC_AVBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                stH264AVbr.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264AVbr.u32StatTime = 1;
                stH264AVbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH264AVbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH264AVbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH264AVbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
                        stH264AVbr.u32MaxBitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH264AVbr.u32MaxBitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH264AVbr.u32MaxBitRate = 1024 * 4;
                        break;
					case PIC_UHD4K:
                        stH264AVbr.u32MaxBitRate = 1024 * 8;	
                        break;
                    default :
                        stH264AVbr.u32MaxBitRate = 1024 * 4;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264AVbr, &stH264AVbr, sizeof(VENC_ATTR_H264_AVBR_S));
            }				
            else
            {
                return HI_FAILURE;
            }
        }
        break;
        case PT_MJPEG:
        {
            stMjpegAttr.u32MaxPicWidth = stPicSize.u32Width;
            stMjpegAttr.u32MaxPicHeight = stPicSize.u32Height;
            stMjpegAttr.u32PicWidth = stPicSize.u32Width;
            stMjpegAttr.u32PicHeight = stPicSize.u32Height;
            stMjpegAttr.u32BufSize = stPicSize.u32Width * stPicSize.u32Height * 3;
            stMjpegAttr.bByFrame = HI_TRUE;  /*get stream mode is field mode  or frame mode*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrMjpege, &stMjpegAttr, sizeof(VENC_ATTR_MJPEG_S));
            if (SAMPLE_RC_FIXQP == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
                stMjpegeFixQp.u32Qfactor        = 90;
                stMjpegeFixQp.u32SrcFrmRate      = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stMjpegeFixQp.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                memcpy(&stVencChnAttr.stRcAttr.stAttrMjpegeFixQp, &stMjpegeFixQp,
                       sizeof(VENC_ATTR_MJPEG_FIXQP_S));
            }
            else if (SAMPLE_RC_CBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
                stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32StatTime       = 1;
                stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32SrcFrmRate      = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stVencChnAttr.stRcAttr.stAttrMjpegeCbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32FluctuateLevel = 1;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 384 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 768 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 3 * 3;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 5 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 10 * 3;
                        break;
                    default :
                        stVencChnAttr.stRcAttr.stAttrMjpegeCbr.u32BitRate = 1024 * 10 * 3;
                        break;
                }
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32StatTime = 1;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.fr32DstFrmRate = 5;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MinQfactor = 50;
                stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxQfactor = 95;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 2 * 3;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 3 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 6 * 3;
                        break;
					case PIC_UHD4K:
                        stH264Cbr.u32BitRate 								 = 1024 * 12 * 3;
                        break;
                    default :
                        stVencChnAttr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = 1024 * 4 * 3;
                        break;
                }
            }
            else
            {
                SAMPLE_PRT("cann't support other mode in this version!\n");
                return HI_FAILURE;
            }
        }
        break;
        case PT_JPEG:
            stJpegAttr.u32PicWidth  = stPicSize.u32Width;
            stJpegAttr.u32PicHeight = stPicSize.u32Height;
            stJpegAttr.u32MaxPicWidth  = stPicSize.u32Width;
            stJpegAttr.u32MaxPicHeight = stPicSize.u32Height;
            stJpegAttr.u32BufSize   = stPicSize.u32Width * stPicSize.u32Height * 3;
            stJpegAttr.bByFrame     = HI_TRUE;/*get stream mode is field mode  or frame mode*/
            stJpegAttr.bSupportDCF  = HI_FALSE;
            memcpy(&stVencChnAttr.stVeAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        case PT_H265:
        {
            stH265Attr.u32MaxPicWidth = stPicSize.u32Width;
            stH265Attr.u32MaxPicHeight = stPicSize.u32Height;
            stH265Attr.u32PicWidth = stPicSize.u32Width;/*the picture width*/
            stH265Attr.u32PicHeight = stPicSize.u32Height;/*the picture height*/
            stH265Attr.u32BufSize  = stPicSize.u32Width * stPicSize.u32Height * 2;/*stream buffer size*/
            if (u32Profile >= 1)
            { stH265Attr.u32Profile = 0; }/*0:MP; */
            else            
			{ stH265Attr.u32Profile  = u32Profile; }/*0:MP*/
            stH265Attr.bByFrame = HI_TRUE;/*get stream mode is slice mode or frame mode?*/
            //stH265Attr.u32BFrameNum = 0;/* 0: not support B frame; >=1: number of B frames */
            //stH265Attr.u32RefNum = 1;/* 0: default; number of refrence frame*/
            memcpy(&stVencChnAttr.stVeAttr.stAttrH265e, &stH265Attr, sizeof(VENC_ATTR_H265_S));
            if (SAMPLE_RC_CBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265Cbr.u32StatTime       = 1; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrmRate      = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30; /* target frame rate */
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH265Cbr.u32BitRate = 256; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH265Cbr.u32BitRate = 512;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
                        stH265Cbr.u32BitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH265Cbr.u32BitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH265Cbr.u32BitRate = 1024 * 4;
                        break;
					case PIC_UHD4K:
                        stH265Cbr.u32BitRate = 1024 * 8;
                        break;
                    default :
                        stH265Cbr.u32BitRate = 1024 * 4;
                        break;
                }
                stH265Cbr.u32FluctuateLevel = 1; /* average bit rate */
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265Cbr, &stH265Cbr, sizeof(VENC_ATTR_H265_CBR_S));
            }
            else if (SAMPLE_RC_FIXQP == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265FixQp.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265FixQp.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265FixQp.u32IQp = 20;
                stH265FixQp.u32PQp = 23;
                stH265FixQp.u32BQp = 25;
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265FixQp, &stH265FixQp, sizeof(VENC_ATTR_H265_FIXQP_S));
            }
            else if (SAMPLE_RC_VBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265Vbr.u32StatTime = 1;
                stH265Vbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265Vbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265Vbr.u32MinQp  = 10;
                stH265Vbr.u32MinIQp = 10;
                stH265Vbr.u32MaxQp  = 40;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH265Vbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH265Vbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
                        stH265Vbr.u32MaxBitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH265Vbr.u32MaxBitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH265Vbr.u32MaxBitRate = 1024 * 6;
                        break;
					case PIC_UHD4K:
                        stH265Vbr.u32MaxBitRate = 1024 * 8;
                        break;
                    default :
                        stH265Vbr.u32MaxBitRate = 1024 * 4;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH265Vbr, &stH265Vbr, sizeof(VENC_ATTR_H265_VBR_S));
            }
			else if (SAMPLE_RC_AVBR == enRcMode)
            {
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                stH265AVbr.u32Gop = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265AVbr.u32StatTime = 1;
                stH265AVbr.u32SrcFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                stH265AVbr.fr32DstFrmRate = (VIDEO_ENCODING_MODE_PAL == enNorm) ? 25 : 30;
                switch (enSize)
                {
                    case PIC_QCIF:
                        stH265AVbr.u32MaxBitRate = 256 * 3; /* average bit rate */
                        break;
                    case PIC_QVGA:    /* 320 * 240 */
                    case PIC_CIF:
                        stH265AVbr.u32MaxBitRate = 512 * 3;
                        break;
                    case PIC_D1:
                    case PIC_VGA:	   /* 640 * 480 */
                        stH265AVbr.u32MaxBitRate = 1024 * 2;
                        break;
                    case PIC_HD720:   /* 1280 * 720 */
                        stH265AVbr.u32MaxBitRate = 1024 * 3;
                        break;
                    case PIC_HD1080:  /* 1920 * 1080 */
                        stH265AVbr.u32MaxBitRate = 1024 * 4;
                        break;
					case PIC_UHD4K:
                        stH265AVbr.u32MaxBitRate = 1024 * 8;	
                        break;
                    default :
                        stH265AVbr.u32MaxBitRate = 1024 * 4;
                        break;
                }
                memcpy(&stVencChnAttr.stRcAttr.stAttrH264AVbr, &stH265AVbr, sizeof(VENC_ATTR_H265_AVBR_S));
            }			
            else
            {
                return HI_FAILURE;
            }
        }
        break;
        default:
            return HI_ERR_VENC_NOT_SUPPORT;
    }

	if(PT_MJPEG == enType || PT_JPEG == enType )
    {
            stVencChnAttr.stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
            stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;
            }
            else
            {
				memcpy(&stVencChnAttr.stGopAttr,pstGopAttr,sizeof(VENC_GOP_ATTR_S));		
            }

    s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    s32Ret = HI_MPI_VENC_StartRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Stop venc ( stream mode -- H264, MJPEG )
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_Stop(VENC_CHN VencChn)
{
    HI_S32 s32Ret;
    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return HI_FAILURE;
    }    /******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}
void PushStream(int i, PAYLOAD_TYPE_E enPayLoadType)
{
	VENC_STREAM_S stStream;
	VENC_CHN_STAT_S stStat;
	HI_S32 s32Ret;

	/*******************************************************
	step 2.1 : query how many packs in one-frame stream.
	*******************************************************/
	memset(&stStream, 0, sizeof(stStream));
	s32Ret = HI_MPI_VENC_Query(i, &stStat);
	if (HI_SUCCESS != s32Ret)
	{
		//printf("%s: HI_MPI_VENC_Query chn[%d] failed with %#x!\n", __FUNCTION__, i, s32Ret);
		return;
	}
	
	if(0 == stStat.u32CurPacks)
	{
		//SAMPLE_PRT("NOTE: (%d)Current  frame is NULL!\n", i);
		return;
	}

    /*******************************************************
	step 2.2 : malloc corresponding number of pack nodes.
	*******************************************************/
    stStream.pstPack = (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
    if (NULL == stStream.pstPack)
    {
        printf("%s: malloc stream pack failed!\n", __FUNCTION__);
        return;
    }

    /*******************************************************
	step 2.3 : call mpi to get one-frame stream
	*******************************************************/
    stStream.u32PackCount = stStat.u32CurPacks;
    s32Ret = HI_MPI_VENC_GetStream(i, &stStream, HI_TRUE);
    if (HI_SUCCESS != s32Ret)
    {
        free(stStream.pstPack);
        stStream.pstPack = NULL;
        printf("%s: HI_MPI_VENC_GetStream failed with %#x!\n", __FUNCTION__, s32Ret);
        return;
    }

    /*******************************************************
	step 2.4 : save frame
	*******************************************************/
    s32Ret = HI3531D_COMM_VENC_SaveStream(i, enPayLoadType, &stStream);
    if (HI_SUCCESS != s32Ret)
    {
        free(stStream.pstPack);
        stStream.pstPack = NULL;
        printf("%s: save stream failed!\n", __FUNCTION__);
        return;
    }
    /*******************************************************
	step 2.5 : release stream
	*******************************************************/
    s32Ret = HI_MPI_VENC_ReleaseStream(i, &stStream);
    if (HI_SUCCESS != s32Ret)
    {
        free(stStream.pstPack);
        stStream.pstPack = NULL;
        return;
    }
    /*******************************************************
	step 2.6 : free pack nodes
	*******************************************************/
    free(stStream.pstPack);
    stStream.pstPack = NULL;
}

const char* HI3531A_AUDIO_Pt2Str(PAYLOAD_TYPE_E enType)
{
    if (PT_G711A == enType)  return "g711a";
    else if (PT_G711U == enType)  return "g711u";
    else if (PT_ADPCMA == enType)  return "adpcm";
    else if (PT_G726 == enType)  return "g726";
    else if (PT_LPCM == enType)  return "pcm";
    else return "data";
}

/******************************************************************************
* funciton : Start snap
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S* pstSize)
{
    HI_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_ATTR_JPEG_S stJpegAttr;
    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVeAttr.enType = PT_JPEG;
    stJpegAttr.u32MaxPicWidth  = pstSize->u32Width;
    stJpegAttr.u32MaxPicHeight = pstSize->u32Height;
    stJpegAttr.u32PicWidth  = pstSize->u32Width;
    stJpegAttr.u32PicHeight = pstSize->u32Height;
    stJpegAttr.u32BufSize = pstSize->u32Width * pstSize->u32Height * 2;
    stJpegAttr.bByFrame = HI_TRUE;/*get stream mode is field mode  or frame mode*/
    stJpegAttr.bSupportDCF = HI_FALSE;
    memcpy(&stVencChnAttr.stVeAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
    s32Ret = HI_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_CreateChn [%d] faild with %#x!\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Stop snap
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SnapStop(VENC_CHN VencChn)
{
    HI_S32 s32Ret;
    s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return HI_FAILURE;
    }
    s32Ret = HI_MPI_VENC_DestroyChn(VencChn);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/******************************************************************************
* funciton : snap process
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_SnapProcess(VENC_CHN VencChn)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 s32VencFd;
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S stRecvParam;
	VENC_STREAM_BUF_INFO_S stStreamBufInfo;
		
#if 0 /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/
    printf("press the Enter key to snap one pic\n");
    getchar();
#endif /*--NO MODIFY : COMMENT BY CODINGPARTNER--*/

    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = 1;
    s32Ret = HI_MPI_VENC_StartRecvPicEx(VencChn, &stRecvParam);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("HI_MPI_VENC_StartRecvPic faild with%#x!\n", s32Ret);
        return HI_FAILURE;
    }    
    /******************************************
     step 3:  recv picture
    ******************************************/
    s32VencFd = HI_MPI_VENC_GetFd(VencChn);
    if (s32VencFd < 0)
    {
        SAMPLE_PRT("HI_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return HI_FAILURE;
    }
    FD_ZERO(&read_fds);
    FD_SET(s32VencFd, &read_fds);
    TimeoutVal.tv_sec  = 2;
    TimeoutVal.tv_usec = 0;
    s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if (s32Ret < 0)
    {
        SAMPLE_PRT("snap select failed!\n");
        return HI_FAILURE;
    }
    else if (0 == s32Ret)
    {
        SAMPLE_PRT("snap time out!\n");
        return HI_FAILURE;
    }
    else
    {
        if (FD_ISSET(s32VencFd, &read_fds))
        {
            s32Ret = HI_MPI_VENC_Query(VencChn, &stStat);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("HI_MPI_VENC_Query failed with %#x!\n", s32Ret);
                return HI_FAILURE;
            }						
            /*******************************************************
			suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
			 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
			 {				SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
				return HI_SUCCESS;
			 }			
			 *******************************************************/
            if (0 == stStat.u32CurPacks)
            {
                SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                return HI_SUCCESS;
            }
            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
            if (NULL == stStream.pstPack)
            {
                SAMPLE_PRT("malloc memory failed!\n");
                return HI_FAILURE;
            }
            stStream.u32PackCount = stStat.u32CurPacks;
            s32Ret = HI_MPI_VENC_GetStream(VencChn, &stStream, -1);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;
            }
	        s32Ret = HI_MPI_VENC_GetStreamBufInfo (VencChn, &stStreamBufInfo);
	        if (HI_SUCCESS != s32Ret)
	        {
	            SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
	            return HI_FAILURE;
	        }
            s32Ret = HI3531D_COMM_VENC_SaveSnap(&stStreamBufInfo,&stStream);
            if (HI_SUCCESS != s32Ret)
            {
                SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;
            }
            s32Ret = HI_MPI_VENC_ReleaseStream(VencChn, &stStream);
            if (s32Ret)
            {
                SAMPLE_PRT("HI_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);
                free(stStream.pstPack);
                stStream.pstPack = NULL;
                return HI_FAILURE;
            }
            free(stStream.pstPack);
            stStream.pstPack = NULL;
        }
    }    
    /******************************************
     step 4:  stop recv picture
    ******************************************/
    s32Ret = HI_MPI_VENC_StopRecvPic(VencChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VENC_StopRecvPic failed with %#x!\n",  s32Ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/*************************/

/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
HI_VOID* HI3531D_COMM_VENC_GetVencStreamProc(HI_VOID* p)
{
	HI_S32 i;
	HI_S32 s32VideoChnTotal;
	HI_S32 s32AudioChnTotal;
	VENC_CHN_ATTR_S stVencChnAttr;
	SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
	HI_S32 maxfd = 0;
	struct timeval TimeoutVal;
	fd_set read_fds;
	HI_S32 VencFd[VENC_MAX_CHN_NUM];
	HI_S32 AencFd[AENC_MAX_CHN_NUM];
	char szFilePostfix[10];
	VENC_CHN_STAT_S stStat;
	VENC_STREAM_S stStream;
	HI_S32 s32Ret;
	VENC_CHN VencChn;
	AENC_CHN AeChn;
	PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
	VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];

	prctl(PR_SET_NAME, "hi_GetStream", 0, 0, 0);
	
	s32AudioChnTotal = 8;
	s32VideoChnTotal = 2;

	/******************************************
	step 1:  check & prepare save-file & venc-fd
	******************************************/
	if (s32VideoChnTotal >= VENC_MAX_CHN_NUM)
	{
		SAMPLE_PRT("input count invaild\n");
	//	   return HI_FAILURE;
	}	
	if (s32AudioChnTotal >= AENC_MAX_CHN_NUM)
	{
		SAMPLE_PRT("input count invaild\n");
	//		  return HI_FAILURE;
	}
	
	while(1){
		while(g_mpp_reset == 1){
			usleep(1000);
		}
		for (i = 0; i < s32VideoChnTotal; i++)
		{
			/* decide the stream file name, and open file to save stream */
			VencChn =  i;

			s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
			if (s32Ret != HI_SUCCESS)
			{
				SAMPLE_PRT("HI_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n",
				VencChn, s32Ret);
				continue;
			}
			enPayLoadType[i] = stVencChnAttr.stVeAttr.enType;
			s32Ret = HI3531D_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
			if (s32Ret != HI_SUCCESS)
			{
				SAMPLE_PRT("SAMPLE_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n",
				stVencChnAttr.stVeAttr.enType, s32Ret);
				//return HI_FAILURE;
			}

			/* Set Venc Fd. */
			VencFd[i] = HI_MPI_VENC_GetFd(VencChn);
			if (VencFd[i] < 0)
			{
				SAMPLE_PRT("HI_MPI_VENC_GetFd(%d) failed with %#x!\n",
				VencChn,VencFd[i]);
				//continue;
			}
			if (maxfd <= VencFd[i])
			{
				maxfd = VencFd[i];
			}
			s32Ret = HI_MPI_VENC_GetStreamBufInfo (i, &stStreamBufInfo[i]);
			if (HI_SUCCESS != s32Ret)
			{
				SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
			//	return (void *)HI_FAILURE;
			}
		}

		for (i = 0; i < s32AudioChnTotal; i++)
		{
			/* decide the stream file name, and open file to save stream */
			AeChn = i;
			AencFd[i] = HI_MPI_AENC_GetFd(AeChn);
			if (AencFd[i] < 0)
			{
				SAMPLE_PRT("HI_MPI_AENC_GetFd(%d) failed with %#x!\n",
				AencFd,AencFd[i]);
				continue;
			}
			if (maxfd <= AencFd[i])
			{
				maxfd = AencFd[i];
			}
		}

		/******************************************
		 step 2:  Start to get streams of each channel.
		******************************************/
		int enc_run_do = 1;

		for(int i = 0; i < CHANEL_NUM; i++){
			enc_run_do &=g_enc_reset_end[i];
		}
		g_enc_run = enc_run_do;
		printf("========== HI3531D_COMM_VENC_GetVencStreamProc  start  g_enc_run  = %d!!! \n",g_enc_run);
		while (g_enc_run)
		{			
			FD_ZERO(&read_fds);
			for (i = 0; i < s32VideoChnTotal; i++)
			{
				
				FD_SET(VencFd[i], &read_fds);
			}
			for (i = 0; i < s32AudioChnTotal; i++)
			{
				FD_SET(AencFd[i], &read_fds);
			}
			TimeoutVal.tv_sec = 2;
			TimeoutVal.tv_usec = 0;
			s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
			if (s32Ret < 0)
			{
				SAMPLE_PRT("select failed!\n");
				break;
			}
			else if (s32Ret == 0)
			{
				SAMPLE_PRT("get venc stream time out, exit thread\n");
				continue;
			}
			else
			{
				for(i = 0; i < s32AudioChnTotal; ++i)
				{
					if((g_enc_reset_end[i/2] == 0) && (g_enc_reset[i/2] == 1))
						continue;
					if (FD_ISSET(AencFd[i], &read_fds))
					{
						//帧码流类型结构体
						AUDIO_STREAM_S stStream;

						AENC_CHN AeChn =i;

						s32Ret = HI_MPI_AENC_GetStream(AeChn, &stStream, HI_FALSE);
						if (HI_SUCCESS != s32Ret )
						{
							printf("%s: HI_MPI_AENC_GetStream(%d), failed with %#x!\n", __FUNCTION__, AeChn, s32Ret);
							continue;
						}

						if( i == 6)
						{
							if(DeplyPar::Instance()->AudioIntSign){
								pthread_mutex_lock(DeplyPar::Instance()->GetAudioInLock());
								string buf;
								buf.assign((char*)(char*)stStream.pStream,stStream.u32Len);
								DeplyPar::Instance()->AudioImportdata += buf;
								//printf("-------------------audio chn= %d,len = %d\n",i,stStream.u32Len);
								pthread_mutex_unlock(DeplyPar::Instance()->GetAudioInLock());	
							}
							
						}
						
						if(AeChn == 0||AeChn==1)//hdmi 2
						;
						else if(AeChn==2||AeChn==3)//hdmi1
						;
						s32Ret = HI_MPI_AENC_ReleaseStream(AeChn, &stStream);
						if(HI_SUCCESS != s32Ret)
						{
							printf("%s: HI_MPI_AENC_ReleaseStream(%d), failed with %#x!\n",__FUNCTION__, AeChn, s32Ret);
							continue;
						}					
					}
				}
				
				for (i = 0; i < s32VideoChnTotal; i++)
				{
					
					if((g_enc_reset_end[i/2] == 0) && (g_enc_reset[i/2] == 1))
						continue;
					if (FD_ISSET(VencFd[i], &read_fds))
					{
						PushStream(i, enPayLoadType[i]);
					}
				}
			}					
		}
		usleep(10000);
	}
	
}
/******************************************************************************
* funciton : get svc_t stream from h264 channels and save them
******************************************************************************/
HI_VOID* HI3531D_COMM_VENC_GetVencStreamProc_Svc_t(void* p)
{
    HI_S32 i = 0;
    HI_S32 s32Cnt = 0;
    HI_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    SAMPLE_VENC_GETSTREAM_PARA_S* pstPara;
    HI_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    HI_S32 VencFd[VENC_MAX_CHN_NUM];
    HI_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE* pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STAT_S stStat;
    VENC_STREAM_S stStream;
    HI_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
    VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];

    prctl(PR_SET_NAME, "hi_Proc_Svc_t", 0, 0, 0);

    pstPara = (SAMPLE_VENC_GETSTREAM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;
	printf("this is HI3531D_COMM_VENC_GetVencStreamProc_Svc_t\n");
    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        SAMPLE_PRT("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = i;
        s32Ret = HI_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVeAttr.enType;
        s32Ret = HI3531D_COMM_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI3531D_COMM_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                       stVencChnAttr.stVeAttr.enType, s32Ret);
            return NULL;
        }
        for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
        {
            snprintf(aszFileName[i + s32Cnt], FILE_NAME_LEN, "Tid%d%s", i + s32Cnt, szFilePostfix);
            pFile[i + s32Cnt] = fopen(aszFileName[i + s32Cnt], "wb");

            if (!pFile[i + s32Cnt])
            {
                SAMPLE_PRT("open file[%s] failed!\n",
                           aszFileName[i + s32Cnt]);
                return NULL;
            }
        }        /* Set Venc Fd. */
        VencFd[i] = HI_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
		s32Ret = HI_MPI_VENC_GetStreamBufInfo (i, &stStreamBufInfo[i]);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
            return NULL;
        }
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (HI_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }
        TimeoutVal.tv_sec  = 2;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if (s32Ret < 0)
        {
            SAMPLE_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            SAMPLE_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (FD_ISSET(VencFd[i], &read_fds))
                {
                    /*******************************************************
                    step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    memset(&stStream, 0, sizeof(stStream));
                    s32Ret = HI_MPI_VENC_Query(i, &stStat);
                    if (HI_SUCCESS != s32Ret)
                    {
                        SAMPLE_PRT("HI_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                        break;
                    }										
                    /*******************************************************
					step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
					 if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
					 {						SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
						continue;
					 }					
					 *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        SAMPLE_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                    }                    
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        SAMPLE_PRT("malloc stream pack failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = HI_MPI_VENC_GetStream(i, &stStream, HI_TRUE);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("HI_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }  
			printf("this is HI3531D_COMM_VENC_GetVencStreamProc_Svc_t\n");                  
                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
#if 1
                    for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
                    {
                        switch (s32Cnt)
                        {
                            case 0:
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType)
                                {  

       //                             s32Ret = HI3531D_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i+s32Cnt], &stStream);
                               

                                }
                                break;
                            case 1:
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType      ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYENHANCE == stStream.stH264Info.enRefType)
                                {
//                                    s32Ret = HI3531D_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i+s32Cnt], &stStream);
                                }
                                break;
                            case 2:
  
 //                               s32Ret = HI3531D_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i+s32Cnt], &stStream);

                                break;
                        }
                        if (HI_SUCCESS != s32Ret)
                        {
                            free(stStream.pstPack);
                            stStream.pstPack = NULL;
                            SAMPLE_PRT("save stream failed!\n");
                            break;
                        }
                    }
#else
                    for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
                    {
                        if (s32Cnt == 0)
                        {
                            if (NULL != pFile[i + s32Cnt])
                            {
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType)
                                {
                                   // s32Ret = HI3531D_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i + s32Cnt], &stStream);
                                }
                            }
                        }
                        else if (s32Cnt == 1)
                        {
                            if (NULL != pFile[i + s32Cnt])
                            {
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType         ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYENHANCE == stStream.stH264Info.enRefType)
                                {
                                    //s32Ret = HI3531D_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i + s32Cnt], &stStream);
                                }
                            }
                        }
                        else
                        {
                            if (NULL != pFile[i + s32Cnt])
                            {
				printf("this is h264 channels!!!\n");
                                //s32Ret = HI3531D_COMM_VENC_SaveStream(enPayLoadType[i], pFile[i + s32Cnt], &stStream);
                            }
                        }
                    }
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        SAMPLE_PRT("save stream failed!\n");
                        break;
                    }
#endif
                    /*******************************************************
                    step 2.6 : release stream
                    *******************************************************/
                    s32Ret = HI_MPI_VENC_ReleaseStream(i, &stStream);
                    if (HI_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }                    
                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                }
            }
        }
    }    
    /*******************************************************
     step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
        {
            if (pFile[i + s32Cnt])
            {
                fclose(pFile[i + s32Cnt]);
            }
        }
    }
    return NULL;
}

HI_S32 HI3531D_COMM_VENC_StartGetStream(HI_S32 s32Cnt)
{
    gs_stPara.bThreadStart = HI_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    return pthread_create(&gs_VencPid, 0, HI3531D_COMM_VENC_GetVencStreamProc, (HI_VOID*)&gs_stPara);
}


/******************************************************************************
* funciton : start get venc svc-t stream process thread
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_StartGetStream_Svc_t(HI_S32 s32Cnt)
{
    gs_stPara.bThreadStart = HI_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    return pthread_create(&gs_VencPid, 0, HI3531D_COMM_VENC_GetVencStreamProc_Svc_t, (HI_VOID*)&gs_stPara);
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
HI_S32 HI3531D_COMM_VENC_StopGetStream()
{
    if (HI_TRUE == gs_stPara.bThreadStart)
    {
        gs_stPara.bThreadStart = HI_FALSE;
        if (gs_VencPid)
        {
            pthread_join(gs_VencPid, 0);
            gs_VencPid = 0;
        }
    }
    return HI_SUCCESS;
}



HI_VOID HI3531D_COMM_VENC_ReadOneFrame( FILE* fp, HI_U8* pY, HI_U8* pU, HI_U8* pV,
                                       HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2)
{
    HI_U8* pDst;
    HI_U32 u32Row;
    pDst = pY;
    for ( u32Row = 0; u32Row < height; u32Row++ )
    {
        if (fread( pDst, width, 1, fp ) != 1)
		{
        	printf("fread failed\n");
        	return;
    	}
        pDst += stride;
    }
    pDst = pU;
    for ( u32Row = 0; u32Row < height / 2; u32Row++ )
    {
        if (fread( pDst, width / 2, 1, fp ) != 1)
		{
        	printf("fread failed\n");
        	return;
    	}
        pDst += stride2;
    }
    pDst = pV;
    for ( u32Row = 0; u32Row < height / 2; u32Row++ )
    {
        if (fread( pDst, width / 2, 1, fp ) != 1)
		{
        	printf("fread failed\n");
        	return;
    	}
        pDst += stride2;
    }
}

HI_S32 HI3531D_COMM_VENC_PlanToSemi(HI_U8* pY, HI_S32 yStride,
                                   HI_U8* pU, HI_S32 uStride,                                   HI_U8* pV, HI_S32 vStride,
                                   HI_S32 picWidth, HI_S32 picHeight)
{
    HI_S32 i;
    HI_U8* pTmpU, *ptu;
    HI_U8* pTmpV, *ptv;
    HI_S32 s32HafW = uStride >> 1 ;
    HI_S32 s32HafH = picHeight >> 1 ;
    HI_S32 s32Size = s32HafW * s32HafH;
    pTmpU = (HI_U8 *)malloc( s32Size );
    ptu = pTmpU;
    pTmpV = (HI_U8 *)malloc( s32Size );
    ptv = pTmpV;
    if ((pTmpU == HI_NULL) || (pTmpV == HI_NULL))
    {
        printf("malloc buf failed\n");
		if (pTmpU != HI_NULL)
        {
            free( pTmpU );
        }

		if (pTmpV != HI_NULL)
        {
            free( pTmpV );
        }
        return HI_FAILURE;
    }
    memcpy(pTmpU, pU, s32Size);
    memcpy(pTmpV, pV, s32Size);
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;
    }
    for (i = 0; i<s32Size >> 1; i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }
    free( ptu );
    free( ptv );
    return HI_SUCCESS;
}

