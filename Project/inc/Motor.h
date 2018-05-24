#ifndef _MOTOR_H_
#define _MOTOR_H_
#include "head.h"

#define PWM_RATE 1
#define ERR_SUM_LIMIT 10000

void Motor_PWM_Output( int left_value, int right_value );
void Servo_Control(void);
void Motor_PID_Output();
void PID_speed_set(int leftspeed, int rightspeed);
void QD_value( void );
extern int16_t qd_left_value;           //ʹ�õ������������ֵ
extern int16_t qd_right_value;          //ʹ�õ��ұ���������ֵ
extern int16_t avespeed; //��ǰ�ٶ�
extern float pwm_Kp, pwm_Ki, pwm_Kd; 


#endif
