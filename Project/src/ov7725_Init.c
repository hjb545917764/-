#include "ov7725_Init.h"
#include "lptmr.h"
#include "common.h"
#include "dma.h"
#include "ov7725.h"
#include "gpio.h"
#include "uart.h"
#include "ftm.h"
#include "Drv_LCD.h"

extern uint32_t Left_QD_value;           //左编码器计数值
extern uint32_t Right_QD_value;          //右编码器计数值
extern uint32_t QDVALUE_GETFINSH;        //编码器测速状态

/*指定数组地址*/
uint8_t CCDBufferPool[OV7725_H*(OV7725_W/8)]; 

uint8_t img[OV7725_H*OV7725_W]={1};

int8_t center[OV7725_H];

/* 摄像头数据指针*/
uint8_t *CCDBuffer;

/*图像采集状态*/
volatile OV7725_Status ov7725_img_flag;

/* 引脚定义 */
#define BOARD_OV7725_PCLK_PORT      HW_GPIOC
#define BOARD_OV7725_PCLK_PIN       (16)          //使用PTA7引脚作为像素信号引脚         //7
#define BOARD_OV7725_VSYNC_PORT     HW_GPIOA
#define BOARD_OV7725_VSYNC_PIN      (17)         //使用PTA16引脚作为场信号引脚        //16
#define BOARD_OV7725_HREF_PORT      HW_GPIOA
#define BOARD_OV7725_HREF_PIN       (16)         //使用PTA17引脚作为行信号引脚          //17
#define BOARD_OV7725_DATA_OFFSET    (8) /* 摄像头数据采集引脚PTA8-PTA15 只能为 0 8 16 24 */

void ov7725_Port_Init(void);

/*************************************************
* 函数名称:SCCB_Init()
 * 简介： 通过SCCB写7725寄存器
 * 返回类型：0正常，1失败
 *************************************************/
int SCCB_Init()
{
    int r;
    r = ov7725_probe(); 
    if(r)
        return 1;
    r = ov7725_set_image_size(IMAGE_SIZE);
    if(r)
    {
        printf("OV7725 set image error\r\n");
        return 1;
    }
    return 0;
}

/*************************************************
 * 函数名称：void OV_ISR()
 * 简介： 场中断服务函数
 * 返回类型：无
 *************************************************/

void OV_ISR(uint32_t index)
{
    if(index & (1 << BOARD_OV7725_VSYNC_PIN))
    {
        //场中断需要判断是场结束还是场开始
        if(ov7725_img_flag == IMG_START)                   //需要开始采集图像
        {             
            ov7725_img_flag = IMG_GATHER;                  //标记图像采集中
            //disable_irq (PORTA_IRQn);
            PORTC ->ISFR = 1<<BOARD_OV7725_PCLK_PIN;       //清空PCLK标志位

            DMA_EnableRequest (HW_DMA_CH2 );                  //使能通道CHn 硬件请求
            PORTC ->ISFR = 1 <<  BOARD_OV7725_PCLK_PIN;            //清空PCLK标志位
            DMA_SetDestAddress (HW_DMA_CH2,(uint32_t)CCDBufferPool );    //恢复地址
					

		        disable_irq (PORTA_IRQn);                      //关闭PTA的中断
					

        }
        else if(ov7725_img_flag == IMG_FINISH)
        {
//            uint8_t i;
//            //获取编码器值
//            FTM_QD_GetData (HW_FTM1 ,&Left_QD_value ,&i);  //获得左编码器的计数值
//            FTM_QD_GetData (HW_FTM2 ,&Right_QD_value,&i);  //获得右编码器的计数值

//            FTM_QD_ClearCount (HW_FTM1 );                  //清除正交解码通道1的count值
//            FTM_QD_ClearCount (HW_FTM2 );
//            QDVALUE_GETFINSH =1;
        }
        else                                        //图像采集错误
        {
            disable_irq (PORTA_IRQn);                      //关闭PTA的中断
            ov7725_img_flag = IMG_FAIL;                    //标记图像采集失败
        }
    }
}

/*************************************************
 * 函数名称：void ov7725_DMA(void )
 * 简介： DMA中断服务函数
 * 返回类型：无
 *************************************************/
void ov7725_DMA(void )
{
    ov7725_img_flag = IMG_FINISH ;
    DMA_DisableRequest (HW_DMA_CH2 );
    DMA_IRQ_CLEAN (CAMERA_7_DMA_CH ) ;  //清除中断标志位
}

/*************************************************
 * 函数名称：void ov7725_Init()
 * 简介： ov7725初始化
 * 返回类型：无
 *************************************************/
void ov7725_Init()
{
    CCDBuffer =(uint8_t*)&CCDBufferPool;    
	//检测摄像头
    if(SCCB_Init())
    {
        printf("no device found!\r\n");
    }       
    ov7725_Port_Init();
    DelayMs (10);
}

/*************************************************
 * 函数名称：void ov7725_Port_Init()
 * 简介： ov7725管脚初始化（内部调用）
 * 返回类型：无
 *************************************************/
void ov7725_Port_Init()
{
     /* 初始化摄像头数据采集端口 */
    for(uint8_t i=0;i<8;i++)
    {
        GPIO_QuickInit(HW_GPIOC, BOARD_OV7725_DATA_OFFSET+i, kGPIO_Mode_IFT);
    }
		OV7725_PORT_INIT1111 &= 0xFFFF00FF;//将低PTE0~7置为0，输入状态   
    //0 Pin is configured as general-purpose input, for the GPIO function. 
    //1 Pin is configured as general-purpose output, for the GPIO function.
    /* 场中断  行中断 像素中断设置 */
    GPIO_QuickInit(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_Mode_IPU);       //PCLK设置为上拉输入
    GPIO_QuickInit(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_Mode_IPD);     //VSYNC设置为下拉输入
    
    /* install callback 安装回调函数*/
    GPIO_CallbackInstall(BOARD_OV7725_VSYNC_PORT, OV_ISR);
    
    //配置引脚下降沿中断功能   上升沿触发
    GPIO_ITDMAConfig(BOARD_OV7725_VSYNC_PORT, BOARD_OV7725_VSYNC_PIN, kGPIO_IT_RisingEdge, true);
    //配置引脚DMA触发功能     下降沿触发
    GPIO_ITDMAConfig(BOARD_OV7725_PCLK_PORT, BOARD_OV7725_PCLK_PIN, kGPIO_DMA_FallingEdge, true);
		
		
		//初始化状态
		ov7725_img_flag = IMG_FINISH;
	  disable_irq (PORTA_IRQn);                      //关闭PTA的中断
		
		
    
         //申请DMA结构
    DMA_InitTypeDef DMA_InitStruct1 = {0};
        
    //DMA配置
    DMA_InitStruct1.chl = CAMERA_7_DMA_CH ;     //HW_DMA_CH2
    DMA_InitStruct1.chlTriggerSource = PORTC_DMAREQ;
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct1.minorLoopByteCnt = 1;
    DMA_InitStruct1.majorLoopCnt = (OV7725_H *(OV7725_W/8));          //主循环次数
    
    DMA_InitStruct1.sAddr = (uint32_t)&PTC->PDIR + BOARD_OV7725_DATA_OFFSET/8;
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)CCDBuffer;
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;
    
    /* initialize DMA moudle 初始化DMA模块*/
    DMA_Init(&DMA_InitStruct1);  
    
    DMA_ITConfig (HW_DMA_CH2 ,kDMA_IT_Major ,true ); //设置DMA中断
    DMA_CallbackInstall (HW_DMA_CH2 ,ov7725_DMA);
    
    DMA_DisableRequest (HW_DMA_CH2 );
		
		
		
}
/*************************************************
 * 函数名称：void ov7725_Get_Imge(void)
 * 简介： 图像采集函数（采集到的数据存储在初始化中的数组上）
 * 返回类型：无
 *************************************************/
void ov7725_Get_Imge(void)
{
    ov7725_img_flag = IMG_START;                    //开始采集图像
    PORTA->ISFR = ~0;                               //写1请中断标志位
    enable_irq(PORTA_IRQn);                         //允许PTA的中断
    while(ov7725_img_flag != IMG_FINISH)            //等待图像采集完毕
    {
        if(ov7725_img_flag == IMG_FAIL)           //加入图像采集错误，则重新采集
        {
            ov7725_img_flag = IMG_START;            //开始采集图像
            PORTA->ISFR = ~0;                       //写1请中断标志位
            enable_irq(PORTA_IRQn);                 //允许PTA的中断
        }
    }
}


void img_extract(uint8_t *dst, uint8_t *src, uint32_t srclen)
{
    uint8_t colour[2] = {0, 1}; //0 和 1 分别对应的颜色
    //注：摄像头 0 表示 白色，1表示 黑色
    uint8_t tmpsrc;
    while(srclen --)
    {
        tmpsrc = *src++;
        *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
    }
}


