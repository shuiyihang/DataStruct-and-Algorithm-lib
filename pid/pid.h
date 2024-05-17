#ifndef __PID_CONTROLLER_H_
#define __PID_CONTROLLER_H_

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum{
    POS_PID = 0,
    Incre_PID,
}ControllerType;

typedef struct{
    float Kp;
	float Ki;
	float Kd;

    float delta_T;// 采样时间

    float integrator;// 累计积分值
    float prevError;
    float preprevError;

    // 微分
    float differentiator;

    float prevMeasurement;

    // D 低通滤波
    float tau;

    // 积分幅值
    float limMinInt;
	float limMaxInt;

    // 输出幅值
    float limMin;
	float limMax;

    float out;

    float (*update)(PIDController*,float,float);

    ControllerType type;
    int enable_lp_filter;// 使能低通滤波器
}PIDController;




void SetPosPidControllerFilter(PIDController* pid,int enable,float tau);
void setPosPidDelt_T(PIDController* pid,float delt_t);
void PosPidControllerInit(PIDController* pid);


void IncrePidControllerInit(PIDController* pid);


#ifdef __cplusplus
}
#endif

#endif
