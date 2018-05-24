#ifndef _OV7725_INIT_H_
#define _OV7725_INIT_H_

#include <stdint.h>


// 改变摄像头采集图像的大小
#define IMAGE_SIZE  0

//采集的图像宽80像素，60行
#if (IMAGE_SIZE  ==  0)
#define OV7725_W    (80)
#define OV7725_H    (60)

//采集的图像宽160像素，120行
#elif (IMAGE_SIZE == 1)
#define OV7725_W    (160)
#define OV7725_H    (120)

//采集的图像宽240像素，180行
#elif (IMAGE_SIZE == 2)
#define OV7725_W    (240)
#define OV7725_H    (180)


#else
#error "Image Size Not Support!"
#endif

#define CAMERA_7_DMA_CH  HW_DMA_CH2
#define OV7725_PORT_INIT1111  (PTC->PDDR)
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
}OV7725_Status;

extern uint8_t CCDBufferPool[OV7725_H*(OV7725_W/8)];

extern uint8_t img[OV7725_H*OV7725_W];



extern int8_t center[OV7725_H];

extern volatile OV7725_Status ov7725_img_flag;



//用户程序接口声明
void extract(void);
int SCCB_Init(void);
void OV_ISR(uint32_t index);
void ov7725_Init(void);
void ov7725_Get_Imge(void);
void img_extract(uint8_t *dst, uint8_t *src, uint32_t srclen);

#endif
