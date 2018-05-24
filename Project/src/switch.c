#include "switch.h"

uint16_t mode_switch[4]={0,0,0,0};//读取拔码开关的输入
uint16_t data_switch[4]={0,0,0,0};

uint32_t switch_mode( void )
{
			uint32_t status;
			//	return status;
	
			GPIO_QuickInit(HW_GPIOB, 0, kGPIO_Mode_IPU);
			GPIO_QuickInit(HW_GPIOB, 1, kGPIO_Mode_IPU);
			GPIO_QuickInit(HW_GPIOB, 2, kGPIO_Mode_IPU);
			GPIO_QuickInit(HW_GPIOB, 3, kGPIO_Mode_IPU);

	
        if(GPIO_ReadBit(HW_GPIOB, 0)==0)    //拨码开关控制  
       {
				mode_switch[0]=1;
       }      
       	if(GPIO_ReadBit(HW_GPIOB, 1)==0)    //拨码开关控制  
       {
					mode_switch[1]=1;
       }
       	if(GPIO_ReadBit(HW_GPIOB, 2)==0)    //拨码开关控制  
       {
					mode_switch[2]=1;
       }
       	if(GPIO_ReadBit(HW_GPIOB, 3)==0)    //拨码开关控制  
       {
					mode_switch[3]=1;
       }   
       status=mode_switch[0]+2* mode_switch[1]+2*2*mode_switch[2]+2*2*2*mode_switch[3];
	   return status;
}

uint32_t switch_data( void )
{
			uint32_t status;
			if(GPIO_ReadBit(HW_GPIOB, 4)==0)    //拨码开关控制  
		 {
				data_switch[0]=1;
		 }      
			if(GPIO_ReadBit(HW_GPIOB, 5)==0)    //拨码开关控制  
		 {
				data_switch[1]=1;
		 }
			if(GPIO_ReadBit(HW_GPIOB, 6)==0)    //拨码开关控制  
		 {
				data_switch[2]=1;
		 }
			if(GPIO_ReadBit(HW_GPIOB, 7)==0)    //拨码开关控制  
		 {
				data_switch[3]=1;
		 }   
		 status=data_switch[0]+2* data_switch[1]+2*2*data_switch[2]+2*2*2*data_switch[3];
		 
		 return status;
}
