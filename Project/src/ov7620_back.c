#include "ov7620_back.h"
#include "ov7620.h"
#include "common.h"
#include "dma.h"
#include "uart.h"
#include "gpio.h"
#include "string.h"

/*图像采集状态*/
uint8_t ov7620_img_flag2;

uint8_t CCD_Image12[OV7620_H2][OV7620_W2];//图像数据

//OV7620当前行的计数值
uint16_t currRow2 = 0;
//OV7620需要行的计数值，最大为采集场的行数OV7620_H
//因有时侯不需要采集全部的行数据，或者受采集速度限制不能全采。
uint16_t Image_Row2 = 0;

//行标志
//uint8_t Image_RowFlag2[OV7620_H2];
uint8_t const OV7620_SelectedROWS2[OV7620_H2] =
			{
//				2,4,6,8,10,12,14,16,18,20,
//				22,24,26,28,30,32,34,36,38,40,
//				42,44,46,48,50,52,54,56,58,60,
//				62,64,66,68,70,72,74,76,78,80,
//				82,84,86,88,90,92,94,96,98,100,
//				102,104,106,108,110,112,114,116,118,120,
//				122,124,126,128,130,132,134,136,138,140,
//				142,144,146,148,150,152,154,156,158,160,
//				162,164,166,168,170,172,174,176,178,180,
//				182,184,186,188,190,192,194,196,198,200
//				
				2,4,6,8,10,12,14,16,18,20,
				22,24,26,28,30,32,34,36,38,40,
				42,44,46,48,50,52,54,56,58,60,
				62,64,66,68,70,72,74,76,78,80,
				82,84,86,87,88,90,92,94,95,96,97,98,99,100,101,
				102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
				122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,
				142,143,144,145,146
//				202,204,206,208,210,212,214,216,218,220,
//				222,224,226,228,230,232,234,236,238,239
				
			};

DMA_InitTypeDef DMA_InitStruct2;                 
            
void OV7620_back_Init(void)
{
    //摄像头数据D0-D7
    for(uint8_t i=0;i<8;i++){
        GPIO_QuickInit(HW_GPIOC, i+BOARD_OV7620_DATA_OFFSET, kGPIO_Mode_IFT);//摄像头数据端口悬空输入
    }
    OV7620_back_PORT_INIT &= 0xFFFF00FF;//将低PTC8~15置为0，输入状态
    //0 Pin is configured as general-purpose input, for the GPIO function. 
    //1 Pin is configured as general-purpose output, for the GPIO function.
    
    //后置摄像头
    GPIO_QuickInit(OV7620_back_PCLK_PORT, OV7620_back_PCLK_PIN, kGPIO_Mode_IPD);       //PCLK设置为下拉输入
    GPIO_QuickInit(OV7620_back_VSYNC_PORT,OV7620_back_VSYNC_PIN, kGPIO_Mode_IPD);     //VSYNC设置为下拉输入
	GPIO_QuickInit(OV7620_back_HREF_PORT,OV7620_back_HREF_PIN, kGPIO_Mode_IPD);     //HREF设置为下拉输入
        
    //后置摄像头中断设置
    GPIO_ITDMAConfig(OV7620_back_PCLK_PORT, OV7620_back_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
    GPIO_ITDMAConfig(OV7620_back_VSYNC_PORT, OV7620_back_VSYNC_PIN, kGPIO_IT_RisingEdge, true);     disable_irq(OV7620_back_VSYN_GPIO_IRQ);
    GPIO_ITDMAConfig(OV7620_back_HREF_PORT, OV7620_back_HREF_PIN, kGPIO_IT_RisingEdge, true);       disable_irq(OV7620_back_HREF_GPIO_IRQ);
    
//    //后置摄像头
//    enable_irq(OV7620_back_VSYN_GPIO_IRQ);
//	disable_irq(OV7620_back_VSYN_GPIO_IRQ);
//	enable_irq(OV7620_back_HREF_GPIO_IRQ);
//	disable_irq(OV7620_back_HREF_GPIO_IRQ);
    
//    memset(&Image_RowFlag2,0,sizeof(Image_RowFlag2));
    
    //DMA配置
    DMA_InitStruct2.chl = CAMERA_back_DMA_CH ;     //HW_DMA_CH1
    DMA_InitStruct2.chlTriggerSource = PORTD_DMAREQ;
    DMA_InitStruct2.minorLoopByteCnt = 1;
    DMA_InitStruct2.majorLoopCnt = (OV7620_W2);          //主循环次数
    DMA_InitStruct2.triggerSourceMode = kDMA_TriggerSource_Normal;
    DMA_InitStruct2.sAddr = (uint32_t)&OV7620_back_DATA_VALU+ BOARD_OV7620_DATA_OFFSET/8;//数据偏移量
    DMA_InitStruct2.sLastAddrAdj = 0;
    DMA_InitStruct2.sAddrOffset = 0;
    DMA_InitStruct2.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct2.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct2.dAddr = (uint32_t)CCD_Image12;
    DMA_InitStruct2.dLastAddrAdj = 0;
    DMA_InitStruct2.dAddrOffset = 1;
    DMA_InitStruct2.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct2.dMod = kDMA_ModuloDisable;
    
    /* initialize DMA moudle 初始化DMA模块*/
    DMA_Init(&DMA_InitStruct2);  
    
    DMA_ITConfig (CAMERA_back_DMA_CH ,kDMA_IT_Major ,true ); //设置DMA中断
    DMA_CallbackInstall (CAMERA_back_DMA_CH ,ov7620_DMA_back);
    
    DMA_DisableRequest (CAMERA_back_DMA_CH );
		
//	enable_irq(OV7620_back_VSYN_GPIO_IRQ);
//    disable_irq(OV7620_back_VSYN_GPIO_IRQ);
//    disable_irq(OV7620_back_HREF_GPIO_IRQ);
//    disable_irq(OV7620_back_PCLK_eDMA_IRQ);
//    PORTA->ISFR = ~0;
//    PORTC->ISFR = ~0;
//    PORTD->ISFR = ~0;
	
		//空闲状态
	ov7620_img_flag2 = OV7620_STATE_IDLE2;
}

/*************************************************
 * 函数名称：void ov7620_DMA_back(void )
 * 简介： DMA中断服务函数
 * 返回类型：无
 *************************************************/
void ov7620_DMA_back(void)
{  
	DMA0->CINT = DMA_CINT_CINT(CAMERA_back_DMA_CH);/**< Clear Interrupt Request Register, offset: 0x1F */
//	Image_RowFlag2[Image_Row2] = 1;//该行采集完成标志，
	Image_Row2 ++;//完成一行数据DMA读取
    DMA_DisableRequest (CAMERA_back_DMA_CH);
    DMA_IRQ_CLEAN (CAMERA_back_DMA_CH) ;  //清除中断标志位/**< Interrupt Request Register, offset: 0x24 */
}

//PORTC外部中断入口，后置摄像头场中断VSYN，PTE16
//void PORTC_IRQHandler(void)
//{
////	uint8_t i = 31;
////	for(i=31;i>0;i--)  //判断哪个引脚的中断发生，i表示哪个引脚发生的中断
////	{
////	  if((PORTC->ISFR>>i)==1)
////			break;
////	}
////	switch(i)
////	{ //中断处理

////	  case OV7620_back_VSYNC_PIN://场中断
////				VIsr_back();
////			break;
////    // ...		
////	  default : break;
////	}
////    PORTC->ISFR|=PORT_ISFR_ISF_MASK;   //清除标志位 PORT_ISFR_ISF_MASK 0xFFFFFFFFu
//	 DelayUs (1);
//    if((PORTC->ISFR>>16)==1)
//	{
////		GPIO_ToggleBit (HW_GPIOA,17);//进入场中断时，行中断触发没响应，所以翻转触发电平
//        PORTC->ISFR = ~0;//清除中断标志位
//        if(ov7620_img_flag2!=OV7620_STATE_IDLE2)//空闲状态才进入采集
//        {
//            return;
//        }
//        currRow2 = 0;
//        Image_Row2 = 0;
//        disable_irq(OV7620_back_VSYN_GPIO_IRQ);           //关闭场中断
//        enable_irq(OV7620_back_HREF_GPIO_IRQ);            //打开行中断
////        DMA_EnableRequest (CAMERA_back_DMA_CH);							 //使能DMA中断
//        DMA_SetDestAddress(CAMERA_back_DMA_CH,(uint32_t)CCD_Image12);
//        ov7620_img_flag2 = OV7620_STATE_START2;//正在采集
//	}
//}


////后置摄像头场中断
//void VIsr_back(void)
//{
//    GPIO_ToggleBit (HW_GPIOA,17);//进入场中断时，行中断触发没响应，所以翻转触发电平
//    PORTC->ISFR = ~0;//清除中断标志位
//    if(ov7620_img_flag2!=OV7620_STATE_IDLE2)//空闲状态才进入采集
//    {
//        return;
//    }
//    currRow2 = 0;
//    Image_Row2 = 0;
//    disable_irq(OV7620_back_VSYN_GPIO_IRQ);           //关闭场中断
//    enable_irq(OV7620_back_HREF_GPIO_IRQ);            //打开行中断
//    DMA_EnableRequest (CAMERA_back_DMA_CH);							 //使能DMA中断
//    DMA_SetDestAddress(CAMERA_back_DMA_CH,(uint32_t)CCD_Image12);
//    ov7620_img_flag2 = OV7620_STATE_START2;//正在采集
//}

//void PORTA_IRQHandler(void)
//{
//	uint8_t i = 31;
//	for(i=31;i>0;i--)  //判断哪个引脚的中断发生，i表示哪个引脚发生的中断
//	{
//	  if((PORTA->ISFR>>i)==1)
//			break;
//	}
//	switch(i)
//	{ //中断处理
//	  case OV7620_HREF_PIN: //行中断
//			HIsr();break;
//      case OV7620_back_HREF_PIN://后置摄像头行中断
//            HIsr_back();break;
//		default : break;
//	}
//    PORTA->ISFR|=PORT_ISFR_ISF_MASK;   //清除标志位
//}

//void PORTA_IRQHandler(void)
//{
////    uint8_t i = 31;
////	for(i=31;i>0;i--)  //判断哪个引脚的中断发生，i表示哪个引脚发生的中断
////	{
////	  if((PORTA->ISFR>>i)==1)
////			break;
////	}
////	switch(i)
////	{ //中断处理
////	  case OV7620_HREF_PIN: //行中断
////			HIsr();break;
////      case OV7620_back_HREF_PIN://后置摄像头行中断
////            HIsr_back();break;
////		default : break;
////	}
////    PORTA->ISFR|=PORT_ISFR_ISF_MASK;   //清除标志位
//    if((PORTA->ISFR>>17)==1)
//    {
//		PORTA->ISFR = ~0;
//        currRow2 ++;
//        if(currRow2 == OV7620_SelectedROWS2[Image_Row2])				//如果当前行数据应该采集
//        {
//            //启动DMA传输
//            DMA_EnableRequest(CAMERA_back_DMA_CH);
//        }
//        if(Image_Row2>=OV7620_H2)//一场完成，关闭行中断
//        {
//            disable_irq(OV7620_back_HREF_GPIO_IRQ);   

//            DMA_DisableRequest(CAMERA_back_DMA_CH);//关闭DMA
//            ov7620_img_flag2 = OV7620_STATE_COMPLETE2;//一场采集完成	
//        }
//    }
//}

//void HIsr_back(void) //调用行中段回调函数
//{
//	PORTA->ISFR = ~0;
//	currRow2 ++;
//	if(currRow2 == OV7620_SelectedROWS2[Image_Row2])				//如果当前行数据应该采集
//  {
//		//启动DMA传输
//		DMA_EnableRequest(CAMERA_back_DMA_CH);
//	}
//	if(Image_Row2>=OV7620_H2)//一场完成，关闭行中断
//    {
//        disable_irq(OV7620_back_HREF_GPIO_IRQ);   

//        DMA_DisableRequest(CAMERA_back_DMA_CH);//关闭DMA
//		ov7620_img_flag2 = OV7620_STATE_COMPLETE2;//一场采集完成	
//	}
//}
