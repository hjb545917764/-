#ifndef __SCCB_H  
#define __SCCB_H  
  
  
#define SCL_HIGH         PEout(1) = 1    //设置为输出后输出1
#define SCL_LOW         PEout(1) = 0    //设置为输出后输出0
#define SCL_OUT     PTE->PDDR|=(1<<1)   //设置为输出
//#define   SCL_DDR_IN()      PTE->PDDR&=~(1<<1)//输入
  
#define SDA_HIGH         PEout(0)= 1    //设置为输出后输出1
#define SDA_LOW         PEout(0)= 0    //设置为输出后输出0
#define SDA_DATA        PEin(0)  
#define SDA_OUT   PTE->PDDR|=(1<<0)  //设置为输出
#define SDA_IN    PTE->PDDR&=~(1<<0) //设置为输入
#define u8 unsigned char  
#define u16 unsigned short   
  
//#define ADDR_OV7725   0x42  
  
void sccb_init(void);                           //初始化SCCB端口为GPIO  
void sccb_wait(void);                           //SCCB时序延时
void sccb_start(void);                          //起始标志
void sccb_stop(void);                           //停止标志
void sccb_regWrite(u8 device,u8 address,u8 data);  
#endif   
#include "sys.h"  
#include "gpio.h"  
#include "sccb.h"  
#include "delay.h"  
#include "stdio.h"  
  
/************************************************************************* 
*  函数名称:sccb_init 
*  功能说明:初始化SCCB  其中SCL接PE1 SDA接PTE0 
*************************************************************************/  
void sccb_init(void)  
{  
            int i ;  
            GPIO_InitTypeDef GPIO_InitStruct1;    
            for(i=0;i<8;i++)  
            {  
                GPIO_InitStruct1.GPIO_Pin = i;  
                GPIO_InitStruct1.GPIO_InitState = Bit_RESET; //change as Bit_Set , it will shut.  
                GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;  
                GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
                GPIO_InitStruct1.GPIOx = PTD;  
                GPIO_Init(&GPIO_InitStruct1);  
            }  
              
             GPIO_InitStruct1.GPIO_Pin = 0;  
             GPIO_InitStruct1.GPIO_InitState = Bit_RESET;  
             GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;  
             GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;  
             GPIO_InitStruct1.GPIOx = PTE;  
             GPIO_Init(&GPIO_InitStruct1);    
              
            GPIO_InitStruct1.GPIO_Pin = 1;  
            GPIO_InitStruct1.GPIO_InitState = Bit_RESET;  
            GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;  
            GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;  
            GPIO_InitStruct1.GPIOx = PTE;  
            GPIO_Init(&GPIO_InitStruct1);  
}  
/************************************************************************ 
*  函数名称:sccb_wait 
*  功能说明:SCCB延时,不应太小
*************************************************************************/  
void sccb_wait(void)  
{  
  u8 i;  
  u16 j;  
  for( i=0; i<100; i++)  
  {  
    j++;  
  }  
}  
/************************************************************************ 
*  函数名称:sccb_start 
*  功能说明:SCCB启动位
*************************************************************************/  
void sccb_start(void)  
{  
  SCL_OUT;  
  SDA_OUT;  
   
  SDA_HIGH;  
  //sccb_wait();  
  SCL_HIGH;  
  sccb_wait();  
  SDA_LOW;  
  sccb_wait();  
  SCL_LOW;  
}  
  
/************************************************************************ 
*  函数名称:sccb_stop
*  功能说明:SCCB停止位
*************************************************************************/  
void sccb_stop(void)  
{  
  SCL_OUT;  
  SDA_OUT;  
    
  SDA_LOW;  
  sccb_wait();  
  SCL_HIGH;  
  sccb_wait();  
  SDA_HIGH;  
  sccb_wait();  
}  
  
/************************************************************************ 
*  函数名称:sccb_sendByte 
*  功能说明:在SCCB总线上发送一个字节 
*  参数说明:data 要发送的字节内容
*************************************************************************/  
u8 sccb_sendByte(u8 data)  
{  
  u8 i;  
   u8 ack;  
  SDA_OUT;  
  for( i=0; i<8; i++)  
  {  
    if(data & 0x80)  
      SDA_HIGH;  
    else   
      SDA_LOW;  
    data <<= 1;  
    sccb_wait();  
    SCL_HIGH;  
    sccb_wait();  
    SCL_LOW;  
    sccb_wait();  
  }  
  SDA_HIGH;  
  SDA_IN;  
  sccb_wait();  
  SCL_HIGH;  
  sccb_wait();  
  ack = SDA_DATA;  
  SCL_LOW;  
  sccb_wait();  
  return ack;  
}  
  
  
/************************************************************************ 
*  函数名称:sccb_regWrite 
*  功能说明:通过SCCB总线向指定设备的指定地址发送指定内容 
*  参数说明:device---设备号 读写有区别 42是读 43是写
*            address---写数据的寄存器
*            data---写的内容
*  函数返回:ack=1未收到应答(失败)    ack=0收到应答(成功) 
*************************************************************************/  
void sccb_regWrite(u8 device,u8 address,u8 data)  
{  
 // u8 i;  
  u8 ack;  
//  for( i=0; i<20; i++)  
//  {  
    sccb_start();  
    ack = sccb_sendByte(device);  
    while( ack )  
    {  
      ack = sccb_sendByte(device);  
    //      printf("device\n\r");  
    }     
    ack = sccb_sendByte(address);  
    while( ack )  
    {  
      ack = sccb_sendByte(address);;  
  //    printf("address\n\r");  
    }  
    ack = sccb_sendByte(data);  
    while( ack  )  
    {  
     ack = sccb_sendByte(data);  
  //    printf("data\n\r");  
    }     
    sccb_stop();  
//    if( ack == 0 ) break;  
//  }  
}  