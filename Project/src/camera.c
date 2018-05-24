#include "head.h"

void Start_image(	void )
{
	  ov7620_img_flag = OV7620_STATE_IDLE;            //空闲状态，开始采集图像
    PORTE->ISFR = ~0; //清除后置摄像头标志位 	
		Image_Change=	1	-	Image_Change;//换场
    enable_irq(OV7620_VSYN_GPIO_IRQ);
}

 void End_image()
{
    //7620 采集图像结束  
    while(ov7620_img_flag != OV7620_STATE_COMPLETE);   //等待图像采集完毕           
} 

void send_image(void)
{
		Start_image();
		End_image();
   
		//串口发送图像
    uint16_t ii,jj;   

		for(ii=0;ii<OV7620_H;ii++)
		{
			for(jj=0;jj<OV7620_W;jj++)
			{                
					if(CCD_Image1[ii][jj]==0xff)
							CCD_Image1[ii][jj]=0xfe;
					UART_WriteByte(HW_UART4,CCD_Image1[ii][jj]);
			}
		}
					UART_WriteByte(HW_UART4,0xff); 
}


 void First_image()
{
	
		Start_image();
		End_image();
		while(1){
			Start_image();
			End_image();
			findpoint();
			if(target_flag ==1)
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
				break;
			}
		}
} 
