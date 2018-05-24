#include "cmt.h"
#include "common.h"
#include "gpio.h"
#include <stdint.h>

static uint16_t FullDuty; 

void CMT_PWM_QuickInit(uint16_t fre,uint16_t duty)
{
    uint32_t clock;
    /* 获取系统板时钟频率 */
    CLOCK_GetClockFrequency(kBusClock, &clock);
    /* CMT时钟使能 */
    SIM->SCGC4 |=SIM_SCGC4_CMT_MASK ;
    /* 引脚复用 */
    PORT_PinMuxConfig (HW_GPIOD ,7,kPinAlt2);
    /* 总线时钟分频，产生IF时钟信号 */
    CMT->PPS =CMT_PPS_PPSDIV(10);
    /* CMT时钟 IF时钟信号分频 */
    CMT->MSC =CMT_MSC_CMTDIV (0);
    /* 根据频率计算计数值 */
    FullDuty =clock /(80*fre);
    
    CMT->CMD1 =(uint8_t)((duty >>8)&CMT_CMD1_MB_MASK);
    CMT->CMD2 =(uint8_t)(duty&CMT_CMD2_MB_MASK);
    CMT->CMD3 =(uint8_t )(((FullDuty -duty)>>8)&CMT_CMD3_SB_MASK);
    CMT->CMD4 =(uint8_t )((FullDuty -duty)&CMT_CMD4_SB_MASK);
    
    /* 基带模式使能 */
    CMT->MSC &= ~CMT_MSC_FSK_MASK;
    CMT->MSC |=CMT_MSC_BASE_MASK; 
    /* 载波调制波使能 */
    CMT->MSC |=CMT_MSC_MCGEN_MASK ;   
    /* 翻转电平 */
    CMT->OC |=CMT_OC_CMTPOL_MASK ;
    /*  输出使能 */
    CMT->OC |=CMT_OC_IROPEN_MASK ;    
}

void CMT_PWM_ChangDuty(uint16_t duty)
{
    duty=duty*FullDuty/10000;
    CMT->CMD1 =(uint8_t)((duty >>8)&CMT_CMD1_MB_MASK);
    CMT->CMD2 =(uint8_t)(duty&CMT_CMD2_MB_MASK);
    CMT->CMD3 =(uint8_t )(((FullDuty -duty)>>8)&CMT_CMD3_SB_MASK);
    CMT->CMD4 =(uint8_t )((FullDuty -duty)&CMT_CMD4_SB_MASK);
}
