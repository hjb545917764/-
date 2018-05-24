#include "head.h"

uint16_t tt=0;//初始化1ms中断函数的变量

uint16_t delay_flag=DELAY_END;

void delay_ms( uint16_t ms )
{
	if (delay_flag==DELAY_END)
	{
		delay_flag=DELAY_START;
		tt=0;
	}
	while(tt<ms)
	{
	}
	delay_flag=DELAY_END;
}


//1ms中断回调函数 ,用于延时
void PIT1_HandlerU( void )
{
	if(delay_flag==DELAY_START)
	{
		tt++;
		if(tt==2500){
			tt=0;
		}
	}
}

