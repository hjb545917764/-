#include "head.h"


int16_t qd_left_value;           //使用的左编码器计数值
int16_t qd_right_value;          //使用的右编码器计数值
int16_t avespeed; //当前速度
int16_t sum_left_value;           //使用的左编码器计数值
int16_t sum_right_value;          //使用的右编码器计数值

int16_t left_speed_desire=0;		//左期望速度
int16_t right_speed_desire=0;		//右期望速度


uint8_t direction_left;   
uint8_t direction_right;
uint32_t Left_QD_value;      //左编码器计数值
uint32_t Right_QD_value;     //右编码器计数值      

float pwm_Kp = 80, pwm_Ki = 0.5, pwm_Kd=0.05;   //前向电机加速和减速pid参数
static int32_t error_sum_left = 0;
static int32_t error_sum_right = 0;
static int32_t pwm_left=0;
static int32_t pwm_right=0;

uint16_t	loopcnt=0;


void QD_value( void )
{
	  FTM_QD_GetData(HW_FTM1, &Right_QD_value, &direction_right);
    FTM_QD_GetData(HW_FTM2, &Left_QD_value, &direction_left);
    FTM_QD_ClearCount(HW_FTM1);            //延时20ms  记录编码器数据
    FTM_QD_ClearCount(HW_FTM2);            //延时20ms  记录编码器数据
        
	//当轮子反转的时候，编码器的数据从65535往下减。正转的时候从0往上加
	if(Left_QD_value>60000)
		Left_QD_value=-65536+Left_QD_value;
	if(Right_QD_value>60000)
		Right_QD_value=-65536+Right_QD_value;
	
	qd_left_value=Left_QD_value;
	qd_right_value=-Right_QD_value;
  avespeed=(qd_left_value+qd_right_value)/2;

	
		
		//开始发车时，记录500次数据
		if(loopcnt>150 && PID_test_flag==1)
		{
			LCD_printf(0,48,"pwm_Kp=:%5f",pwm_Kp);
			LCD_printf(0,64,"pwm_Ki=:%5f",pwm_Ki);
			LCD_printf(0,80,"pwm_Kd=:%5f",pwm_Kd);
			LCD_printf(0,96,"state=:%5d",state);
			PID_test_flag=0;
			loopcnt=0;
		}
	
		//如果蓝牙发车，则发送编码器的速度
		if(PID_test_flag==1)
		{
				//发送编码器数据
			loopcnt++;
			send_PC_data(qd_left_value,qd_right_value,0xF0,0xF0);
		}
}


void Servo_Control()
{     
    
		
			switch(state)
		{
			case 0:	//找不到信标，转一转来寻找
				angle_now = angle_target;
				break;
			
			case 1:	//找到信标，前往信标位置
				angle_now = angle_target	+	angle_Kd*(angle_fore-angle_target) /8;  
				break;
			
			case 2:	//信标切过
						angle_now = angle_target;
						if( angle_now < angle_turn_right)
						{
								angle_now=angle_turn_right;
						}
						if( angle_now > angle_turn_left)
						{    
								angle_now=angle_turn_left;     
						}
				break;
			
			case 3:	//信标避障
				angle_now = angle_target;
				break;
			case 4:
				angle_now = angle_target;
				break;
			default:
				break;
		}

      
		//防止舵机超出极限
    if( angle_now < angle_right)
		{
        angle_now=angle_right;
    }
		if( angle_now > angle_left)
    {    
				angle_now=angle_left;     
		}
		
		//记录当前角度
		angle_fore = angle_now;
		
    //舵机打角只在这里实现
    CMT_PWM_ChangDuty(angle_now); 
		
		if(state==4)
		{
			DelayMs(50);
		}
}

//驱动左右电机运转
void Motor_PWM_Output( int left_value, int right_value )
{
	/*设定左右极限，防止越界*/
	if(left_value > 10000)
	{
		left_value = 10000*PWM_RATE;
	}
	
	if(left_value < -10000)
	{
		left_value = -10000*PWM_RATE;
	}
	
	if(right_value>10000)
	{
		right_value=10000*PWM_RATE;
	}
	
	if(right_value<-10000)
	{
		right_value=-10000*PWM_RATE;
	}
	/*驱动电机*/
	if (left_value > 0)
	{
		FTM_PWM_ChangeDuty (HW_FTM0 ,HW_FTM_CH3 ,0 );
		FTM_PWM_ChangeDuty (HW_FTM0 ,HW_FTM_CH2 ,left_value);
	}
	else
	{
		left_value = -left_value ;	
		FTM_PWM_ChangeDuty (HW_FTM0 ,HW_FTM_CH3 ,left_value );
			FTM_PWM_ChangeDuty (HW_FTM0 ,HW_FTM_CH2 ,0 );
	}
	if ( right_value > 0)
	{
		FTM_PWM_ChangeDuty (HW_FTM0, HW_FTM_CH0, 0 ); 
		FTM_PWM_ChangeDuty (HW_FTM0, HW_FTM_CH1, right_value);
	}
	else
	{
		right_value = -right_value ;	
		FTM_PWM_ChangeDuty (HW_FTM0, HW_FTM_CH0, right_value ); 
		FTM_PWM_ChangeDuty (HW_FTM0, HW_FTM_CH1, 0 );
	}
}



//PID控制
void Motor_PID_Output()
{
	int32_t speed_error_left=0;
	int32_t speed_error_right=0;
	int32_t speed_error_left_pre=0;
	int32_t speed_error_right_pre=0;
	int32_t errD_l,errD_r;

	speed_error_left = left_speed_desire - qd_left_value;
	speed_error_right = right_speed_desire - qd_right_value;	
	errD_l=speed_error_left-speed_error_left_pre;
	errD_r=speed_error_right-speed_error_right_pre;


	error_sum_left += speed_error_left;
	error_sum_right += speed_error_right;

	pwm_left = pwm_Kp*speed_error_left + pwm_Ki*error_sum_left +pwm_Kd*errD_l;
	pwm_right = pwm_Kp*speed_error_right + pwm_Ki*error_sum_right +pwm_Kd*errD_r;
	
	if(error_sum_left>ERR_SUM_LIMIT)
	{
		error_sum_left=ERR_SUM_LIMIT;
	}
	if(error_sum_left<-ERR_SUM_LIMIT)
	{
		error_sum_left=-ERR_SUM_LIMIT;
	}
	if(error_sum_right>ERR_SUM_LIMIT)
	{
		error_sum_right=ERR_SUM_LIMIT;
	}
	if(error_sum_right<-ERR_SUM_LIMIT)
	{
		error_sum_right=-ERR_SUM_LIMIT;
	}

	Motor_PWM_Output(pwm_left,pwm_right);
	speed_error_left_pre=speed_error_left;
	speed_error_right_pre=speed_error_right;
}



void PID_speed_set(int leftspeed, int rightspeed)
{
	left_speed_desire=leftspeed;
	right_speed_desire=rightspeed;
}




