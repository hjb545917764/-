#include "head.h"


uint16_t loop_count = 0;		//Ѱ���ű�;��ѭ��ִ�д��������ڵ���

uint8_t state	=	0;	//С������״̬��ʶ

//�������
uint16_t angle_now = angle_mid;		//���ʵ�ʴ��
uint16_t angle_fore = angle_mid;	//���ǰ���״̬
uint16_t angle_target = angle_mid;		//���Ŀ����


//�������
uint16_t  motor_speed = 200;		//��������ٶ�
uint16_t speed_left=200;	//���ֵ��ת��
uint16_t speed_right=200;	//���ֵ��ת��



//ͼ�����
uint16_t target_door = 240;	//�ű���ֵ


//�ű����
int target_H = 0;			//Ŀ����
int target_W = 0;			//Ŀ����
uint32_t barrier_H;		//�ϰ���
uint32_t barrier_W;		//�ϰ���
int target_flag = 0;		//�ҵ��ű��־
int turn_flag	=	0;			//�ű�б���й���־
int pass_flag	=	0;			//�ű�����й���־
int barrier_flag=0;			//�ű���ϱ�־

int cnt_lookfor=0;		//û���ҵ��Ƽ����������˳�50HZ���ţ�����20ms�����źŵ�����

//ǰ״̬��¼
int target_H_pre = 0;	
int target_W_pre = 0;
int target_flag_pre = 0;
int turn_flag_pre	=	0;	
int pass_flag_pre	=	0;
int barrier_flag_pre=0;
int count_pre	=	0;


//·��ѡ��
//1��0��
int route_select[10]={1,1,1,1,1,1,1,1,1,1};
int turn_select[10] ={1,1,0,1,0,1,1,0,1,0};
//1��0��
int barrier_turn_select=0; //����·��ѡ��
int count_record=0;	//�ű��й�������¼



//ͼ���������飬��������ͷ��ֵ
uint8_t Img_Mid[10]=
{
	160,//0-9
	163,//10-19
	165,//20-29
	168,//30-39
	163,//40-49
	165,//50-59
	158,//60-69
	155,//70-79
	159,//80-89
	158//90-99
};


