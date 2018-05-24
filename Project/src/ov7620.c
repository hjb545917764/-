#include "ov7620.h"
#include "common.h"
#include "dma.h"
#include "uart.h"
#include "gpio.h"
#include "string.h"
#include "ov7620_back.h"

/*图像采集状态*/
uint8_t ov7620_img_flag;


uint8_t CCD_Image1[OV7620_H][OV7620_W];//图像数据
uint8_t CCD_Image2[OV7620_H][OV7620_W];//两幅图轮流采集奇偶场的图像数据
//H小对应下方，W小对应右方

/***  	  H+   	***/
/*** 	  W+	W-		***/
/***   	  H-   	***/
//切换奇偶场图像
uint8_t Image_Change=0;

uint16_t Image_count=0;	//摄像头采集图像
uint16_t Error_count=0;	//出错次数


const uint8_t Row_Select[100]=
{
	10,11,12,13,14,15,16,17,18,19,20,22,24,
	26,28,30,32,34,36,38,40,42,44,46,48,50,
	52,54,56,58,60,62,64,66,68,70,72,74,76,
	78,80,82,84,86,88,90,92,94,96,98,100,102,
	104,106,108,110,112,114,116,118,120,121,
	122,123,124,125,126,127,128,129,130,132,
	134,136,138,140,142,144,146,148,150,151,
	152,153,154,155,156,157,158,159,160,161,
	162,163,164,165,166,168,169,170
};

//OV7620需要行的计数值，最大为采集场的行数OV7620_H
//因有时侯不需要采集全部的行数据，或者受采集速度限制不能全采。
uint16_t Image_Row = 0;		//DMA中断指示DMA采集行数
uint16_t Curr_Row	=0;			//行中断指示当前摄像头数据所在行

//DMA采集初始化定义
DMA_InitTypeDef DMA_InitStruct1;     


            
void OV7620_Init(void)
{
    /*	摄像头管脚 E0	~	E7	*/
    for(uint8_t i=0;i<8;i++){
        GPIO_QuickInit(HW_GPIOE, i, kGPIO_Mode_IFT);//摄像头数据端口悬空输入
    }
		
		
    OV7620_PORT_INIT &= 0xFFFFFF00;//将低PTE0~7置为0，输入状态
    //0 Pin is configured as general-purpose input, for the GPIO function. 
    //1 Pin is configured as general-purpose output, for the GPIO function.
    
		
		
    /* 	像素中断设置  */
    GPIO_QuickInit(OV7620_PCLK_PORT, OV7620_PCLK_PIN, kGPIO_Mode_IPD);       //PCLK设置为下拉输入
		/* 	场中断中断设置  */
    GPIO_QuickInit(OV7620_VSYNC_PORT,OV7620_VSYNC_PIN, kGPIO_Mode_IPD);     //VSYNC设置为下拉输入
		/* 	行中断设置  */
		GPIO_QuickInit(OV7620_HREF_PORT,OV7620_HREF_PIN, kGPIO_Mode_IPD);     //HREF设置为下拉输入
    
    //配置引脚DMA触发功能     上升沿触发
    GPIO_ITDMAConfig(OV7620_PCLK_PORT, OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
		
    //配置引脚下降沿中断功能   下降沿触发
    GPIO_ITDMAConfig(OV7620_VSYNC_PORT, OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, true);   
   
		//配置后立即关闭
		disable_irq(OV7620_VSYN_GPIO_IRQ);  
		
		//配置引脚下降沿中断功能   上升沿触发
    GPIO_ITDMAConfig(OV7620_HREF_PORT, OV7620_HREF_PIN, kGPIO_IT_RisingEdge, true);     
    
		disable_irq(OV7620_HREF_GPIO_IRQ);
    
		
		//图像采集状态初始化设置
    ov7620_img_flag = OV7620_STATE_COMPLETE;
    
		
		
		
    //DMA配置
    DMA_InitStruct1.chl = CAMERA_DMA_CH ;     						//HW_DMA_CH0
    DMA_InitStruct1.chlTriggerSource = PORTE_DMAREQ;
    DMA_InitStruct1.minorLoopByteCnt = 1;               //主循环(major loop)副循环(minor loop)
    DMA_InitStruct1.majorLoopCnt = (OV7620_W);          //主循环次数
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    
		DMA_InitStruct1.sAddr = (uint32_t)&OV7620_DATA_VALU;//数据偏移量
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)CCD_Image1;
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;
    
    /* initialize DMA moudle 初始化DMA模块*/
    DMA_Init(&DMA_InitStruct1);  
    
		//设置DMA中断
    DMA_ITConfig (CAMERA_DMA_CH ,kDMA_IT_Major ,true );
		
		//注册回调函数
    DMA_CallbackInstall (CAMERA_DMA_CH ,ov7620_DMA);
    
		//关闭DMA通道
    DMA_DisableRequest (CAMERA_DMA_CH );
		
}

/*************************************************
 * 函数名称：void ov7620_DMA(void )
 * 简介： DMA中断服务函数
 * 返回类型：无
 *************************************************/
void ov7620_DMA(void)
{  
		/**< Clear Interrupt Request Register, offset: 0x1F */
	DMA0->CINT = DMA_CINT_CINT(CAMERA_DMA_CH);

	//该行采集完成标志，换行
	Image_Row ++;
	
  DMA_DisableRequest (CAMERA_DMA_CH);
  DMA_IRQ_CLEAN (CAMERA_DMA_CH) ;  
; 
	//清除中断标志位/**< Interrupt Request Register, offset: 0x24 */
}



/***********************************************************************************************
 功能：PORTE外部中断入口，场中断VSYN，PTE11
 形参：0       
 返回：0
 详解：0
************************************************************************************************/
void PORTE_IRQHandler(void)
{

    if((PORTE->ISFR>>11)==1)
    {
			
			PORTE->ISFR = ~0;//清除中断标志位
			
			//空闲状态才进入图像采集
			if(ov7620_img_flag	!=	OV7620_STATE_IDLE)
			{
				return;
			}
			
			Image_Row = 0;
			Curr_Row=0;
			
			//切换图像
			if(Image_Change == 0)
			{
				DMA_SetDestAddress(CAMERA_DMA_CH,(uint32_t)CCD_Image1);
			}
			else
			{
				DMA_SetDestAddress(CAMERA_DMA_CH,(uint32_t)CCD_Image2);
			} 
			

			
			disable_irq(OV7620_VSYN_GPIO_IRQ);           //关闭场中断
			
			enable_irq(OV7620_HREF_GPIO_IRQ);            //打开行中断
			
			ov7620_img_flag = OV7620_STATE_START;//正在采集
			
			DMA_IRQ_CLEAN (CAMERA_DMA_CH) ;  
    }
}

/***********************************************************************************************
 功能：PORTD外部中断入口，行中断
 形参：0       
 返回：0
 详解：0
************************************************************************************************/
void PORTD_IRQHandler(void)
{

	if((PORTD->ISFR>>14)==1)
	{
		PORTD->ISFR = ~0;
		

		
		//DMA读取前100行
		
		//采集完图像
		if( (Curr_Row == Row_Select[Image_Row])&& (Image_Row	<	OV7620_H ))
		{
			DMA_EnableRequest(CAMERA_DMA_CH);
			if(Image_Row == OV7620_H -1)
			{
				Image_count++;
			}
		}
		else{
				if(	Image_Row	< OV7620_H)
				{
					//Error_count++;
				}else
				{
					ov7620_img_flag = OV7620_STATE_COMPLETE;//正在采集
					disable_irq(OV7620_HREF_GPIO_IRQ);	//关闭行中断
				}
			}
		}
			Curr_Row++;
	}

	