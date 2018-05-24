#ifndef __CH_LIB_OV7620_H__
#define __CH_LIB_OV7620_H__

#include "head.h"

//摄像头物理分辨率
#define OV7620_W 320
#define OV7620_H 100


extern DMA_InitTypeDef DMA_InitStruct1; 



extern uint8_t ov7620_img_flag;
extern uint8_t Image_Change;

extern uint8_t CCD_Image1[OV7620_H][OV7620_W];//第一幅图像
extern uint8_t CCD_Image2[OV7620_H][OV7620_W];//第一幅图像

//配置用于OV7620CC的GPIO管脚状态定义。
//使用8位数据端口(PTE0~PTE7)采集数字摄像头OV7620的8位灰度(Y分量)输入数据
#define OV7620_DATA_PORT  PORTE  
#define OV7620_DATA_VALU  (PTE->PDIR)    
#define OV7620_PORT_INIT  (PTE->PDDR)

#define CAMERA_DMA_CH  HW_DMA_CH0   //DMA0通道

//HW_DMA_CH0与DMA0_IRQn数值相等,两者等同
//  DMA0_IRQn                    = 0,                /**< DMA Channel 0 Transfer Complete */
//  DMA1_IRQn                    = 1,                /**< DMA Channel 1 Transfer Complete */
//  DMA2_IRQn                    = 2,                /**< DMA Channel 2 Transfer Complete */
//  DMA3_IRQn                    = 3,                /**< DMA Channel 3 Transfer Complete */

/* 状态机定义，用于记录数据采集状态 */
#define OV7620_STATE_IDLE          (0)          //允许采集
#define OV7620_STATE_START         (1)		    //正在采集
#define OV7620_STATE_COMPLETE      (2)	    	//采集完成
#define OV7620_STATE_PROCEED       (3)          //数据处理
#define OV7620_STATE_SENDING       (4)          //数据处理


//行中断
#define OV7620_HREF_PORT  HW_GPIOD
#define OV7620_HREF_PIN   (14)
#define OV7620_HREF_GPIO_IRQ  PORTD_IRQn

//场中断
#define OV7620_VSYNC_PORT HW_GPIOE
#define OV7620_VSYNC_PIN  (11)
#define OV7620_VSYN_GPIO_IRQ  PORTE_IRQn

//像素中断
#define OV7620_PCLK_PORT  HW_GPIOE
#define OV7620_PCLK_PIN  (10)
#define OV7620_PCLK_eDMA_IRQ  DMA0_IRQn

//接口函数申明
void OV7620_Init(void);
void ov7620_DMA(void);//函数申明

#endif
