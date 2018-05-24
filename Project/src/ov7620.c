#include "ov7620.h"
#include "common.h"
#include "dma.h"
#include "uart.h"
#include "gpio.h"
#include "string.h"
#include "ov7620_back.h"

/*ͼ��ɼ�״̬*/
uint8_t ov7620_img_flag;


uint8_t CCD_Image1[OV7620_H][OV7620_W];//ͼ������
uint8_t CCD_Image2[OV7620_H][OV7620_W];//����ͼ�����ɼ���ż����ͼ������
//HС��Ӧ�·���WС��Ӧ�ҷ�

/***  	  H+   	***/
/*** 	  W+	W-		***/
/***   	  H-   	***/
//�л���ż��ͼ��
uint8_t Image_Change=0;

uint16_t Image_count=0;	//����ͷ�ɼ�ͼ��
uint16_t Error_count=0;	//�������


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

//OV7620��Ҫ�еļ���ֵ�����Ϊ�ɼ���������OV7620_H
//����ʱ���Ҫ�ɼ�ȫ���������ݣ������ܲɼ��ٶ����Ʋ���ȫ�ɡ�
uint16_t Image_Row = 0;		//DMA�ж�ָʾDMA�ɼ�����
uint16_t Curr_Row	=0;			//���ж�ָʾ��ǰ����ͷ����������

//DMA�ɼ���ʼ������
DMA_InitTypeDef DMA_InitStruct1;     


            
void OV7620_Init(void)
{
    /*	����ͷ�ܽ� E0	~	E7	*/
    for(uint8_t i=0;i<8;i++){
        GPIO_QuickInit(HW_GPIOE, i, kGPIO_Mode_IFT);//����ͷ���ݶ˿���������
    }
		
		
    OV7620_PORT_INIT &= 0xFFFFFF00;//����PTE0~7��Ϊ0������״̬
    //0 Pin is configured as general-purpose input, for the GPIO function. 
    //1 Pin is configured as general-purpose output, for the GPIO function.
    
		
		
    /* 	�����ж�����  */
    GPIO_QuickInit(OV7620_PCLK_PORT, OV7620_PCLK_PIN, kGPIO_Mode_IPD);       //PCLK����Ϊ��������
		/* 	���ж��ж�����  */
    GPIO_QuickInit(OV7620_VSYNC_PORT,OV7620_VSYNC_PIN, kGPIO_Mode_IPD);     //VSYNC����Ϊ��������
		/* 	���ж�����  */
		GPIO_QuickInit(OV7620_HREF_PORT,OV7620_HREF_PIN, kGPIO_Mode_IPD);     //HREF����Ϊ��������
    
    //��������DMA��������     �����ش���
    GPIO_ITDMAConfig(OV7620_PCLK_PORT, OV7620_PCLK_PIN, kGPIO_DMA_RisingEdge, true);
		
    //���������½����жϹ���   �½��ش���
    GPIO_ITDMAConfig(OV7620_VSYNC_PORT, OV7620_VSYNC_PIN, kGPIO_IT_FallingEdge, true);   
   
		//���ú������ر�
		disable_irq(OV7620_VSYN_GPIO_IRQ);  
		
		//���������½����жϹ���   �����ش���
    GPIO_ITDMAConfig(OV7620_HREF_PORT, OV7620_HREF_PIN, kGPIO_IT_RisingEdge, true);     
    
		disable_irq(OV7620_HREF_GPIO_IRQ);
    
		
		//ͼ��ɼ�״̬��ʼ������
    ov7620_img_flag = OV7620_STATE_COMPLETE;
    
		
		
		
    //DMA����
    DMA_InitStruct1.chl = CAMERA_DMA_CH ;     						//HW_DMA_CH0
    DMA_InitStruct1.chlTriggerSource = PORTE_DMAREQ;
    DMA_InitStruct1.minorLoopByteCnt = 1;               //��ѭ��(major loop)��ѭ��(minor loop)
    DMA_InitStruct1.majorLoopCnt = (OV7620_W);          //��ѭ������
    DMA_InitStruct1.triggerSourceMode = kDMA_TriggerSource_Normal;
    
		DMA_InitStruct1.sAddr = (uint32_t)&OV7620_DATA_VALU;//����ƫ����
    DMA_InitStruct1.sLastAddrAdj = 0;
    DMA_InitStruct1.sAddrOffset = 0;
    DMA_InitStruct1.sDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.sMod = kDMA_ModuloDisable;
    
    DMA_InitStruct1.dAddr = (uint32_t)CCD_Image1;
    DMA_InitStruct1.dLastAddrAdj = 0;
    DMA_InitStruct1.dAddrOffset = 1;
    DMA_InitStruct1.dDataWidth = kDMA_DataWidthBit_8;
    DMA_InitStruct1.dMod = kDMA_ModuloDisable;
    
    /* initialize DMA moudle ��ʼ��DMAģ��*/
    DMA_Init(&DMA_InitStruct1);  
    
		//����DMA�ж�
    DMA_ITConfig (CAMERA_DMA_CH ,kDMA_IT_Major ,true );
		
		//ע��ص�����
    DMA_CallbackInstall (CAMERA_DMA_CH ,ov7620_DMA);
    
		//�ر�DMAͨ��
    DMA_DisableRequest (CAMERA_DMA_CH );
		
}

/*************************************************
 * �������ƣ�void ov7620_DMA(void )
 * ��飺 DMA�жϷ�����
 * �������ͣ���
 *************************************************/
void ov7620_DMA(void)
{  
		/**< Clear Interrupt Request Register, offset: 0x1F */
	DMA0->CINT = DMA_CINT_CINT(CAMERA_DMA_CH);

	//���вɼ���ɱ�־������
	Image_Row ++;
	
  DMA_DisableRequest (CAMERA_DMA_CH);
  DMA_IRQ_CLEAN (CAMERA_DMA_CH) ;  
; 
	//����жϱ�־λ/**< Interrupt Request Register, offset: 0x24 */
}



/***********************************************************************************************
 ���ܣ�PORTE�ⲿ�ж���ڣ����ж�VSYN��PTE11
 �βΣ�0       
 ���أ�0
 ��⣺0
************************************************************************************************/
void PORTE_IRQHandler(void)
{

    if((PORTE->ISFR>>11)==1)
    {
			
			PORTE->ISFR = ~0;//����жϱ�־λ
			
			//����״̬�Ž���ͼ��ɼ�
			if(ov7620_img_flag	!=	OV7620_STATE_IDLE)
			{
				return;
			}
			
			Image_Row = 0;
			Curr_Row=0;
			
			//�л�ͼ��
			if(Image_Change == 0)
			{
				DMA_SetDestAddress(CAMERA_DMA_CH,(uint32_t)CCD_Image1);
			}
			else
			{
				DMA_SetDestAddress(CAMERA_DMA_CH,(uint32_t)CCD_Image2);
			} 
			

			
			disable_irq(OV7620_VSYN_GPIO_IRQ);           //�رճ��ж�
			
			enable_irq(OV7620_HREF_GPIO_IRQ);            //�����ж�
			
			ov7620_img_flag = OV7620_STATE_START;//���ڲɼ�
			
			DMA_IRQ_CLEAN (CAMERA_DMA_CH) ;  
    }
}

/***********************************************************************************************
 ���ܣ�PORTD�ⲿ�ж���ڣ����ж�
 �βΣ�0       
 ���أ�0
 ��⣺0
************************************************************************************************/
void PORTD_IRQHandler(void)
{

	if((PORTD->ISFR>>14)==1)
	{
		PORTD->ISFR = ~0;
		

		
		//DMA��ȡǰ100��
		
		//�ɼ���ͼ��
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
					ov7620_img_flag = OV7620_STATE_COMPLETE;//���ڲɼ�
					disable_irq(OV7620_HREF_GPIO_IRQ);	//�ر����ж�
				}
			}
		}
			Curr_Row++;
	}

	