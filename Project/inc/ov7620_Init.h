#ifndef _OV7725_INIT_H_
#define _OV7725_INIT_H_

#include <stdint.h>


// 改变摄像头采集图像的大小
#define IMAGE_SIZE  0

//配置用于OV7620CC的GPIO管脚状态定义。
//使用8位数据端口(PTD0~PTD7)采集数字摄像头OV7620的8位灰度(Y分量)输入数据
#define OV7620_DATA_PORT  PORTD  
#define OV7620_DATA_VALU  (PTD->PDIR)    
#define OV7620_PORT_INIT  (PTD->PDDR)

//采集的图像宽80像素，60行
#if (IMAGE_SIZE  ==  0)
#define OV7620_W    (80)
#define OV7620_H    (60)

//采集的图像宽160像素，120行
#elif (IMAGE_SIZE == 1)
#define OV7620_W    (160)
#define OV7620_H    (120)

//采集的图像宽240像素，180行
#elif (IMAGE_SIZE == 2)
#define OV7620_W    (240)
#define OV7620_H    (180)


#else
#error "Image Size Not Support!"
#endif

#define CAMERA_DMA_CH  HW_DMA_CH2

/*BITBAND宏定义区*/
#define BITBAND(Reg,Bit) ((uint32_t volatile*)(0x22000000 + (32*((uint32_t)&(Reg) - (uint32_t)0x20000000)) + (4*((uint32_t)(Bit)))))

/* 状态机定义，用于记录数据采集状态 */
typedef enum
{
    IMG_NOTINIT = 0,
    IMG_FINISH,             //图像采集完成
    IMG_FAIL,               //图像采集失败
    IMG_GATHER,             //图像猜忌中
    IMG_START,              //开始采集图像
    IMG_STOP,               //禁止图像采集
}OV7620_Status;



//用户程序接口声明
void extract(void);
int SCCB_Init(void);
void OV_ISR(uint32_t index);
void ov7620_Init(void);
void ov7725_Get_Imge(void);


#endif
