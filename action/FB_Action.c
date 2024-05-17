#include "FB_Action.h"
#include "NavObj.h"
#include "Action.h"
#include "math.h"
#include "stdlib.h"
#include "Bsp_Log.h"
#include "Bsp_Task.h"
#include "SysParamManage.h"

#define RAD2ANGLE       57.29578f
#define CLOCK_T            -1       // 顺时针
#define ANTICLOCK_T         1       // 逆时针
#define UNCERTAIN_T         2

RotateFollowUp_Str RotateAct;
turnTableMotionSpd_t TurnTableMotionSpd;
turnTableMotionPos_t TurnTableMotionPos;
/*
转盘随动功能块，根据激光定位信息和二维码定位信息，计算转盘的绝对角度，使其始终保持和地面角度不变
*/
float record[10];
uint32_t record_j = 0;
uint32_t g_record_i;
#define FILTER_K 0.3/10
#if 0
void TurntableTrackingExt(void *P0)
{ 
	xNetSendEnable.AdjustPoseEn = 1;
	float palletw=0;
    float Reliability,LaserAngle;
    float TagAngle;
	float TmpAng;
	uint8_t TransMode=2;//0 激光 1 二维码 2 无效 5 手动
	uint8_t TransCMD=2;//0 关闭 1 启动
    uint32_t TagID;
	float KalLaserAng;
	RotateFollowUp_Str *pRotate=(RotateFollowUp_Str *)P0;
	PalletAngCal(pRotate);
	LaserAngle = GetLaserPosInfo.theta;
    Reliability = GetLaserPosInfo.relibality;
    TagID = gTagNaviInfo.ID;  
	float rad; 
    //pRotate->InGyroG =pRotate->GyroG/1000;
  
	pRotate->InGyroGFilter = pRotate->InGyroGFilter + (GetVehicleStatues.fw-pRotate->InGyroGFilter)*FILTER_K;
/* 	
	if(TagID == 0 || fabs(GetVehicleStatues.fw)<0.0000001)
	{
		pRotate->reset_qr = 1; 
	}

	if(pRotate->reset_qr==1 && TagID!=0 )
	{
		mw_kalman.Fil_Init(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.RealZ),AngToRad(gTagNaviInfo.Yaw),0.05);
		TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
		pRotate->reset_qr=0; 
	} 
*/	
	if(TagID!=0)
	{
		if(gTagNaviInfo.angChange == 1)//tt 二维码信息发生变化
		{ 
			mw_kalman.GetFilQR(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.RealZ),AngToRad(gTagNaviInfo.Yaw));
			TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
			gTagNaviInfo.angChange = 0; 
		}
		else  
		{ 
			TagAngle = mw_kalman.GetFil_QR_GYRO(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.Yaw));
			TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
		}	
	}else
	{ 
		TagAngle = mw_kalman.GetFil_QR_GYRO(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.Yaw));  
		TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
	} 
#if 0
	/***********************************************************************/
	record[0] = gTagNaviInfo.Yaw;
	record[1] = gTagNaviInfo.RealZ;
	record[2] = LaserAngleDeal(LaserAngle);
	record[3] = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
	record[4] = LaserAngleDeal(gTagNaviInfo.LASER1DIM.fil_now);//pRotate->TurnSpd + pRotate->InGyroGFilter; 
	record[5] = GetVehicleStatues.fw;//pRotate->InGyroG;
	record[6] = pRotate->InGyroGFilter;
	record[7] = pRotate->InGyroG;  
	record[8] = pRotate->RotateW;
	record[9] = pRotate->Bias; 

	g_record_i++;
	if(g_record_i%20 == 0)
	{ 
		//DeltaAng = pRotate->palletangsum - pRotate->palletangsumLast;
		//pRotate->palletangsumLast = pRotate->palletangsum;
		//pRotate->InTurnFilter = AngToRad(pRotate->InTurnFilter + (DeltaAng*50 - pRotate->InTurnFilter)*0.3);

		gLogMgEvent.WriteTo("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",record[0],record[1],record[2],record[3],record[4],record[5],record[6],record[7],record[8],record[9]);		
		record_j++;
		record_j%=10;
	}
#endif
	//TagAngle = gTagNaviInfo.TurnTable_Z;
	
    if(pRotate->Mode==2)
		TransMode = 1;
	if(pRotate->Mode==3)
		TransMode = 0;
	if(TransMode==2)
		TransCMD = 0;
	else
		TransCMD = pRotate->Cmd;
	if(pRotate->precmd!=TransCMD)
	{
        pRotate->precmd=TransCMD;
		if(pRotate->Cmd==1)//init  stop->start
		{
			//mode is not changed.
			pRotate->premode = TransMode;
		}
		else//close  check if bias need to be dealed
		{
			pRotate->bylaserready=0;
			pRotate->byqrready=0;
			pRotate->isum=0;
			pRotate->prebias=0;
			pRotate->restbias = 0;
			if((fabs(pRotate->Bias)>0.2)&&(fabs(pRotate->Bias)<10))
			{
				pRotate->closeadjen = 1;
				pRotate->closerestbias=pRotate->Bias;
				pRotate->closepalletang = pRotate->palletangsum;
			}
			else
			{
				pRotate->closeadjen = 0;
				if(fabs(pRotate->Bias)>10)
				{
					pRotate->state = 4;
				}
                else
                    pRotate->state = 0;
			}
		}
	}
	
	if(TransCMD==1)
	{
		if(pRotate->premode != TransMode)//mod switch
		{
			//mode changed. need to deal the premode bias.
			if(TransMode==0)//qr switch to laser
			{
				if((pRotate->byqrready==1)&&(pRotate->bylaserready==0))
					pRotate->restbias=pRotate->qrbias;
				else
					pRotate->restbias=0;
			}
			if(TransMode==1)//laser switch to qr
			{
				if((pRotate->byqrready==0)&&(pRotate->bylaserready==1))
					pRotate->restbias=pRotate->laserbias;
				else
					pRotate->restbias=0;
			}
			pRotate->premode = TransMode;
			
			//pRotate->bylaserready=0;
			//pRotate->byqrready=0;
			gLogMgEvent.WriteToLog(334,2,"ResetBias=%d,%f",pRotate->Mode,pRotate->restbias);
		}
		else
		{
			//do nothing
		}
		if((pRotate->bylaserready==0)&&(TransMode==0))//laser mode
		{
			if(Reliability>0.5)
			{
				pRotate->state=1;
				pRotate->bylaserready=1; 
				KalLaserAng = LaserRadDeal(LaserAngle);
				mw_kalman.Fil_Init(&(gTagNaviInfo.LASER1DIM),KalLaserAng,AngToRad(gTagNaviInfo.Yaw),0.05);
				pRotate->lasersetangle = LaserAngleDeal(LaserAngle);//SetAngleCal(LaserAngleDeal(LaserAngle),pRotate->AbsPalletAng);
				pRotate->laserpalletsetangle = pRotate->AbsPalletAng-pRotate->restbias;
				if(pRotate->laserpalletsetangle<0) pRotate->laserpalletsetangle= pRotate->laserpalletsetangle +360;
				if(pRotate->laserpalletsetangle>360) pRotate->laserpalletsetangle= pRotate->laserpalletsetangle -360;
				gLogMgEvent.WriteToLog(335,3,"L_Set=%f,%f,%f",LaserAngle,pRotate->palletangsum,pRotate->restbias);
			}
			else
			{
				pRotate->state=2;
				palletw = -GetVehicleStatues.fw;
				//set warning state
			}
		}
		if((pRotate->byqrready==0)&&(TransMode==1))//qrmode
		{
			if(TagID!=0)
			{
				//if(CheckQRAngisCorrect(TagAngle))
				{
					mw_kalman.Fil_Init(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.RealZ),AngToRad(gTagNaviInfo.Yaw),0.05);
					TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
					pRotate->state=1;
					pRotate->byqrready=1;
					pRotate->qrsetangle = TagAngle;//SetAngleCal(TagAngle,pRotate->AbsPalletAng);
					pRotate->qrpalletsetangle = pRotate->AbsPalletAng-pRotate->restbias;
					if(pRotate->qrpalletsetangle<0) pRotate->qrpalletsetangle= pRotate->qrpalletsetangle +360;
					if(pRotate->qrpalletsetangle>360) pRotate->qrpalletsetangle= pRotate->qrpalletsetangle -360;
					gLogMgEvent.WriteToLog(489,3,"QR_Set=%f,%f,%f",TagAngle,pRotate->palletangsum,pRotate->restbias);
				}
			}
			else
			{
				pRotate->state=2;
				palletw = -GetVehicleStatues.fw;
				pRotate->reset_qr = 1;
				//set warning state
			}
		}
		if((pRotate->bylaserready==1)&&(TransMode==0))//laser mode running
		{
			if(Reliability>0.5)
			{
				pRotate->state=1;
				KalLaserAng = LaserRadDeal(LaserAngle);
				if(gTagNaviInfo.laserChange == 1)
				{
					gTagNaviInfo.laserChange=0;
					mw_kalman.GetFilQR(&(gTagNaviInfo.LASER1DIM),KalLaserAng,AngToRad(gTagNaviInfo.Yaw));
					TmpAng = LaserAngleDeal(gTagNaviInfo.LASER1DIM.fil_now);	
			 
				}else
				{
					mw_kalman.GetFil_QR_GYRO(&(gTagNaviInfo.LASER1DIM),AngToRad(gTagNaviInfo.Yaw)); 
					TmpAng = LaserAngleDeal(gTagNaviInfo.LASER1DIM.fil_now);
				} 
				 
				
				pRotate->laserbias = CalPalletBias(TmpAng,pRotate->lasersetangle,pRotate->laserpalletsetangle,pRotate->AbsPalletAng,pRotate->restbias);
				//cal bias
				pRotate->Bias = pRotate->laserbias;
				pRotate->CarAng = TmpAng;
				palletw = BiasPIDCal(pRotate)-pRotate->InGyroGFilter;//+(pRotate->TurnSpd + pRotate->InGyroGFilter) *pRotate->D;//GetVehicleStatues.fw;
			}
			else
			{
				pRotate->state=2;
				palletw = -GetVehicleStatues.fw;
				//set warning state
			}
			//get pallet w
			
		}
		if((pRotate->byqrready==1)&&(TransMode==1))//qr mode running
		{
			if(TagID!=0)
			{
				pRotate->state=1;
				pRotate->qrbias = CalPalletBias(TagAngle,pRotate->qrsetangle,pRotate->qrpalletsetangle,pRotate->AbsPalletAng,pRotate->restbias);
				pRotate->Bias = pRotate->qrbias;
				pRotate->CarAng = TagAngle;//角度
				//cal bias
				palletw = BiasPIDCal(pRotate)-pRotate->InGyroGFilter;//-GetVehicleStatues.fw;
				//palletw = BiasPIDCal(pRotate)-pRotate->InGyroGFilter;//-(pRotate->TurnSpd + pRotate->InGyroGFilter) *pRotate->D;//-GetVehicleStatues.fw;
			}
			else
			{
				pRotate->reset_qr=1;
				pRotate->state=2;

                pRotate->qrbias = CalPalletBias(TagAngle,pRotate->qrsetangle,pRotate->qrpalletsetangle,pRotate->AbsPalletAng,pRotate->restbias);
				pRotate->Bias = pRotate->qrbias;
				pRotate->CarAng = TagAngle;//角度
				//cal bias
				palletw = BiasPIDCal(pRotate)-pRotate->InGyroGFilter;//-GetVehicleStatues.fw;
				//set warning state
			}
			//get pallet w
		}
		if(fabs(palletw)>pRotate->MaxPalletW)
		{
			pRotate->state=3;
			palletw = palletw/fabs(palletw)*pRotate->MaxPalletW;
		}
        //pRotate->RotateW = pRotate->RotateW+(palletw-pRotate->RotateW)*FILTER_K;
		pRotate->RotateW = palletw;
		//GetVehicleStatues.fw;// car's velocity
		
	}
	else
	{
		if(pRotate->closeadjen)
		{
			pRotate->state = 5;
			pRotate->Bias = pRotate->closerestbias-(pRotate->palletangsum-pRotate->closepalletang);
			palletw = BiasPIDCal(pRotate);
			pRotate->RotateW = palletw;
			//pRotate->RotateW = pRotate->RotateW+(palletw-pRotate->RotateW)*FILTER_K;
			if(fabs(pRotate->Bias)<0.2)
			{
				pRotate->restbias = pRotate->restbias+pRotate->Bias;
				pRotate->closeadjen = 0;  
                pRotate->state = 0;
			}
		}
        else
          pRotate->RotateW = 0;
	}
  
}
#else 
extern xNetSendEnable_Str xNetSendEnable;

void TurntableTrackingExt(void *P0)
{ 
	xNetSendEnable.AdjustPoseEn = 1;
	RotateFollowUp_Str *pRotate=(RotateFollowUp_Str *)P0;
		
	RotateAct.Mode = pRotate->Mode;
	RotateAct.Encoder =  pRotate->Encoder;
	RotateAct.EncoderPerCyc = pRotate->EncoderPerCyc;
	RotateAct.MaxPalletW = pRotate->MaxPalletW;
	RotateAct.P = pRotate->P;
	RotateAct.I = pRotate->I;
	RotateAct.D = pRotate->D;
 
	pRotate->state = RotateAct.state; 
	pRotate->RotateW = RotateAct.RotateW;
	pRotate->PalletAngSum = RotateAct.PalletAngSum;
	pRotate->AbsPalletAng = RotateAct.AbsPalletAng;
	pRotate->CarAng = RotateAct.CarAng;
	pRotate->Bias = RotateAct.Bias;

	if(pRotate->lasetCmd != pRotate->Cmd)
	{
		RotateAct.Cmd = pRotate->Cmd;
	}
	pRotate->lasetCmd = pRotate->Cmd;
}
#endif


void DoTableFollowTask(void)
{
    /*
    转盘随动功能块，根据激光定位信息和二维码定位信息，计算转盘的绝对角度，使其始终保持和地面角度不变
    */ 
	float palletw=0;
    float Reliability,LaserAngle;
    float TagAngle;
	float TmpAng;
	uint8_t TransMode=2;//0 激光 1 二维码 2 无效 5 手动
	uint8_t TransCMD=2;//0 关闭 1 启动
    uint32_t TagID;
	float KalLaserAng;
	RotateFollowUp_Str *pRotate=(RotateFollowUp_Str *)&RotateAct;

	PalletAngCal(pRotate);
	LaserAngle = GetLaserPosInfo.theta;
    Reliability = GetLaserPosInfo.relibality;
    TagID = gTagNaviInfo.ID;  
	float rad; 
    
    //任务状态判断 //状态：0-空闲1-运行2-等待数据3-已达转盘最大速度4-误差过大取消 5-等待完成校准停止中

    if (pRotate->Cmd == 1) // 启动
    {
        if (pRotate->state == 1 || pRotate->state == 3)
        {
            pRotate->actStatus = 0;
        }
        else if (pRotate->state == 2 || pRotate->state == 4)
        {
            pRotate->actStatus = -1;
        }
        else if (pRotate->state == 1)
        {
            pRotate->actStatus = 1;
        }
    }
    else
    {
        if (pRotate->state == 0)
        {
            pRotate->actStatus = 0;
        }
        else if (pRotate->state == 2 || pRotate->state == 4)
        {
            pRotate->actStatus = -1;
        }
        else if (pRotate->state == 1 || pRotate->state == 5)
        {
            pRotate->actStatus = 1;
        }
    }
    //pRotate->InGyroG =pRotate->GyroG/1000;
  
	pRotate->InGyroGFilter = pRotate->InGyroGFilter + (GetVehicleStatues.fw-pRotate->InGyroGFilter)*FILTER_K;

	if(TagID!=0)
	{
		if(gTagNaviInfo.angChange == 1)//tt 二维码信息发生变化
		{ 
			mw_kalman.GetFilQR(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.RealZ),AngToRad(gTagNaviInfo.Yaw));
			TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
			gTagNaviInfo.angChange = 0; 
		}
		else  
		{ 
			TagAngle = mw_kalman.GetFil_QR_GYRO(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.Yaw));
			TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
		}	
	}else
	{ 
		TagAngle = mw_kalman.GetFil_QR_GYRO(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.Yaw));  
		TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
	} 

	//TagAngle = gTagNaviInfo.TurnTable_Z;
	
    if(pRotate->Mode==2)
		TransMode = 1;
	if(pRotate->Mode==3)
		TransMode = 0;
	if(TransMode==2)
		TransCMD = 0;
	else
		TransCMD = pRotate->Cmd;
	if(pRotate->precmd!=TransCMD)
	{
        pRotate->precmd=TransCMD;
		if(pRotate->Cmd==1)//init  stop->start
		{
			//mode is not changed.
			pRotate->premode = TransMode;
		}
		else//close  check if bias need to be dealed
		{
			pRotate->bylaserready=0;
			pRotate->byqrready=0;
			pRotate->isum=0;
			pRotate->prebias=0;
			pRotate->restbias = 0;
			if((fabs(pRotate->Bias)>0.2)&&(fabs(pRotate->Bias)<10))
			{
				pRotate->closeadjen = 1;
				pRotate->closerestbias=pRotate->Bias;
				pRotate->closepalletang = pRotate->palletangsum;
			}
			else
			{
				pRotate->closeadjen = 0;
				if(fabs(pRotate->Bias)>10)
				{
					pRotate->state = 4;
				}
                else
                    pRotate->state = 0;
			}
		}
	}
	
	if(TransCMD==1)
	{
		if(pRotate->premode != TransMode)//mod switch
		{
			//mode changed. need to deal the premode bias.
			if(TransMode==0)//qr switch to laser
			{
				if((pRotate->byqrready==1)&&(pRotate->bylaserready==0))
					pRotate->restbias=pRotate->qrbias;
				else
					pRotate->restbias=0;
			}
			if(TransMode==1)//laser switch to qr
			{
				if((pRotate->byqrready==0)&&(pRotate->bylaserready==1))
					pRotate->restbias=pRotate->laserbias;
				else
					pRotate->restbias=0;
			}
			pRotate->premode = TransMode;
			
			//pRotate->bylaserready=0;
			//pRotate->byqrready=0;
			gLogMgEvent.WriteToLog(334,2,"ResetBias=%d,%f",pRotate->Mode,pRotate->restbias);
		}
		else
		{
			//do nothing
		}
		if((pRotate->bylaserready==0)&&(TransMode==0))//laser mode
		{
			if(Reliability>0.5)
			{
				pRotate->state=1;
				pRotate->bylaserready=1; 
				KalLaserAng = LaserRadDeal(LaserAngle);
				mw_kalman.Fil_Init(&(gTagNaviInfo.LASER1DIM),KalLaserAng,AngToRad(gTagNaviInfo.Yaw),0.05);
				pRotate->lasersetangle = LaserAngleDeal(LaserAngle);//SetAngleCal(LaserAngleDeal(LaserAngle),pRotate->AbsPalletAng);
				pRotate->laserpalletsetangle = pRotate->AbsPalletAng-pRotate->restbias;
				if(pRotate->laserpalletsetangle<0) pRotate->laserpalletsetangle= pRotate->laserpalletsetangle +360;
				if(pRotate->laserpalletsetangle>360) pRotate->laserpalletsetangle= pRotate->laserpalletsetangle -360;
				gLogMgEvent.WriteToLog(335,3,"L_Set=%f,%f,%f",LaserAngle,pRotate->palletangsum,pRotate->restbias);
			}
			else
			{
				pRotate->state=2;
				palletw = -GetVehicleStatues.fw;
				//set warning state
			}
		}
		if((pRotate->byqrready==0)&&(TransMode==1))//qrmode
		{
			if(TagID!=0)
			{
				//if(CheckQRAngisCorrect(TagAngle))
				{
					mw_kalman.Fil_Init(&(gTagNaviInfo.QR1DIM),AngToRad(gTagNaviInfo.RealZ),AngToRad(gTagNaviInfo.Yaw),0.05);
					TagAngle = LaserAngleDeal(gTagNaviInfo.QR1DIM.fil_now);
					pRotate->state=1;
					pRotate->byqrready=1;
					pRotate->qrsetangle = TagAngle;//SetAngleCal(TagAngle,pRotate->AbsPalletAng);
					pRotate->qrpalletsetangle = pRotate->AbsPalletAng-pRotate->restbias;
					if(pRotate->qrpalletsetangle<0) pRotate->qrpalletsetangle= pRotate->qrpalletsetangle +360;
					if(pRotate->qrpalletsetangle>360) pRotate->qrpalletsetangle= pRotate->qrpalletsetangle -360;
					gLogMgEvent.WriteToLog(489,3,"QR_Set=%f,%f,%f",TagAngle,pRotate->palletangsum,pRotate->restbias);
				}
			}
			else
			{
				pRotate->state=2;
				palletw = -GetVehicleStatues.fw;
				pRotate->reset_qr = 1;
				//set warning state
			}
		}
		if((pRotate->bylaserready==1)&&(TransMode==0))//laser mode running
		{
			if(Reliability>0.5)
			{
				pRotate->state=1;
				KalLaserAng = LaserRadDeal(LaserAngle);
				if(gTagNaviInfo.laserChange == 1)
				{
					gTagNaviInfo.laserChange=0;
					mw_kalman.GetFilQR(&(gTagNaviInfo.LASER1DIM),KalLaserAng,AngToRad(gTagNaviInfo.Yaw));
					TmpAng = LaserAngleDeal(gTagNaviInfo.LASER1DIM.fil_now);	
			 
				}else
				{
					mw_kalman.GetFil_QR_GYRO(&(gTagNaviInfo.LASER1DIM),AngToRad(gTagNaviInfo.Yaw)); 
					TmpAng = LaserAngleDeal(gTagNaviInfo.LASER1DIM.fil_now);
				} 
				 
				
				pRotate->laserbias = CalPalletBias(TmpAng,pRotate->lasersetangle,pRotate->laserpalletsetangle,pRotate->AbsPalletAng,pRotate->restbias);
				//cal bias
				pRotate->Bias = pRotate->laserbias;
				pRotate->CarAng = TmpAng;
				palletw = BiasPIDCal(pRotate)-pRotate->InGyroGFilter;//+(pRotate->TurnSpd + pRotate->InGyroGFilter) *pRotate->D;//GetVehicleStatues.fw;
			}
			else
			{
				pRotate->state=2;
				palletw = -GetVehicleStatues.fw;
				//set warning state
			}
			//get pallet w
			
		}
		if((pRotate->byqrready==1)&&(TransMode==1))//qr mode running
		{
			if(TagID!=0)
			{
				pRotate->state=1;
				pRotate->qrbias = CalPalletBias(TagAngle,pRotate->qrsetangle,pRotate->qrpalletsetangle,pRotate->AbsPalletAng,pRotate->restbias);
				pRotate->Bias = pRotate->qrbias;
				pRotate->CarAng = TagAngle;//角度
				//cal bias
				palletw = BiasPIDCal(pRotate)-pRotate->InGyroGFilter;//-GetVehicleStatues.fw;
				//palletw = BiasPIDCal(pRotate)-pRotate->InGyroGFilter;//-(pRotate->TurnSpd + pRotate->InGyroGFilter) *pRotate->D;//-GetVehicleStatues.fw;
			}
			else
			{
				pRotate->reset_qr=1;
				pRotate->state=2;

                pRotate->qrbias = CalPalletBias(TagAngle,pRotate->qrsetangle,pRotate->qrpalletsetangle,pRotate->AbsPalletAng,pRotate->restbias);
				pRotate->Bias = pRotate->qrbias;
				pRotate->CarAng = TagAngle;//角度
				//cal bias
				palletw = BiasPIDCal(pRotate)-pRotate->InGyroGFilter;//-GetVehicleStatues.fw;
				//set warning state
			}
			//get pallet w
		}
		if(fabs(palletw)>pRotate->MaxPalletW)
		{
			pRotate->state=3;
			palletw = palletw/fabs(palletw)*pRotate->MaxPalletW;
		}
        //pRotate->RotateW = pRotate->RotateW+(palletw-pRotate->RotateW)*FILTER_K;
		pRotate->RotateW = palletw;
		//GetVehicleStatues.fw;// car's velocity
		
	}
	else
	{
		if(pRotate->closeadjen)
		{
			pRotate->state = 5;
			pRotate->Bias = pRotate->closerestbias-(pRotate->palletangsum-pRotate->closepalletang);
			palletw = BiasPIDCal(pRotate);
			pRotate->RotateW = palletw;
			//pRotate->RotateW = pRotate->RotateW+(palletw-pRotate->RotateW)*FILTER_K;
			if(fabs(pRotate->Bias)<0.2)
			{
				pRotate->restbias = pRotate->restbias+pRotate->Bias;
				pRotate->closeadjen = 0;  
                pRotate->state = 0;
			}
		}
        else
          pRotate->RotateW = 0;
	}
    
}

/**
 * @brief 判断当前角度curAng 是否 在tarAng的区间内 (tarAng - errRange, tarAng + errRange)
 * @param curAng 当前角度
 * @param tarAng 目标角度
 * @param errRange 设定误差值
 * @return 0-不在区间 1-在允许误差区间范围之内
*/
static uint8_t isInErrorRange(float curAng,float tarAng,float errorRange)
{
    float left = tarAng + 360 - errorRange;
    float right = tarAng + errorRange;
    uint8_t mark = 0;

    if(left > 360)
    {
        left -= 360;
    }else
    {
        // 左值很大300以上
        mark = 1;
    }
    if(right > 360)
    {
        right -= 360;// 右值在小角度10内
        mark = 1;
    }

    // 正常范围
    if(curAng >= left && curAng <= right)
    {
        return 1;
    }

    // 在0/360点的界限 left-0-right
    if(mark == 1)
    {
        if(curAng >= left || curAng <= right)
        {
            return 1;
        }
    }
    return 0;
    
}

/**
 * @brief cmd动作之后，方向和速度控制状态的首次初始化
 * @note
 * @param   curAngle    当前角度
 * @param   targetAngle 目标角度
 * @param   rotorw      当前角速度
 * @param   state       模块运动状态
 * @param   dir         最小运动方向
 * @param   startAngle  起始运动角度
 * @param   startRotorW 起始运动角速度 （默认是0的话，不需要这个参数）
 * 
*/

static void updateInitState(float curAngle,float targetAngle,float rotorw,uint8_t state,int8_t *dir,float *startAngle,float *startRotorW)
{
    float curAngDiff;       // 当前角度差

    if(state == _eSET_ZERO_ST)
    {
        // 首次开机未确认零位
        *dir = ANTICLOCK_T;
        return;
    }

    // 到达指定角度
    if(state == _eRUN_ST)
    {
        if(curAngle < targetAngle)
        {
            curAngDiff = targetAngle - curAngle;

            if(*dir == UNCERTAIN_T)
            {
                if(curAngDiff > 180)
                {
                    *dir = CLOCK_T;// 顺时针
                }else
                {
                    *dir = ANTICLOCK_T;
                }
                *startAngle = curAngle;
                *startRotorW = fabs(rotorw);
            }
        }else
        {
            curAngDiff = curAngle - targetAngle;
            if(*dir == UNCERTAIN_T)
            {
                if(curAngDiff > 180)
                {
                    *dir = ANTICLOCK_T;
                }else
                {
                    *dir = CLOCK_T;// 顺时针
                }
                *startAngle = curAngle;
                *startRotorW = fabs(rotorw);
            }

        }
    }
}

/**
 * @brief 计算带有加减速功能的角速度的绝对值
 * @note
 * @param   curAngle      当前角度
 * @param   lastAngle     模块上一次的角度
 * @param   startAngle    运动开始的起始角度
 * @param   startRotorW   运动开始的角速度
 * @param   curAngDiff    与目标角度的角度差
 * @param   vang          运动的最大角速度
 * @param   accVang       角加速度
 * @param   curRotorW     当前角速度       
 * @param   zeroVang      归零的默认角速度
 * @param   tick          函数执行间隔(ms)
 * 
*/
static float calcRotorW(float curAngle,float lastAngle,float startAngle,float startRotorW,float curAngDiff,float vang,float accVang,float curRotorW,float zeroVang,float tick)
{
    static uint32_t ulcyc = 0;
    
    float rotorw = 0;

    float absOldRotorW = fabs(curRotorW); // 单位rad/s

    if(curAngDiff > 180)
    {
        curAngDiff = 360 - curAngDiff;
    }

    float decTime = absOldRotorW/accVang;
    float resrvAng = absOldRotorW * decTime / 2 *RAD2ANGLE;// 减速的角度值

    if(curAngDiff > resrvAng)
    {
        float accAng = 0;
        if(absOldRotorW < vang)
        {
            // 还未达到最高速度
            if(curAngDiff - resrvAng > 1)
            {
                if(curAngle != lastAngle)
                {
                    // 1. 走过的角度 accAng
                    accAng = fabs(curAngle - startAngle);
                    if(accAng > 180)accAng = 360 - accAng;
                    accAng = AngToRad(accAng);// 弧度值
                    rotorw = sqrt(2 * accVang * accAng + startRotorW * startRotorW) + accVang * tick;// 矫正的速度 + 加速度*时间 v1^2 = v0^2 + 2ax
                }
                else
                {
                    rotorw = absOldRotorW + accVang * tick;
                }
                if(rotorw < 0.1)
                {
                    rotorw = absOldRotorW + accVang * tick; 
                } 

            }else
            {
                rotorw = absOldRotorW;
            }
            if(rotorw > vang)rotorw = vang;
        }else
        {
            rotorw = absOldRotorW; // 匀速阶段：保持当前速度
        }
    }else
    {
        // 得减速了
        if(curAngle != lastAngle)
        {
            decTime = sqrt(2 * AngToRad(curAngDiff) / accVang);
            rotorw = accVang * decTime;// - accVang * tick;
        }else
        {
            rotorw = absOldRotorW - accVang * tick;
        }

        if(rotorw < 0.1)
        {
            rotorw = absOldRotorW - accVang * tick;
        }
        if(rotorw < 0) rotorw = 0;
    }
    if(ulcyc % 5 == 0)
    {
        gLogMgEvent.WriteTo("curAngDiff:%f ,resrvAng:%f ,curAngle:%f ,lastAngle:%f ,tick:%f ,rotorw:%f ", curAngDiff,resrvAng,curAngle,lastAngle,tick,rotorw);
    }
    ulcyc++;
    return rotorw;
}

/**
 * @brief 更新当前角度并输出目标编码器值
 * @param   encoderPerCyc   一圈的编码器数
 * @param   lastEncoder     上一次的编码值
 * @param   encoder         当前编码器值
 * @param   lastZeroEncoder 上一次过零点时的编码值
 * @param   upperEncoder    上限
 * @param   lowerEncoder    下限
 * @param   state           模块运行状态
 * @param   tarAngle        目标角度
 * @param   lastAngle       上一次的角度值
 * @param   curAngle        当前角度值
*/
static int32_t calcTargetEncoder(int encoderPerCyc,int lastEncoder,int encoder,int lastZeroEncoder,int upperEncoder,int lowerEncoder,uint8_t state,float tarAngle,float *lastAngle,float *curAngle)
{
    float precision = encoderPerCyc/360.0;
    uint8_t upCrossFlag = 0;
    uint8_t lowCrossFlag = 0;

    float absltAngle;
    int32_t outEncoder = encoder;
    
    if(abs(encoder - lastEncoder) > encoderPerCyc/2)
    {
        // 编码器值发生了越限
        if(encoder < lowerEncoder + encoderPerCyc/2)
        {
            // 编码器值递增越限
            upCrossFlag = 1;
            absltAngle = 360 - ((upperEncoder - lastZeroEncoder) + (encoder - lowerEncoder))/precision;
        }else
        {
            // 编码器值递减越限
            lowCrossFlag = 1;
            absltAngle = ((upperEncoder - encoder) + (lastZeroEncoder - lowerEncoder))/precision;
        }
    }else
    {
        // 逆时针方向，角度变大，编码值减少
        if(encoder > lastZeroEncoder)
        {
            absltAngle = 360 - (encoder - lastZeroEncoder)/precision;
        }else
        {
            absltAngle = (lastZeroEncoder - encoder)/precision;
        }
    }

    gLogMgEvent.WriteTo("===curEncoder:%d ,lastZeroEncode:%d",encoder,lastZeroEncoder);
    // 更新当前角度
    *lastAngle = *curAngle;
    if(absltAngle >= 360)
    {
        *curAngle = absltAngle - 360;
    }else
    {
        *curAngle = absltAngle;
    }

    // 回零
    if(state == _eSET_ZERO_ST || state == _eFIND_CYC_ST)
    {
        // 按逆时针方向旋转，编码值减少
        int32_t varVal = encoderPerCyc / 16;
        if(encoder - varVal > lowerEncoder)
        {
            outEncoder = encoder - varVal;
        }else
        {
            outEncoder = upperEncoder - (varVal - (encoder - lowerEncoder));
        }
    }

    // 到达指定角度
    if(state == _eRUN_ST)
    {
        float deltaAngle;
        int32_t deltaEncoder;

        if(*curAngle < tarAngle)
        {
            deltaAngle = tarAngle - *curAngle;
            if(deltaAngle > 180)
            {
                deltaEncoder = (int32_t)((360.0 - deltaAngle)*precision);
                if(upperEncoder - encoder < deltaEncoder)
                {
                    outEncoder = deltaEncoder - (upperEncoder - encoder) + lowerEncoder;
                }else
                {
                    outEncoder = encoder + deltaEncoder;
                }
            }else
            {
                deltaEncoder = (int32_t)(deltaAngle*precision);
                if(lowerEncoder + deltaEncoder > encoder)
                {  // 有符号类型 和 无符号类型运算 转为无符号类型比较
                    outEncoder = upperEncoder - (deltaEncoder - (encoder - lowerEncoder));
                }else
                {
                    outEncoder = encoder - deltaEncoder;
                }
            }
        }else
        {
            deltaAngle = *curAngle - tarAngle;
            if(deltaAngle > 180)
            {
                deltaEncoder = (int32_t)((360.0 - deltaAngle)*precision);
                if(lowerEncoder + deltaEncoder > encoder)
                {
                    outEncoder = upperEncoder - (deltaEncoder - (encoder - lowerEncoder));
                }else
                {
                    outEncoder = encoder - deltaEncoder;
                }
            }else
            {
                deltaEncoder = (int32_t)(deltaAngle*precision);
                if(upperEncoder - encoder < deltaEncoder)
                {
                    outEncoder = deltaEncoder - (upperEncoder - encoder) + lowerEncoder;
                }else
                {
                    outEncoder = encoder + deltaEncoder;
                }
            }
        }
    }

    return outEncoder;

}
/*
  位置式转盘动作功能块
*/
void TurntableMotionPosExt(void *P0)
{
    turnTableMotionPos_t *handle = (turnTableMotionPos_t *)P0;
  
    TurnTableMotionPos.zeroSignal = handle->zeroSignal;
    TurnTableMotionPos.encoder = handle->encoder;
    TurnTableMotionPos.encoderPerCyc = handle->encoderPerCyc; 
    TurnTableMotionPos.upperEncoder = handle->upperEncoder;
    TurnTableMotionPos.lowerEncoder = handle->lowerEncoder;
    TurnTableMotionPos.zeroOffset = handle->zeroOffset;
    TurnTableMotionPos.errorRange = handle->errorRange;
     
    handle->zeroFlag = TurnTableMotionPos.zeroFlag;
    handle->state = TurnTableMotionPos.state;
    handle->outEncoder = TurnTableMotionPos.outEncoder;
    handle->curAngle = TurnTableMotionPos.curAngle;
    handle->ecdPerCyc = TurnTableMotionPos.ecdPerCyc;

    if(handle->Cmd != handle->preCmd)
    {
        TurnTableMotionPos.Cmd = handle->Cmd;
        TurnTableMotionPos.targetAngle = handle->targetAngle;
    } 
    handle->preCmd = handle->Cmd;
}
/*
  位置式转盘动作任务
*/
void TurntableMotionPosTask(void)
{
    static uint32_t ulcycPos = 0;
    turnTableMotionPos_t *handle = (turnTableMotionPos_t *)&TurnTableMotionPos;

    float precision = handle->encoderPerCyc/360.0;
    // 非法输入，输出错误标志
    if((handle->zeroOffset >= 360) || (handle->zeroOffset < 0) || (handle->targetAngle >= 360) || (handle->targetAngle < 0) || (handle->encoderPerCyc <= 0) \
        || (handle->Cmd >= _eCMD_MAX))
    {
        handle->state = _eERROR_3;
    }else
    {
        if(handle->state == _eERROR_3)  // 用户恢复输入清除错误状态
        {
            handle->state = _eFREE_ST;
        }

        if(handle->loadData == 0)
        {
            //
            if(handle->zeroFlag != 0)
            {
                handle->zeroFlag = 2;// 表示使用断电前角度 <初始处curAngle赋值>
            }

            handle->lastZeroEncoder = handle->encoder + (int32_t)(handle->curAngle * precision);// 使用保存的angle
            handle->loadData = 1;
        }
        // gLogMgEvent.WriteTo("pos>>>curEncoder:%d ,lastZeroEncode:%d",handle->encoder,handle->lastZeroEncoder);
        if(handle->Cmd != handle->preCmd)
        {
            switch (handle->Cmd)
            {
            case _eSET_ZERO:
                {
                    // 回零操作
                    handle->state = _eSET_ZERO_ST;
                }
                break;
            
            case _eFIND_CYC:
                {
                    handle->state = _eFIND_CYC_ST;
                }
                break;
            
            case _eSTART:
                {
                    if(handle->zeroFlag == 0)
                    {
                        handle->state = _eERROR_4;
                    }else
                    {
                        // 转动到角度
                        handle->state = _eRUN_ST;
                    }
                }
                break;
            case _eCLOSE:
                {
                    handle->state = _eFREE_ST;
                }
                break;
            default:
                {
                    //
                }
                break;
            }
            handle->preCmd = handle->Cmd;
        }
    }
    // if(ulcycPos % 10 == 0)
    // {
    //     gLogMgEvent.WriteTo("Pos>>cmd:%d ,state:%d ,ecdPerCyc:%f ,tarAng:%f", handle->Cmd,handle->state,handle->ecdPerCyc,handle->targetAngle);
    // }

    if(handle->state == _eFREE_ST || handle->state == _eERROR_1 || handle->state == _eERROR_2 || handle->state == _eERROR_3|| handle->state == _eERROR_4)
    {
        handle->outEncoder = handle->encoder;
    }else
    {
        // if(handle->Cmd == _eSTART && fabs(handle->targetAngle - handle->oldTargetAngle) > 0.001f)
        // {
        //     // 目标角度发生了变化
        //     handle->state = _eRUN_ST;
        // }

        handle->outEncoder = calcTargetEncoder(handle->encoderPerCyc,handle->lastEncoder,handle->encoder,handle->lastZeroEncoder,handle->upperEncoder,handle->lowerEncoder,\
                                                handle->state,handle->targetAngle,&handle->lastAngle,&handle->curAngle);


        // 运动状态下几次没有确认零位信号，要报警停止
        if(abs(handle->encoder - handle->lastZeroEncoder) > handle->encoderPerCyc * 3)
        {
            handle->state = _eERROR_1;
            handle->zeroFlag = 0;
        }

        if(handle->zeroSignal)
        {
            if(handle->signalLevel != 1)
            {
                if(handle->passZeroSignal && handle->state == _eFIND_CYC_ST)
                {
                    int32_t cycVal = abs(handle->encoder - handle->lastZeroEncoder - (int32_t)(handle->zeroOffset * precision));
                    
                    handle->ecdPerCyc = cycVal;
                    handle->state = _eFREE_ST;// 找到实际编码值后，进入空闲状态
                }

                if(handle->state == _eRUN_ST)
                {
                    float absAng = 360 - handle->zeroOffset;// 读到物理零位置，更新自定义零位置下的角度值
                    if(absAng == 360)absAng = 0;
                    if(!isInErrorRange(handle->curAngle,absAng,1.5f))
                    {
                        gLogMgEvent.WriteTo("pos>>curangle:%f ,absAng:%f",handle->curAngle,absAng);
                        // 认为转盘断电时发生了变化或者传感器位置发生了变化，需要重新标定
                        handle->state = _eERROR_2;
                        handle->zeroFlag = 0;
                    }
                }

                // 按逆时针方向，角度变大，编码值减少
                handle->lastZeroEncoder = handle->encoder - (int32_t)(handle->zeroOffset * precision);// 读到零位置，更新零位编码器值
                handle->passZeroSignal = 1;
            }
            handle->signalLevel = 1;
        }else
        {
            handle->signalLevel = 0;
        }

        float absDeltaAng = fabs(handle->curAngle - handle->lastAngle);
        if(absDeltaAng > 180.0)
        {
            // 发生了越界 340 -> 10
            absDeltaAng = 360 - absDeltaAng;
        }
        float errRange = absDeltaAng/8 + handle->errorRange;

        if(handle->state == _eSET_ZERO_ST)
        {
            // 需要经过零位才可以,归零是逆时针的
            gLogMgEvent.WriteTo("pos>>curAng:%f deltaAng:%f ,errRange:%f ,encoder:%d, lastZero:%d",handle->curAngle,absDeltaAng,errRange,handle->encoder,handle->lastZeroEncoder);
            if((handle->passZeroSignal == 1) && abs(handle->encoder-handle->lastZeroEncoder) < handle->encoderPerCyc/1800)// 0.1 °有问题 零点位置停，没变化直接标1了
            {
                handle->outEncoder = handle->encoder;
                handle->zeroFlag = 1;   // 找到逻辑零位
                handle->state = _eARRIVE_ST;
            }

        }else if(handle->state == _eRUN_ST)
        {
            if(abs(handle->encoder - handle->outEncoder) < 5)
            {
                handle->state = _eARRIVE_ST;
            }
        }

        handle->lastEncoder = handle->encoder;
        handle->oldTargetAngle = handle->targetAngle;

    }
    gSysParam.sysData.TurnActPos.zeroFlag = handle->zeroFlag;
    gSysParam.sysData.TurnActPos.curAngle = handle->curAngle;
    ulcycPos++;

    if(handle->state == _eFREE_ST || handle->state == _eARRIVE_ST)
    {
        handle->actStatus = 0;
        handle->Cmd = _eCLOSE;
    }else if(handle->state == _eERROR_1 || handle->state == _eERROR_2 || handle->state == _eERROR_3 || handle->state == _eERROR_4)
    {
        handle->actStatus = -1;
    }else
    {
        handle->actStatus = 1;
    }
}

void TurntableMotionSpdExt(void *P0)
{
    turnTableMotionSpd_t *handle = (turnTableMotionSpd_t *)P0;
        
    TurnTableMotionSpd.zeroSignal = handle->zeroSignal;
    TurnTableMotionSpd.encoder = handle->encoder;
    TurnTableMotionSpd.encoderPerCyc = handle->encoderPerCyc;
    TurnTableMotionSpd.zeroOffset = handle->zeroOffset;
    TurnTableMotionSpd.errorRange = handle->errorRange; 
    TurnTableMotionSpd.fVang = handle->fVang;
    TurnTableMotionSpd.fAccAndDec = handle->fAccAndDec;
    TurnTableMotionSpd.zeroVang = handle->zeroVang;

    handle->zeroFlag = TurnTableMotionSpd.zeroFlag;
    handle->state = TurnTableMotionSpd.state;
    handle->curAngle = TurnTableMotionSpd.curAngle; 
    handle->rotorW = TurnTableMotionSpd.rotorW;
    handle->ecdPerCyc = TurnTableMotionSpd.ecdPerCyc;

    if(handle->Cmd != handle->preCmd)
    {
        TurnTableMotionSpd.Cmd = handle->Cmd; 
        TurnTableMotionSpd.targetAngle = handle->targetAngle;
    } 
    handle->preCmd = handle->Cmd;
}

/*
  速度式转盘动作功能块-实际任务执行
*/
void TurntableMotionSpdTask(void)
{
    static uint32_t ulcycSpd = 0;
    turnTableMotionSpd_t *handle = (turnTableMotionSpd_t *)&TurnTableMotionSpd;
    uint32_t lTick;
    gSysTimeMgEvent.CalculateTimeAxis(&lTick,&handle->PreTime);
    gSysTimeMgEvent.GetTimeAxis(&handle->PreTime);
    handle->tick = lTick/1000000.0;
    if(handle->tick > 0.03)
    {
        handle->tick = 0.03;
    }

    float precision = handle->encoderPerCyc/360.0;

    float expectAng;    // 用于拒绝执行任务过程中改变的目标角度

    if((handle->zeroOffset >= 360) || (handle->zeroOffset < 0) || (handle->targetAngle >= 360) \
                || (handle->targetAngle < 0) || (handle->fAccAndDec < 0) || (handle->zeroVang < 0) || (handle->Cmd >= _eCMD_MAX))
    {
        handle->state = _eERROR_3;
    }else
    {
        if(handle->state == _eERROR_3)
        {
            handle->state = _eFREE_ST;
        }

        if(handle->loadData == 0)
        {
            if(handle->zeroFlag != 0)
            {
                handle->zeroFlag = 2;// 表示使用断电前角度 <初始处curAngle赋值>
            }
            handle->loadData = 1;
        }

        if(handle->Cmd != handle->preCmd)
        {
            switch (handle->Cmd)
            {
            case _eSET_ZERO:
                {
                    // 回零操作
                    handle->dir = UNCERTAIN_T;    // 等待重新确认方向
                    handle->state = _eSET_ZERO_ST;
                }
                break;
            
            case _eFIND_CYC:
                {
                    handle->dir = ANTICLOCK_T;
                    handle->state = _eFIND_CYC_ST;
                }
                break;
            
            case _eSTART:
                {
                    if(handle->zeroFlag == 0)
                    {
                        handle->state = _eERROR_4;
                    }else
                    {
                        // 转动到角度
                        float detAng = fabs(handle->curAngle - handle->targetAngle);
                        if(detAng > 180)detAng = 360 - detAng;
                        if(detAng > 1.0f)
                        {
                            // 转动到角度
                            handle->oldTargetAngle = handle->targetAngle;
                            handle->dir = UNCERTAIN_T;
                            handle->state = _eRUN_ST;
                        }
                    }
                }
                break;
            case _eCLOSE:
                {
                    handle->state = _eFREE_ST;
                }
                break;
            default:
                {
                    //
                }
                break;
            }
            handle->preCmd = handle->Cmd;
        }
    }

    // if(ulcycSpd % 10 == 0)
    // {
    //     gLogMgEvent.WriteTo("spd>>cmd:%d ,state:%d ,ecdPerCyc:%d ,tarAng:%f", handle->Cmd,handle->state,handle->ecdPerCyc,handle->targetAngle);
    // }

    if(handle->state == _eFREE_ST || handle->state == _eERROR_1 || handle->state == _eERROR_2 || handle->state == _eERROR_3 || handle->state == _eERROR_4)
    {
        handle->rotorW = 0;
    }else
    {
        // if(handle->Cmd == _eSTART && handle->state != _eRUN_ST && fabs(handle->targetAngle - handle->oldTargetAngle) > 0.001f)
        // {
        //     // 目标角度发生了变化
        //     handle->state = _eRUN_ST;
        //     handle->oldTargetAngle = handle->targetAngle;
        //     handle->dir = UNCERTAIN_T;
        // }
        expectAng = handle->oldTargetAngle;

        // 更新当前角度
        if(abs(handle->encoder - handle->lastEncoder) > handle->encoderPerCyc/2)
        {
            // do nothing
        }else
        {
            int32_t deltaEncoder = handle->encoder - handle->lastEncoder;
            float deltaAngle = abs(deltaEncoder)/precision;
            handle->lastAngle = handle->curAngle;
            if(deltaEncoder > 0)
            {//顺时针转,角度值减小
                handle->curAngle -= deltaAngle;
                if(handle->curAngle < 0)handle->curAngle += 360;
            }else
            {
                handle->curAngle += deltaAngle;
                if(handle->curAngle >= 360)handle->curAngle -= 360;
            }

            // 运动状态下几次没有确认零位信号，要报警停止
            if(abs(handle->encoder - handle->lastZeroEncoder) > handle->encoderPerCyc * 3)// 这种情况是没法恢复的
            {
                handle->state = _eERROR_1;
                handle->zeroFlag = 0;
            }
        }

        updateInitState(handle->curAngle,expectAng,handle->rotorW,handle->state,&handle->dir,&handle->startAngle,&handle->startRotorW);
        
        // 更新输出角速度
        float curAngDiff,calcW;
        if(handle->state == _eSET_ZERO_ST)
        {
            calcW = fabs(handle->rotorW) + handle->fAccAndDec * handle->tick;
            if(calcW > handle->zeroVang)
            {
                calcW = handle->zeroVang;
            }
        }else if(handle->state == _eRUN_ST)
        {
            curAngDiff = fabs(handle->curAngle - expectAng);
            calcW = calcRotorW(handle->curAngle,handle->lastAngle,handle->startAngle,handle->startRotorW,curAngDiff,\
                                handle->fVang,handle->fAccAndDec,handle->rotorW,handle->zeroVang,handle->tick);
            
        }else if(handle->state == _eFIND_CYC_ST)
        {
            calcW = fabs(handle->rotorW) + handle->fAccAndDec * handle->tick;
            if(calcW > handle->zeroVang)
            {
                calcW = handle->zeroVang;
            }

        }else
        {
            calcW = 0;
        }
        // 做个安全限制保护,防止跑飞
        if(calcW > handle->fVang)
        {
            calcW = handle->fVang;
        }
        handle->rotorW = handle->dir*calcW;


        if(handle->zeroSignal)
        {
            if(handle->signalLevel != 1)// 表示重新拾到1
            {
                float absAng = 360 - handle->zeroOffset;// 读到物理零位置，更新自定义零位置下的角度值
                if(absAng == 360)absAng = 0;

                // if(handle->state == _eRUN_ST)
                // {
                //     if(!isInErrorRange(handle->curAngle,absAng,1.5f))
                //     {
                //         // 认为转盘断电时发生了变化或者传感器位置发生了变化，需要重新标定
                //         handle->state = _eERROR_2;
                //         handle->zeroFlag = 0;
                //     }
                // }

                handle->curAngle = absAng;

                if(handle->passZeroSignal && handle->state == _eFIND_CYC_ST)
                {
                    handle->ecdPerCyc = abs(handle->encoder - handle->lastZeroEncoder);
                    handle->state = _eFREE_ST;// 找到实际编码值后，进入空闲状态
                }

                handle->lastZeroEncoder = handle->encoder;
                handle->passZeroSignal = 1;
                handle->signalLevel = 1;
            }
        }else
        {
            handle->signalLevel = 0;
        }


        float errRange = RAD2ANGLE*fabs(handle->rotorW)*handle->tick/2 + handle->errorRange;

        if(handle->state == _eSET_ZERO_ST)
        {
            // 需要经过零位才可以
            if((handle->passZeroSignal == 1) && isInErrorRange(handle->curAngle,0,errRange))
            {
                handle->zeroFlag = 1;   // 找到逻辑零位
                handle->state = _eARRIVE_ST;
                handle->rotorW = 0;
            }

        }else if(handle->state == _eRUN_ST)
        {
            if(isInErrorRange(handle->curAngle,expectAng,errRange))
            {
                handle->state = _eARRIVE_ST;
                handle->rotorW = 0;
            }
        }

        handle->lastEncoder = handle->encoder;

    }

    gSysParam.sysData.TurnActSpd.zeroFlag = handle->zeroFlag;
    gSysParam.sysData.TurnActSpd.curAngle = handle->curAngle;
    ulcycSpd++;

    if(handle->state == _eFREE_ST || handle->state == _eARRIVE_ST)
    {
        handle->actStatus = 0;
        handle->Cmd = _eCLOSE;// 便于udp命令动作
    }else if(handle->state == _eERROR_1 || handle->state == _eERROR_2 || handle->state == _eERROR_3 || handle->state == _eERROR_4)
    {
        handle->actStatus = -1;
    }else
    {
        handle->actStatus = 1;
    }
}