#include "head.h"




//车辆运行初始化
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
	//采集图像直到信标亮起时候发车
	First_image();
	while(1)
	{
		
		GPIO_ToggleBit(HW_GPIOD, 15);  //小灯闪烁表示进入循环		

		Start_image();	//开始采集图像
		
		findpoint();	//搜索信标
		
		findbarrier();//避障检查
		
		if( target_flag == 0)	
		{
			//未找到信标
			
			state=0;	//转圈进行寻找
		}
		else	
		{
			//找到信标
			if(turn_flag == 0)
			{
				state=1;	//前往信标位置
			}
			else
			{
				state=2;	//信标斜向切过
			}
			
			if(pass_flag == 1)
			{
				state=3;	//信标近切过
			}
			
			if(barrier_flag ==1)
			{
				state=4;	//信标避障
			}

		}

		
		switch(state)
		{
			case 0:	//找不到信标，转一转来寻找
				Lookfor();
				break;
			case 1:	//找到信标，前往信标位置
				GotoTarget();
				break;
			case 2:	//信标斜向切过
				Turn();
				break;
			case 3:	//信标近切过
				Pass();
				break;
			case 4:	//信标避障
				Evade();
				break;
			default:
				Lookfor();
				break;
		}
		

		//设置运行速度
		PID_speed_set(speed_left,speed_right); 
		
		//Motor_PWM_Output(motor_speed,motor_speed);
		
		//舵机打角
		Servo_Control();
		
		//倒车控制
		Back();
		
		//等待图像采集完毕	
		End_image();	
		
	}
	
}

