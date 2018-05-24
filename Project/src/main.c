#include "head.h"


uint32_t mode_selection=0; //工作模式选择

uint32_t data_selection=0; //模式状态选择




void Test_module( uint32_t data );


int main()
{
	Init();//初始化配置
		
	
	//	通过读取拨码开关的值来决定工作状态
	mode_selection	= switch_mode();
	data_selection	= switch_data();
	
	while(1)
	{
			switch(mode_selection)
		{
			case 0:		//测试车辆模块
				Test_module(data_selection);
				break;
			case 1:		//赛车运行
				Run_init();
				Run_car();
				break;
			default:
				Test_module(data_selection);
				break;
		}
	}
}





