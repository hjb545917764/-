#include "head.h"

uint8_t PID_test_flag=0; 



//求像素点邻域最小值
uint8_t min_point(const uint8_t CCD_img[OV7620_H][OV7620_W],int i,int j);


//模块测试
void Test_module( uint32_t data )
{
	switch (data)
	{
		case 0:
			Uart_test(); //串口测试
			break;
		case 1:
			Servo_Test(); //舵机测试
			break;
		case 2:
			Motor_Test();  //电机PWM测试
			break;
		case 3:
			Image_Test();	//串口发送图像显示
			break;
		case 4:
			LCD_Test();	//LCD显示图像
			break;
		case 5:
			PID_Test();
		case 6:
			barrier_Test();
		default:		
		break;
	}
}



//串口测试
//回环发送数据
void Uart_test(	void )
{
	char	write_str[] = "hello , uart module is ready!";
	UART_printf(write_str);
	uint16_t read_byte=0;
	while(1)
	{
		
		if(	UART_ReadByte(HW_UART4,&read_byte) ==	0)
		{
			//将数据回发
			UART_WriteByte(HW_UART4,read_byte);
			//翻转指示灯电平
			GPIO_ToggleBit(HW_GPIOD,15);
		
		}
	}
}




//舵机测试
//串口控制转向
//以___#来设置pwm占空比
void Servo_Test( )
{
	uint16_t duty	=	angle_mid; 	//占空比
	uint16_t data	=	0;
	uint16_t read_byte	=	0;
	PID_speed_set(150,150); 

	while(1)
	{
		//根据串口读取的值改变占空比  形式：10000#
		if(	UART_ReadByte(HW_UART4,&read_byte) ==	0)
		{
			if( read_byte == '#')
			{
					//翻转指示灯电平
					GPIO_ToggleBit(HW_GPIOD,15);
					duty	=	data;
					CMT_PWM_ChangDuty(duty);
					data	=	0;
			}else
			{
				if(read_byte <='9' && read_byte>='0')
				{
					data = data *10 + read_byte - '0';
				}
					//数据回发
				UART_WriteByte(HW_UART4,read_byte);
			}
		}
		CMT_PWM_ChangDuty(duty);
	}
}




//电机驱动开环测试
void Motor_Test(	void )
{
	    /* 设置FTM初使占空比，范围0-10000分别表示0-100% */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 0); 
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH1, motor_speed);
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH2, 0); 
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, motor_speed);
		while(1)
		{
		}
}


//串口发送图像
void Image_Test(	void )
{
	OV7620_Init();
	Start_image();
	End_image();
	while(1)
	{
		GPIO_ToggleBit(HW_GPIOD, 15);  //小灯闪烁表示进入循环
		send_image();
	}
}


//LCD显示图像（信标检测）
void LCD_Test(	void )
{
		LCD_Init();
		OV7620_Init();
		Start_image();
		End_image();
		
		int i, j,sum_H,sum_W;
		int count;
		uint16_t pixel;
	  uint32_t sum_P;
    uint8_t  ave_P;
		uint8_t  evade_door;
	  uint32_t count_block;
		uint32_t count_block_P;
		uint32_t sum_block_H;
		uint32_t sum_block_W;

		while(1)
		{
			
		GPIO_ToggleBit(HW_GPIOD, 15);  //小灯闪烁表示进入循环
			
			
		//采集图像
		Start_image();

			
			
		//图像处理,求目标坐标
		count=0;
		sum_H=0;
		sum_W=0;
			
		for(j=0;j<OV7620_W;j++){
     for(i=0;i<OV7620_H;i++)
      {
				if(Image_Change == 1)
				{
					if( CCD_Image1[i][j] >= target_door)
					{
						if( (i>1 )&& (i<OV7620_H-1) && (j>1) && (j<OV7620_W-1))
						{
								pixel=CCD_Image1[i][j] + CCD_Image1[i-1][j] + CCD_Image1[i+1][j] + CCD_Image1[i][j-1] +CCD_Image1[i][j+1];
								pixel=pixel/5;
								if(pixel >= target_door)
								{
									count++;
									sum_H = sum_H +i;
									sum_W	= sum_W +j;
								}
						}

					}
				}
				else
				{
					if( CCD_Image2[i][j] >= target_door)
					{
						if( (i>1 )&& (i<OV7620_H-1) && (j>1) && (j<OV7620_W-1))
						{
								pixel=CCD_Image2[i][j] + CCD_Image2[i-1][j] + CCD_Image2[i+1][j] + CCD_Image2[i][j-1] +CCD_Image2[i][j+1];
								pixel=pixel/5;
								if(pixel >= target_door)
								{
									count++;
									sum_H = sum_H +i;
									sum_W	= sum_W +j;
								}
						}
					}	
				}
			}
		}
		target_H = sum_H /count;
		target_W = sum_W /count;
		

		barrier_H=sum_block_H/count_block_P;
		barrier_W=sum_block_W/count_block_P;

	
		//白底
		for(i=0;i<128;i++){
			for(j=32;j<60;j++)
			{
				LCD_Draw_Point(i,j,WHITE);
			}
		}		

		
		//信标信息打印
			LCD_printf(0,32,"t_H=:%3d",target_H);
			LCD_printf(64,32,"t_W=:%3d",target_W);
			LCD_printf(0,48,"flag=:%3d",target_flag);
			LCD_printf(64,48,"cnt=:%5d",count);
		
		//图像打印
		for(j=0;j<OV7620_W;j++){
     for(i=0;i<OV7620_H;i++)
      {
				if(Image_Change == 1)
				{
				pixel=((CCD_Image1[i][j]>>3)<<11 )| ((CCD_Image1[i][j]>>2) <<5) | (CCD_Image1[i][j]>>3);
				}
				else
				{
				pixel=((CCD_Image2[i][j]>>3)<<11 )| ((CCD_Image2[i][j]>>2) <<5) | (CCD_Image2[i][j]>>3);
				}
				
				LCD_Draw_Point(j*128/320,i+32+28,pixel);		
			}
		}

		LCD_Draw_Point(sum_W*128/320,sum_H+32+28,RED);
		End_image();		

	}
}



//LCD显示图像（避障检测）
void barrier_Test(	void )
{
		LCD_Init();
		OV7620_Init();
		Start_image();
		End_image();
		
		int i, j,sum_H,sum_W;
		int count;
		uint16_t pixel;
	  uint32_t sum_P;
    uint8_t  ave_P;
		uint8_t  evade_door;
	  uint32_t count_block;
		uint32_t count_block_P;
		uint32_t sum_block_H;
		uint32_t sum_block_W;
		
		//避障窗大小
		int b_H_up=75;
		int b_H_down=30;
		int b_W_up=260;
		int b_W_down=60;
	
	
		while(1)
		{
			
		GPIO_ToggleBit(HW_GPIOD, 15);  //小灯闪烁表示进入循环
			
			
		//采集图像
		Start_image();
		End_image();
		//图像处理,求目标坐标
			

		//自适应像素避障
		if(Image_Change == 0)
		{
			count_block=0;
			count_block_P=0;
			sum_block_H=0;
			sum_block_W=0;
			barrier_H=0;
			barrier_W=0;
			sum_P=0;
			for(i=0;i<OV7620_H;i++)
			{
				for(j=0;j<OV7620_W;j++)
				{
					CCD_Image2[i][j]=min_point(CCD_Image1,i,j);
				}
			}
			
			//计算避障区域像素均值
			 for(i=b_H_down;i<b_H_up;i++)
			 {       
					for(j=b_W_down;j<b_W_up;j++)
					{
							sum_P=sum_P+CCD_Image2[i][j];
							count_block++;
					}
			 }
			 
			 ave_P=sum_P/count_block;//计算均值
			 evade_door=ave_P+EVADE_DELTA;//计算避障的阈值
		
			 if(evade_door>Evade_door_up)
			 {
				evade_door=255;
			 }
			 
			 for(i=b_H_down;i<b_H_up;i++)
			 {       
					for(j=b_W_down;j<b_W_up;j++)
					{
							if(CCD_Image2[i][j]>evade_door)
							{
								CCD_Image2[i][j]=255;
//								sum_block_H=sum_block_H+i;
//								sum_block_W=sum_block_W+j;
									count_block_P++;
							}else
							{
								CCD_Image2[i][j]=0;
							}
						
					}
			 }
			 
			 for(i=b_H_down;i<b_H_up-15;i++)
			 {       
					for(j=b_W_down+40;j<b_W_up-40;j++)
					{
							if(CCD_Image2[i][j]>evade_door)
							{
								CCD_Image2[i][j]=255;
								sum_block_H=sum_block_H+i;
								sum_block_W=sum_block_W+j;
							}else
							{
								CCD_Image2[i][j]=125;
							}
						
					}
			 }
   }
	 else
	 {
			count_block=0;
			count_block_P=0;
			sum_block_H=0;
			sum_block_W=0;
			barrier_H=0;
			barrier_W=0;
			sum_P=0;
			for(i=0;i<OV7620_H;i++)
			{
				for(j=0;j<OV7620_W;j++)
				{
					CCD_Image1[i][j]=min_point(CCD_Image2,i,j);
				}
			}
			
			//计算避障区域像素均值
			 for(i=b_H_down;i<b_H_up;i++)
			 {       
					for(j=b_W_down;j<b_W_up;j++)
					{
							sum_P=sum_P+CCD_Image1[i][j];
							count_block++;
					}
			 }
			 
			 ave_P=sum_P/count_block;//计算均值
			 evade_door=ave_P+EVADE_DELTA;//计算避障的阈值
		
			 if(evade_door>Evade_door_up)
			 {
				evade_door=255;
			 }
			 
			 for(i=b_H_down;i<b_H_up;i++)
			 {       
					for(j=b_W_down;j<b_W_up;j++)
					{
							if(CCD_Image1[i][j]>evade_door)
							{
								CCD_Image1[i][j]=255;
//								sum_block_H=sum_block_H+i;
//								sum_block_W=sum_block_W+j;
								count_block_P++;
							}else
							{
								CCD_Image1[i][j]=0;
							}
						
					}
			 }
			 
			 for(i=b_H_down;i<b_H_up-15;i++)
			 {       
					for(j=b_W_down+40;j<b_W_up-40;j++)
					{
							if(CCD_Image1[i][j]>evade_door)
							{
								CCD_Image1[i][j]=255;
								sum_block_H=sum_block_H+i;
								sum_block_W=sum_block_W+j;
							}else
							{
								CCD_Image1[i][j]=125;
							}
						
					}
			 }
	 }

    barrier_H=sum_block_H/count_block_P;
		barrier_W=sum_block_W/count_block_P;

	
		//白底
		for(i=0;i<128;i++){
			for(j=32;j<60;j++)
			{
				LCD_Draw_Point(i,j,WHITE);
			}
		}		
		//信息打印避障
			LCD_printf(0,32,"b_H=:%3d",barrier_H);
			LCD_printf(64,32,"b_W=:%3d",barrier_W);
			LCD_printf(0,48,"door=:%3d",evade_door);
			LCD_printf(64,48,"cnt=:%5d",count_block_P);
		
		for(j=0;j<OV7620_W;j++){
     for(i=0;i<OV7620_H;i++)
      {
				if(Image_Change == 1)
				{
					pixel=((CCD_Image1[i][j]>>3)<<11 )| ((CCD_Image1[i][j]>>2) <<5) | (CCD_Image1[i][j]>>3);
				}
				else
				{
					pixel=((CCD_Image2[i][j]>>3)<<11 )| ((CCD_Image2[i][j]>>2) <<5) | (CCD_Image2[i][j]>>3);
				}
				
				LCD_Draw_Point(j*128/320,i+32+28,pixel);		
			}
		}

		LCD_Draw_Point(sum_W*128/320,sum_H+32+28,RED);		
	}
}



//PID测试，MATLAB上位机控制
void PID_Test()
{
	LCD_Init();
	uint16_t test_speed=0;
	uint16_t data	=	0;
	uint16_t read_byte	=	0;
	PID_speed_set(test_speed,test_speed);
	uint16_t state=0;

	while(1)
	{
		//根据串口读取的值改变占空比  形式：10000#
		if(	UART_ReadByte(HW_UART4,&read_byte) ==	0)
		{
			if( read_byte == '#')
			{
					//翻转指示灯电平
					GPIO_ToggleBit(HW_GPIOD,15);
					switch(state)
					{
						case 1:
							test_speed=data;//设置测试速度
							break;
						case 2:
							pwm_Kp=data;//设置Kp
							break;
						case 3:
							pwm_Ki=0.01*data;//设置Ki
							break;
						case 4:
							pwm_Kd=0.01*data;//设置Kd
							break;
						case 5:
							PID_test_flag=1;//发车
							break;
					}
					data	=	0;
			}else
			{
				if(read_byte <='9' && read_byte>='0')
				{
					data = data *10 + read_byte - '0';
				}
				
				if(read_byte == '*')
				{
					state=data;
					data=0;
				}
			}
		}

		PID_speed_set(test_speed,test_speed);
		
		
		//停车
		if(PID_test_flag==0)
		{
			test_speed=0;
		}
	}
}



//发送MATLAB示波器4通道显示数据
void send_PC_data(uint16_t data1,uint16_t data2,uint16_t data3,uint16_t data4)
{
	UART_WriteByte(HW_UART4,0xff);
	UART_WriteByte(HW_UART4,0xff);
	UART_WriteByte(HW_UART4,data1>>8);
	UART_WriteByte(HW_UART4,(data1 & 0xFF));
	UART_WriteByte(HW_UART4,data2>>8);
	UART_WriteByte(HW_UART4,(data2 & 0xFF));
	UART_WriteByte(HW_UART4,data3>>8);
	UART_WriteByte(HW_UART4,(data3 & 0xFF));
	UART_WriteByte(HW_UART4,data4>>8);
	UART_WriteByte(HW_UART4,(data4 & 0xFF));
	UART_WriteByte(HW_UART4,0x00);
	UART_WriteByte(HW_UART4,0x00);
}





//图像邻域最小点
uint8_t min_point(const uint8_t CCD_img[OV7620_H][OV7620_W],int i,int j)
{
	uint8_t min=CCD_img[i][j];
	if(i>0 && i<OV7620_H-1 && j>0 && j<OV7620_W-1)
	{
		if(CCD_img[i-1][j]<min)
		{
			min=CCD_img[i-1][j];
		}
		if(CCD_img[i+1][j]<min)
		{
			min=CCD_img[i+1][j];
		}
		if(CCD_img[i][j-1]<min)
		{
			min=CCD_img[i][j-1];
		}
		if(CCD_img[i][j+1]<min)
		{
			min=CCD_img[i][j+1];
		}
	}

	return min;
}