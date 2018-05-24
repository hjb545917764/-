#include	"head.h"
uint8_t min_point(const uint8_t CCD_img[OV7620_H][OV7620_W],int i,int j);

void findpoint()
{
	int i,j,count;

  uint32_t sum_H	= 0;
  uint32_t sum_W	=	0;
  count	=	0;	//计算target像素点
	uint16_t pixel;
	
	/*******图像二值化********/
	
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
		
		
	/****图像数据分析***/

	if(count > Find_Cnt )
	{
		//找到信标
		target_H	=	sum_H	/	count;
		target_W	=	sum_W	/	count;
		target_flag = 1;
		
		cnt_lookfor=0;//没有找到灯计数，用以滤除50HZ干扰
		
		//信标切过		
		if( target_H < Turn_H) 
		{
			turn_flag=1; 
		}
		else
		{
			turn_flag=0;
		}

		//之前处于找信标状态
		if(target_flag_pre == 0 )
		{
			if(target_W < Img_Mid[target_H/10]){
				//信标在右边
				//左切过
				turn_select[count_record]=1;
				route_select[(count_record+1)%10]=0;
			}
			else
			{
				//信标在左边
				//右切过			
				turn_select[count_record]=0;
				route_select[(count_record+1)%10]=1;
			}
		}
		
		//进行信标避障
		if( target_H < Pass_H) 
		{
			pass_flag=1; 
		}
		else
		{
			//切灯判断
			if(pass_flag == 1)
			{
				pass_flag=0;
			}
		}
		
	}
	else
	{
		cnt_lookfor=cnt_lookfor+1;//没有找到灯计数
		
		if(cnt_lookfor==4)
		{
			//切灯判断
			if(target_H_pre< 66)
			{
				count_record++;
				count_record =	count_record%10;
			}
			//清楚标志位
			target_flag = 0;
			target_H=0;
			target_W=0;
			turn_flag=0;
			pass_flag=0;
		}
	}
	
	
			//用以计算循环时间
			static uint16_t cnt=0;
	
			//计时
			if(data_selection==1)
			{
				if(tt > 1000){
					LCD_printf(0,32,"loop_count=:%d",cnt);
					LCD_printf(0,46,"tt=:%d",tt);
					tt=0;
					cnt=0;
				}
				cnt++;
			}
			
			//打印信标信息
			if(data_selection==2)
			{
				LCD_printf(0,32,"count=:%4d",count);
				LCD_printf(64,32,"tar_F=:%d",target_flag);
				LCD_printf(0,46,"t_H=:%d",target_H);
				LCD_printf(64,46,"t_W=:%d",target_W);
				LCD_printf(0,60,"tur_F=:%d",turn_flag);
				LCD_printf(64,60,"pass_F=:%d",pass_flag);
				LCD_printf(0,74,"ang_tar=:%5d",angle_target-angle_mid);
				LCD_printf(0,88,"ang_now=:%5d",angle_now-angle_mid);
			}
			
			//串口分析
			if(data_selection==3)
			{
				UART_printf("Target_F=%d",target_flag);
				UART_printf("Turn_F=%d",turn_flag);
				UART_printf("Pass_F=%d",pass_flag);
				UART_printf("target_H=%d",target_H);
				UART_printf("target_W=%d",target_W);
				UART_printf("angle_target=%d",angle_target);
				UART_printf("angle_now=%d",angle_now);
			}
			
			if(data_selection==4)
			{
				LCD_printf(0,32,"left_speed=:%5d",qd_left_value);
				LCD_printf(0,48,"rignt_speed=:%5d",qd_right_value);
				LCD_printf(0,64,"pwm_Kp=:%5f",pwm_Kp);
				LCD_printf(0,80,"pwm_Ki=:%5f",pwm_Ki);
				LCD_printf(0,96,"pwm_Kd=:%5f",pwm_Kd);
			}
			
			//状态信息保存
			target_H_pre	= target_H;	
			target_W_pre	=	target_W;
			count_pre =	count;	//信标切过
			target_flag_pre = target_flag;
			turn_flag_pre = turn_flag;
			pass_flag_pre = pass_flag;
}






void findbarrier()
{
	  uint32_t sum_P;
    uint8_t  ave_P;
		uint8_t  evade_door;
	  uint32_t count_block;
		uint32_t count_block_P;
		uint32_t barrier_cnt;
		uint32_t sum_block_H;
		uint32_t sum_block_W;
		uint16_t i,j;
		uint16_t pixel;
	
//避障窗大小
		int b_H_up=75;
		int b_H_down=30;
		int b_W_up=260;
		int b_W_down=60;
		
		barrier_cnt=0;
		count_block=0;
		count_block_P=0;
		sum_block_H=0;
		sum_block_W=0;
		barrier_H=0;
		barrier_W=0;
		sum_P=0;
		if(Image_Change == 1)
		{
	
			//计算避障区域像素均值
			 for(i=b_H_down;i<b_H_up;i++)
			 {       
					for(j=b_W_down;j<b_W_up;j++)
					{
							pixel=min_point(CCD_Image1,i,j);
							sum_P=sum_P+pixel;
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
								pixel=min_point(CCD_Image1,i,j);
								if(pixel > evade_door)
								{
									count_block_P++;
									//在避障窗里面
									if(i<b_H_up-15 && j> (b_W_down+40) && j<(b_W_up-40))
									{
										barrier_cnt++;
										sum_block_H=sum_block_H+i;
										sum_block_W=sum_block_W+j;
									}
								}
							}
					}
			 }
			 
   }
	 else
	 {
			//计算避障区域像素均值
			 for(i=b_H_down;i<b_H_up;i++)
			 {       
					for(j=b_W_down;j<b_W_up;j++)
					{
							pixel=min_point(CCD_Image2,i,j);
							sum_P=sum_P+pixel;
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
								pixel=min_point(CCD_Image2,i,j);
								if(pixel > evade_door)
								{
									count_block_P++;
									//在避障窗里面
									if(i<b_H_up-15 && j> (b_W_down+40) && j<(b_W_up-40))
									{
										barrier_cnt++;
										sum_block_H=sum_block_H+i;
										sum_block_W=sum_block_W+j;
									}
								}
							}
					}
			 }
	 }
	 
    barrier_H=sum_block_H/barrier_cnt;
		barrier_W=sum_block_W/barrier_cnt;
		
	 
	 if(barrier_cnt>Evade_Cnt_down && count_block_P<Evade_Cnt_up && (target_H>82 || target_H==0))
	 {
		 barrier_flag=1;
	 }
	 else
	 {
		 barrier_flag=0;
	 }
		
	 if(data_selection==5)
	{

				LCD_printf(0,32,"blo_cnt=:%5d",count_block_P);
				LCD_printf(64,32,"bar_cnt=:%5d",barrier_cnt);
				LCD_printf(0,46,"b_H=:%3d",barrier_H);
				LCD_printf(64,46,"b_W=:%3d",barrier_W);
				LCD_printf(0,62,"b_door=:%4d",evade_door);
				LCD_printf(0,80,"bar_flag=:%d",barrier_flag);
				
	}

}