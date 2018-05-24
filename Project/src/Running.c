#include "head.h"




//�������г�ʼ��
void Run_init()
{
	OV7620_Init();
	Start_image();
	End_image();
	Motor_PWM_Output(0,0);
	LCD_Init();
	delay_flag=DELAY_START;
}



void Run_car()
{
	//�ɼ�ͼ��ֱ���ű�����ʱ�򷢳�
	First_image();
	while(1)
	{
		
		GPIO_ToggleBit(HW_GPIOD, 15);  //С����˸��ʾ����ѭ��		

		Start_image();	//��ʼ�ɼ�ͼ��
		
		findpoint();	//�����ű�
		
		findbarrier();//���ϼ��
		
		if( target_flag == 0)	
		{
			//δ�ҵ��ű�
			
			state=0;	//תȦ����Ѱ��
		}
		else	
		{
			//�ҵ��ű�
			if(turn_flag == 0)
			{
				state=1;	//ǰ���ű�λ��
			}
			else
			{
				state=2;	//�ű�б���й�
			}
			
			if(pass_flag == 1)
			{
				state=3;	//�ű���й�
			}
			
			if(barrier_flag ==1)
			{
				state=4;	//�ű����
			}

		}

		
		switch(state)
		{
			case 0:	//�Ҳ����ű꣬תһת��Ѱ��
				Lookfor();
				break;
			case 1:	//�ҵ��ű꣬ǰ���ű�λ��
				GotoTarget();
				break;
			case 2:	//�ű�б���й�
				Turn();
				break;
			case 3:	//�ű���й�
				Pass();
				break;
			case 4:	//�ű����
				Evade();
				break;
			default:
				Lookfor();
				break;
		}
		

		//���������ٶ�
		PID_speed_set(speed_left,speed_right); 
		
		//Motor_PWM_Output(motor_speed,motor_speed);
		
		//������
		Servo_Control();
		
		//��������
		Back();
		
		//�ȴ�ͼ��ɼ����	
		End_image();	
		
	}
	
}

