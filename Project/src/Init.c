#include "head.h"




void Init()
 {    
    DelayInit();//初始化延时程序
	 
    /* 正常运行指示灯初始化  管脚：D15 E26  */
    GPIO_QuickInit(HW_GPIOD,15, kGPIO_Mode_OPP); 
    GPIO_QuickInit(HW_GPIOE ,26, kGPIO_Mode_OPP); 

	 
    /* UART端口初始化   管脚：E25  E24    波特率：115200*/
    //k60核心板接口E25 E24 管脚 串口波特率 115200 UART4
    UART_QuickInit(UART4_RX_PE25_TX_PE24, 115200);
		 
	 
	 
    /* IO口模拟IIC端口初始化     管脚：C6   C7*/   
		/* 如需更改端口请去io2iic.h头文件中修改 */
    GPIO_QuickInit(IIC_SCL_PORT , IIC_SCL_PIN , kGPIO_Mode_OPP);
    GPIO_QuickInit(IIC_SDA_PORT , IIC_SDA_PIN , kGPIO_Mode_OPP);
    
    /* 初始化正交解码，端口号分别为左：PA12,PA13;右：PB18,PB19*/      
    FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13, kFTM_QD_NormalPolarity, kQD_CountDirectionEncoding);
    FTM_QD_QuickInit(FTM2_QD_PHA_PB18_PHB_PB19, kFTM_QD_NormalPolarity, kQD_CountDirectionEncoding); 
	 
     /* 拨码开关初始化  管脚：  B0 ~ B7 */
    GPIO_QuickInit(HW_GPIOB, 0, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 1, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 2, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 3, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 4, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 5, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 6, kGPIO_Mode_IPU);
    GPIO_QuickInit(HW_GPIOB, 7, kGPIO_Mode_IPU);
       
    /* 舵机初始化 */
    CMT_PWM_QuickInit(55,0);
		
    /* 设置舵机初使占空比，范围0-10000分别表示0-100% */
    CMT_PWM_ChangDuty (angle_mid);
      
    /* FTM快速初始化  管脚：	C1	~	C4	电机PWM初始化配置*/
		
    FTM_PWM_QuickInit(FTM0_CH0_PC01, kPWM_EdgeAligned ,10000);     /* 设置PC03为电机输出端口，频率设定为5000Hz*/
    FTM_PWM_QuickInit(FTM0_CH1_PC02, kPWM_EdgeAligned ,10000);     /* 设置PD04为电机输出端口，频率设定为5000Hz*/
    FTM_PWM_QuickInit(FTM0_CH2_PC03, kPWM_EdgeAligned ,10000);     /* 设置PC03为电机输出端口，频率设定为5000Hz*/
    FTM_PWM_QuickInit(FTM0_CH3_PC04, kPWM_EdgeAligned ,10000);     /* 设置PD04为电机输出端口，频率设定为5000Hz*/
    
		
		
    /* 设置FTM初使占空比，范围0-10000分别表示0-100% */
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH0, 0); 
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH1, 0);
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH2, 0); 
    FTM_PWM_ChangeDuty(HW_FTM0, HW_FTM_CH3, 0);

		//初始化pit模块的0通道，产生20ms的中断
    PIT_QuickInit(HW_PIT_CH0, 20000);
    PIT_CallbackInstall(HW_PIT_CH0, QD_value ); //注册通道0的回调函数为：QD_value（）
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, ENABLE); //开起模块0通道中断 

    //初始化pit模块的1通道，产生1ms的中断,用于延时
    PIT_QuickInit(HW_PIT_CH1, 1000);
    PIT_CallbackInstall(HW_PIT_CH1, PIT1_HandlerU ); //注册通道1的回调函数为：PIT1_HandlerU（）		
    PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF, ENABLE); //开起模块1通道中断 
		
		
		/*初始化pit模块的2通道，产生20ms中断*/
		PIT_QuickInit(HW_PIT_CH2, 20000);
    PIT_CallbackInstall(HW_PIT_CH2, Motor_PID_Output ); //注册通道0的回调函数为：QD_value（）
    PIT_ITDMAConfig(HW_PIT_CH2, kPIT_IT_TOF, ENABLE); //开起模块0通道中断 
}
 



