#include "head.h"


int16_t qd_left_value;           //ʹ�õ������������ֵ
int16_t qd_right_value;          //ʹ�õ��ұ���������ֵ
int16_t avespeed; //��ǰ�ٶ�
int16_t sum_left_value;           //ʹ�õ������������ֵ
int16_t sum_right_value;          //ʹ�õ��ұ���������ֵ

int16_t left_speed_desire=0;		//�������ٶ�
int16_t right_speed_desire=0;		//�������ٶ�


uint8_t direction_left;   
uint8_t direction_right;
uint32_t Left_QD_value;      //�����������ֵ
uint32_t Right_QD_value;     //�ұ���������ֵ      

float pwm_Kp = 80, pwm_Ki = 0.5, pwm_Kd=0.05;   //ǰ�������ٺͼ���pid����
static int32_t error_sum_left = 0;
static int32_t error_sum_right = 0;
static int32_t pwm_left=0;
static int32_t pwm_right=0;

uint16_t	loopcnt=0;


void QD_value( void )
{
	  FTM_QD_GetData(HW_FTM1, &Right_QD_value, &direction_right);
    FTM_QD_GetData(HW_FTM2, &Left_QD_value, &direction_left);
    FTM_QD_ClearCount(HW_FTM1);            //��ʱ20ms  ��¼����������
    FTM_QD_ClearCount(HW_FTM2);            //��ʱ20ms  ��¼����������
        
	//�����ӷ�ת��ʱ�򣬱����������ݴ�65535���¼�����ת��ʱ���0���ϼ�
	if(Left_QD_value>60000)
		Left_QD_value=-65536+Left_QD_value;
	if(Right_QD_value>60000)
		Right_QD_value=-65536+Right_QD_value;
	
	qd_left_value=Left_QD_value;
	qd_right_value=-Right_QD_value;
  avespeed=(qd_left_value+qd_right_value)/2;

	
		
		//��ʼ����ʱ����¼500������
		if(loopcnt>150 && PID_test_flag==1)
		{
			LCD_printf(0,48,"pwm_Kp=:%5f",pwm_Kp);
			LCD_printf(0,64,"pwm_Ki=:%5f",pwm_Ki);
			LCD_printf(0,80,"pwm_Kd=:%5f",pwm_Kd);
			LCD_printf(0,96,"state=:%5d",state);
			PID_test_flag=0;
			loopcnt=0;
		}
	
		//����������������ͱ��������ٶ�
		if(PID_test_flag==1)
		{
				//���ͱ���������
			loopcnt++;
			send_PC_data(qd_left_value,qd_right_value,0xF0,0xF0);
		}
}


void Servo_Control()
{     
    
		
			switch(state)
		{
			case 0:	//�Ҳ����ű꣬תһת��Ѱ��
				angle_now = angle_target;
				break;
			
			case 1:	//�ҵ��ű꣬ǰ���ű�λ��
				angle_now = angle_target	+	angle_Kd*(angle_fore-angle_target) /8;  
				break;
			
			case 2:	//�ű��й�
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
			
			case 3:	//�ű����
				angle_now = angle_target;
				break;
			case 4:
				angle_now = angle_target;
				break;
			default:
				break;
		}

      
		//��ֹ�����������
    if( angle_now < angle_right)
		{
        angle_now=angle_right;
    }
		if( angle_now > angle_left)
    {    
				angle_now=angle_left;     
		}
		
		//��¼��ǰ�Ƕ�
		angle_fore = angle_now;
		
    //������ֻ������ʵ��
    CMT_PWM_ChangDuty(angle_now); 
		
		if(state==4)
		{
			DelayMs(50);
		}
}

//�������ҵ����ת
void Motor_PWM_Output( int left_value, int right_value )
{
	/*�趨���Ҽ��ޣ���ֹԽ��*/
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
	/*�������*/
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



//PID����
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




