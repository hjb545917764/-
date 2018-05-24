#ifndef __SCCB_H  
#define __SCCB_H  
  
  
#define SCL_HIGH         PEout(1) = 1    //����Ϊ��������1
#define SCL_LOW         PEout(1) = 0    //����Ϊ��������0
#define SCL_OUT     PTE->PDDR|=(1<<1)   //����Ϊ���
//#define   SCL_DDR_IN()      PTE->PDDR&=~(1<<1)//����
  
#define SDA_HIGH         PEout(0)= 1    //����Ϊ��������1
#define SDA_LOW         PEout(0)= 0    //����Ϊ��������0
#define SDA_DATA        PEin(0)  
#define SDA_OUT   PTE->PDDR|=(1<<0)  //����Ϊ���
#define SDA_IN    PTE->PDDR&=~(1<<0) //����Ϊ����
#define u8 unsigned char  
#define u16 unsigned short   
  
//#define ADDR_OV7725   0x42  
  
void sccb_init(void);                           //��ʼ��SCCB�˿�ΪGPIO  
void sccb_wait(void);                           //SCCBʱ����ʱ
void sccb_start(void);                          //��ʼ��־
void sccb_stop(void);                           //ֹͣ��־
void sccb_regWrite(u8 device,u8 address,u8 data);  
#endif   
#include "sys.h"  
#include "gpio.h"  
#include "sccb.h"  
#include "delay.h"  
#include "stdio.h"  
  
/************************************************************************* 
*  ��������:sccb_init 
*  ����˵��:��ʼ��SCCB  ����SCL��PE1 SDA��PTE0 
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
*  ��������:sccb_wait 
*  ����˵��:SCCB��ʱ,��Ӧ̫С
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
*  ��������:sccb_start 
*  ����˵��:SCCB����λ
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
*  ��������:sccb_stop
*  ����˵��:SCCBֹͣλ
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
*  ��������:sccb_sendByte 
*  ����˵��:��SCCB�����Ϸ���һ���ֽ� 
*  ����˵��:data Ҫ���͵��ֽ�����
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
*  ��������:sccb_regWrite 
*  ����˵��:ͨ��SCCB������ָ���豸��ָ����ַ����ָ������ 
*  ����˵��:device---�豸�� ��д������ 42�Ƕ� 43��д
*            address---д���ݵļĴ���
*            data---д������
*  ��������:ack=1δ�յ�Ӧ��(ʧ��)    ack=0�յ�Ӧ��(�ɹ�) 
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