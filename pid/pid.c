#include "pid.h"

static float PosControllerUpdate(PIDController* this,float setpoint, float measurement)
{
    float error = setpoint - measurement;

    float proportional = this->Kp * error;// P

    this->integrator = this->integrator + 0.5f * this->Ki * this->delta_T * (error + this->prevError);

    // 积分限幅
    if (this->integrator > this->limMaxInt)
    {
        this->integrator = this->limMaxInt;
    } else if (this->integrator < this->limMinInt)
    {
        this->integrator = this->limMinInt;
    }

    if(0 != this->enable_lp_filter)
    {
        this->differentiator = this->Kd/(this->tau + this->delta_T)*(this->prevMeasurement - measurement) + this->tau/(this->tau + this->delta_T)*this->differentiator;
    }else
    {
        this->differentiator = this->Kd*(this->prevMeasurement - measurement)/this->delta_T;
    }

    this->out = proportional + this->integrator + this->differentiator;

    if (this->out > this->limMax)
    {
        this->out = this->limMax;
    } else if (this->out < this->limMin)
    {
        this->out = this->limMin;
    }

    this->prevError       = error;
    this->prevMeasurement = measurement;

    return this->out;
}


// 输出增量
static float IncrementalControllerUpdate(PIDController* this,float setpoint, float measurement)
{

    float error = setpoint - measurement;

    float proportional = this->Kp * (error - this->prevError);// P

    this->integrator = this->Ki * error;

    this->differentiator = this->Kd*(error-2*this->prevError+this->preprevError);

    this->out = proportional + this->integrator + this->differentiator;

    if (this->out > this->limMax)
    {
        this->out = this->limMax;
    } else if (this->out < this->limMin)
    {
        this->out = this->limMin;
    }

    this->preprevError    = this->prevError;
    this->prevError       = error;

    return this->out;
}

// tau(10ms)
void SetPosPidControllerFilter(PIDController* pid,int enable,float tau)
{
    pid->enable_lp_filter = enable;
    pid->tau = tau;
}

// update前必须重新调用
void setPosPidDelt_T(PIDController* pid,float delt_t)
{
    pid->delta_T = delt_t;
}
void PosPidControllerInit(PIDController* pid)
{
    pid->Kp = 1.0f;
    pid->Ki = 0.0f;
    pid->Kd = 0.0f;

    pid->integrator = 0;// 累计积分值
    pid->prevError = 0;
    pid->preprevError = 0;

    // 微分
    pid->differentiator = 0;

    // 输出幅值
    pid->limMin = 0;
	pid->limMax = 50;// 电机pwm
    
    pid->update = IncrementalControllerUpdate;
}

void IncrePidControllerInit(PIDController* pid)
{
    pid->Kp = 1.0f;
    pid->Ki = 0.0f;
    pid->Kd = 0.0f;

    pid->integrator = 0;// 累计积分值
    pid->prevError = 0;

    // 微分
    pid->differentiator = 0;
    pid->prevMeasurement = 0;

    // 积分幅值
    pid->limMinInt = -10;
	pid->limMaxInt = 10;
    // 输出幅值
    pid->limMin = 0;
	pid->limMax = 50;// 电机pwm
    
    pid->enable_lp_filter = 1;
    pid->update = PosControllerUpdate;
}