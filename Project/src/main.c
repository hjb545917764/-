#include "head.h"


uint32_t mode_selection=0; //����ģʽѡ��

uint32_t data_selection=0; //ģʽ״̬ѡ��




void Test_module( uint32_t data );


int main()
{
	Init();//��ʼ������
		
	
	//	ͨ����ȡ���뿪�ص�ֵ����������״̬
	mode_selection	= switch_mode();
	data_selection	= switch_data();
	
	while(1)
	{
			switch(mode_selection)
		{
			case 0:		//���Գ���ģ��
				Test_module(data_selection);
				break;
			case 1:		//��������
				Run_init();
				Run_car();
				break;
			default:
				Test_module(data_selection);
				break;
		}
	}
}





