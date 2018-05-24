#include "head.h"




void Init()
 {    
    DelayInit();//��ʼ����ʱ����
	 
    /* ��������ָʾ�Ƴ�ʼ��  �ܽţ�D15 E26  */
    GPIO_QuickInit(HW_GPIOD,15, kGPIO_Mode_OPP); 
    GPIO_QuickInit(HW_GPIOE ,26, kGPIO_Mode_OPP); 

	 
    /* UART�˿ڳ�ʼ��   �ܽţ�E25  E24    �����ʣ�115200*/
    //k60���İ�ӿ�E25 E24 �ܽ� ���ڲ����� 115200 UART4
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 115200);
		 
	 
	 
    /* IO��ģ��IIC�˿ڳ�ʼ��     �ܽţ�C6   C7*/   
		/* ������Ķ˿���ȥio2iic.hͷ�ļ����޸� */
    GPIO_QuickInit(IIC_SCL_PORT , IIC_SCL_PIN , kGPIO_Mode_OPP);
    GPIO_QuickInit(IIC_SDA_PORT , IIC_SDA_PIN , kGPIO_Mode_OPP);
    
    /* ��ʼ���������룬�˿ںŷֱ�Ϊ��PA12,PA13;�ң�PB18,PB19*/      
    FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13, kFTM_QD_NormalPolarity, kQD_CountDirectionEncoding);
    FTM_QD_QuickInit(FTM2_QD_PHA_PB18_PHB_PB19, kFTM_QD_NormalPolarity, kQD_CountDirectionEncoding); 
	 
     /* ���뿪�س�ʼ��  �ܽţ�  B0 ~ B7 */
    GPIO_QuickInit(HW_GPIOB, 0, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 1, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 2, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 3, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 4, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 5, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 6, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 7, kGPIO_Mode_IPU);
       
    /* �����ʼ�� */
    CMT_PWM_QuickInit(55,0);
		
    /* ���ö����ʹռ�ձȣ���Χ0-10000�ֱ��ʾ0-100% */
    CMT_PWM_ChangDuty (angle_mid);
      
    /* FTM���ٳ�ʼ��  �ܽţ�	C1	~	C4	���PWM��ʼ������*/
		
    FTM_PWM_QuickInit(FTM0_CH0_PC01, kPWM_EdgeAligned ,10000);     /* ����PC03Ϊ�������˿ڣ�Ƶ���趨Ϊ5000Hz*/
    FTM_PWM_QuickInit(FTM0_CH1_PC02, kPWM_EdgeAligned ,10000);     /* ����PD04Ϊ�������˿ڣ�Ƶ���趨Ϊ5000Hz*/
    FTM_PWM_QuickInit(FTM0_CH2_PC03, kPWM_EdgeAligned ,10000);     /* ����PC03Ϊ�������˿ڣ�Ƶ���趨Ϊ5000Hz*/
    FTM_PWM_QuickInit(FTM0_CH3_PC04, kPWM_EdgeAligned ,10000);     /* ����PD04Ϊ�������˿ڣ�Ƶ���趨Ϊ5000Hz*/
    
		
		
    /* ����FTM��ʹռ�ձȣ���Χ0-10000�ֱ��ʾ0-100% */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 0); 
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH1, 0);
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH2, 0); 
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, 0);

		//��ʼ��pitģ���0ͨ��������20ms���ж�
    PIT_QuickInit(HW_PIT_CH0, 20000);
    PIT_CallbackInstall(HW_PIT_CH0, QD_value ); //ע��ͨ��0�Ļص�����Ϊ��QD_value����
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, ENABLE); //����ģ��0ͨ���ж� 

    //��ʼ��pitģ���1ͨ��������1ms���ж�,������ʱ
    PIT_QuickInit(HW_PIT_CH1, 1000);
    PIT_CallbackInstall(HW_PIT_CH1, PIT1_HandlerU ); //ע��ͨ��1�Ļص�����Ϊ��PIT1_HandlerU����		
    PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, ENABLE); //����ģ��1ͨ���ж� 
		
		
		/*��ʼ��pitģ���2ͨ��������20ms�ж�*/
		PIT_QuickInit(HW_PIT_CH2, 20000);
    PIT_CallbackInstall(HW_PIT_CH2, Motor_PID_Output ); //ע��ͨ��0�Ļص�����Ϊ��QD_value����
    PIT_ITDMAConfig(HW_PIT_CH2, kPIT_IT_TOF, ENABLE); //����ģ��0ͨ���ж� 
}
 



