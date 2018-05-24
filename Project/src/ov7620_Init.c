#include "ov7620_Init.h"
#include "lptmr.h"
#include "common.h"
#include "dma.h"
#include "ov7620.h"
#include "gpio.h"
#include "uart.h"
#include "ftm.h"
#include "Drv_LCD.h"

extern uint32_t Left_QD_value;           //左编码器计数值
extern uint32_t Right_QD_value;          //右编码器计数值
extern uint32_t QDVALUE_GETFINSH;        //编码器测速状态
DMA_InitTypeDef DMA_InitStruct1;

/*指定数组地址*/
uint8_t CCDBufferPool[OV7620_H*(OV7620_W/8)]; 

/* 摄像头数据指针*/
uint8_t *CCDBuffer;

//有效行数  只采集 68 71 74 77行 等
uint8_t  CCD_Valid_Line[CCD_IMAGE_H];

/*图像采集状态*/
volatile OV7725_Status ov7725_img_flag;

/* 引脚定义 */
#define BOARD_OV7620_PCLK_PORT      HW_GPIOC
#define BOARD_OV7620_PCLK_PIN       (3)          //使用PTA7引脚作为像素信号引脚         //7
#define BOARD_OV7620_VSYNC_PORT     HW_GPIOC
#define BOARD_OV7620_VSYNC_PIN      (11)         //使用PTA16引脚作为场信号引脚        //16
#define BOARD_OV7620_HREF_PORT      HW_GPIOE
#define BOARD_OV7620_HREF_PIN       (8)         //使用PTA17引脚作为行信号引脚          //17
#define BOARD_OV7620_DATA_OFFSET    (8) /* 摄像头数据采集引脚PTA8-PTA15 只能为 0 8 16 24 */

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
    if(index & (1 << BOARD_OV7620_VSYNC_PIN))
    {
        //场中断需要判断是场结束还是场开始
        if(ov7725_img_flag == IMG_START)                   //需要开始采集图像
        {             
            ov7725_img_flag = IMG_GATHER;                  //标记图像采集中
            //disable_irq (PORTA_IRQn);
            PORTC ->ISFR = 1<<BOARD_OV7620_PCLK_PIN;       //清空PCLK标志位

            DMA_EnableRequest (HW_DMA_CH2 );                  //使能通道CHn 硬件请求
            PORTC ->ISFR = 1 <<  BOARD_OV7620_PCLK_PIN;            //清空PCLK标志位
            DMA_SetDestAddress (HW_DMA_CH2,(uint32_t)CCDBufferPool );    //恢复地址

        }
        else if(ov7725_img_flag == IMG_FINISH)
        {
            uint8_t i;
            //获取编码器值
            FTM_QD_GetData (HW_FTM1 ,&Left_QD_value ,&i);  //获得左编码器的计数值
            FTM_QD_GetData (HW_FTM2 ,&Right_QD_value,&i);  //获得右编码器的计数值

            FTM_QD_ClearCount (HW_FTM1 );                  //清除正交解码通道1的count值
            FTM_QD_ClearCount (HW_FTM2 );
            QDVALUE_GETFINSH =1;
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
    DMA_IRQ_CLEAN (CAMERA_DMA_CH ) ;  //清除中断标志位
}

/*************************************************
 * 函数名称：void ov7725_Init()
 * 简介： ov7725初始化
 * 返回类型：无
 *************************************************/
//初始化OV7620模块
void OV7620_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct1;
	uint32_t i = 0;
	//有效行数 
	for(i=0;i<sizeof(CCD_Valid_Line);i++)
	{
		CCD_Valid_Line[i] = i*2; //*2这个参数很重要，决定图像高度分辨 
	}
	//空闲状态
	OV7620Dev.State = OV7620_STATE_IDLE;
	//摄像头数据 D0-D7
	for(i=0;i<8;i++)
	{
		GPIO_InitStruct1.pinx = i;
		GPIO_InitStruct1.GPIO_InitState = Bit_SET;
		GPIO_InitStruct1.GPIO_IRQMode = kGPIO_IT_DISABLE;
		GPIO_InitStruct1.mode = kGPIO_Mode_IFT;
		GPIO_InitStruct1.instance = OV7620_DATAPORT;
		GPIO_Init(&GPIO_InitStruct1);
	}
	
	//像素中断 PCLK
	GPIO_InitStruct1.pinx = OV7620_PCLK_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_SET;
	GPIO_InitStruct1.GPIO_IRQMode = kGPIO_DMA_RisingEdge;
	GPIO_InitStruct1.mode = kGPIO_Mode_IPD;
	GPIO_InitStruct1.instance = OV7620_PCLK_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	//行中断 HREF
	GPIO_InitStruct1.pinx = OV7620_HREF_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_SET;
	GPIO_InitStruct1.GPIO_IRQMode = kGPIO_IT_RisingEdge;
	GPIO_InitStruct1.mode = kGPIO_Mode_IPD;
	GPIO_InitStruct1.instance = OV7620_HREF_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	// 场中断 VSYNC
	GPIO_InitStruct1.pinx = OV7620_VSYNC_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_SET;
	GPIO_InitStruct1.GPIO_IRQMode = kGPIO_IT_RisingEdge;
	GPIO_InitStruct1.mode = kGPIO_Mode_IPD;
	GPIO_InitStruct1.instance = OV7620_VSYNC_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	
	NVIC_EnableIRQ(PORTB_IRQn);
  //配置DMA
  DMA_InitStruct1.chl          = HW_DMA_CH1;         //DMA 1通道
  DMA_InitStruct1.chlTriggerSource   =PORTC_DMAREQ;    //C端口(PCLK) 上升呀触发    
	DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
  DMA_InitStruct1.minorLoopByteCnt = 2;               //传输次数 超过摄像头每行像素数即可
	DMA_InitStruct1.majorLoopCnt = 170;                   //每次传输3个字节       

  DMA_InitStruct1.sAddr =(uint32_t)&PTD->PDIR;//摄像头端口接D0-D7
	DMA_InitStruct1.sLastAddrAdj = 0;                  //地址不增加
	DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;       //8BIT数据
	DMA_InitStruct1.sAddrOffset = 0;

  DMA_InitStruct1.dAddr =(uint32_t)CCDBuffer;  //DMA 内存
	DMA_InitStruct1.dLastAddrAdj = 0;
	DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
	DMA_InitStruct1.dAddrOffset = 1;                   //每次传输 +1个字节
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
        if(ov7725_img_flag == IMG_FAIL)             //加入图像采集错误，则重新采集
        {
            ov7725_img_flag = IMG_START;            //开始采集图像
            PORTA->ISFR = ~0;                       //写1请中断标志位
            enable_irq(PORTA_IRQn);                 //允许PTA的中断
        }
    }
}




