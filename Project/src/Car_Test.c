#include "head.h"

uint8_t PID_test_flag=0; 



//�����ص�������Сֵ
uint8_t min_point(const uint8_t CCD_img[OV7620_H][OV7620_W],int i,int j);


//ģ�����
void Test_module( uint32_t data )
{
	switch (data)
	{
		case 0:
			Uart_test(); //���ڲ���
			break;
		case 1:
			Servo_Test(); //�������
			break;
		case 2:
			Motor_Test();  //���PWM����
			break;
		case 3:
			Image_Test();	//���ڷ���ͼ����ʾ
			break;
		case 4:
			LCD_Test();	//LCD��ʾͼ��
			break;
		case 5:
			PID_Test();
		case 6:
			barrier_Test();
		default:		
		break;
	}
}



//���ڲ���
//�ػ���������
void Uart_test(	void )
{
	char	write_str[] = "hello , uart module is ready!";
	UART_printf(write_str);
	uint16_t read_byte=0;
	while(1)
	{
		
		if(	UART_ReadByte(HW_UART4,&read_byte) ==	0)
		{
			//�����ݻط�
			UART_WriteByte(HW_UART4,read_byte);
			//��תָʾ�Ƶ�ƽ
			GPIO_ToggleBit(HW_GPIOD,15);
		
		}
	}
}




//�������
//���ڿ���ת��
//��___#������pwmռ�ձ�
void Servo_Test( )
{
	uint16_t duty	=	angle_mid; 	//ռ�ձ�
	uint16_t data	=	0;
	uint16_t read_byte	=	0;
	PID_speed_set(150,150); 

	while(1)
	{
		//���ݴ��ڶ�ȡ��ֵ�ı�ռ�ձ�  ��ʽ��10000#
		if(	UART_ReadByte(HW_UART4,&read_byte) ==	0)
		{
			if( read_byte == '#')
			{
					//��תָʾ�Ƶ�ƽ
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
					//���ݻط�
				UART_WriteByte(HW_UART4,read_byte);
			}
		}
		CMT_PWM_ChangDuty(duty);
	}
}




//���������������
void Motor_Test(	void )
{
	    /* ����FTM��ʹռ�ձȣ���Χ0-10000�ֱ��ʾ0-100% */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 0); 
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH1, motor_speed);
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH2, 0); 
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, motor_speed);
		while(1)
		{
		}
}


//���ڷ���ͼ��
void Image_Test(	void )
{
	OV7620_Init();
	Start_image();
	End_image();
	while(1)
	{
		GPIO_ToggleBit(HW_GPIOD, 15);  //С����˸��ʾ����ѭ��
		send_image();
	}
}


//LCD��ʾͼ���ű��⣩
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
			
		GPIO_ToggleBit(HW_GPIOD, 15);  //С����˸��ʾ����ѭ��
			
			
		//�ɼ�ͼ��
		Start_image();

			
			
		//ͼ����,��Ŀ������
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

	
		//�׵�
		for(i=0;i<128;i++){
			for(j=32;j<60;j++)
			{
				LCD_Draw_Point(i,j,WHITE);
			}
		}		

		
		//�ű���Ϣ��ӡ
			LCD_printf(0,32,"t_H=:%3d",target_H);
			LCD_printf(64,32,"t_W=:%3d",target_W);
			LCD_printf(0,48,"flag=:%3d",target_flag);
			LCD_printf(64,48,"cnt=:%5d",count);
		
		//ͼ���ӡ
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



//LCD��ʾͼ�񣨱��ϼ�⣩
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
		
		//���ϴ���С
		int b_H_up=75;
		int b_H_down=30;
		int b_W_up=260;
		int b_W_down=60;
	
	
		while(1)
		{
			
		GPIO_ToggleBit(HW_GPIOD, 15);  //С����˸��ʾ����ѭ��
			
			
		//�ɼ�ͼ��
		Start_image();
		End_image();
		//ͼ����,��Ŀ������
			

		//����Ӧ���ر���
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
			
			//��������������ؾ�ֵ
			 for(i=b_H_down;i<b_H_up;i++)
			 {       
					for(j=b_W_down;j<b_W_up;j++)
					{
							sum_P=sum_P+CCD_Image2[i][j];
							count_block++;
					}
			 }
			 
			 ave_P=sum_P/count_block;//�����ֵ
			 evade_door=ave_P+EVADE_DELTA;//������ϵ���ֵ
		
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
			
			//��������������ؾ�ֵ
			 for(i=b_H_down;i<b_H_up;i++)
			 {       
					for(j=b_W_down;j<b_W_up;j++)
					{
							sum_P=sum_P+CCD_Image1[i][j];
							count_block++;
					}
			 }
			 
			 ave_P=sum_P/count_block;//�����ֵ
			 evade_door=ave_P+EVADE_DELTA;//������ϵ���ֵ
		
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

	
		//�׵�
		for(i=0;i<128;i++){
			for(j=32;j<60;j++)
			{
				LCD_Draw_Point(i,j,WHITE);
			}
		}		
		//��Ϣ��ӡ����
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



//PID���ԣ�MATLAB��λ������
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
		//���ݴ��ڶ�ȡ��ֵ�ı�ռ�ձ�  ��ʽ��10000#
		if(	UART_ReadByte(HW_UART4,&read_byte) ==	0)
		{
			if( read_byte == '#')
			{
					//��תָʾ�Ƶ�ƽ
					GPIO_ToggleBit(HW_GPIOD,15);
					switch(state)
					{
						case 1:
							test_speed=data;//���ò����ٶ�
							break;
						case 2:
							pwm_Kp=data;//����Kp
							break;
						case 3:
							pwm_Ki=0.01*data;//����Ki
							break;
						case 4:
							pwm_Kd=0.01*data;//����Kd
							break;
						case 5:
							PID_test_flag=1;//����
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
		
		
		//ͣ��
		if(PID_test_flag==0)
		{
			test_speed=0;
		}
	}
}



//����MATLABʾ����4ͨ����ʾ����
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





//ͼ��������С��
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