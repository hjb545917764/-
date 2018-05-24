#ifndef _VARIABLE_H_
#define _VARIABLE_H_
#include "head.h"

extern uint16_t loop_count;	//Ѱ���ű�;��ѭ��ִ�д��������ڵ���
extern	uint8_t state;//״̬��ʶ


//�������
#define angle_mid		806		//�����λ
#define angle_left (angle_mid+70) 	//�������
#define angle_right (angle_mid-70)		//����Ҽ���

#define angle_lookfor_left angle_mid+65 //��Ѱ�ű�����
#define angle_lookfor_right angle_mid-65 	//��Ѱ�ű��Ҵ��

#define angle_turn_left	(angle_mid+40)	//�ű�б�й�����
#define angle_turn_right	(angle_mid-40)	//�ű�б�й��Ҵ��

#define angle_pass_left	(angle_mid+20)	//�ű���й�����
#define angle_pass_right	(angle_mid-20)	//�ű���й��Ҵ��
#define angle_evade_left	(angle_mid+50)	//�ű��������
#define angle_evade_right	(angle_mid-50)	//�ű�����Ҵ��

#define angle_Kd 4	//���PD���ֲ���
#define angle_K 0.43		//������ϵ��

#define angle_acc_left	(angle_mid+20)	//������ת����������
#define angle_acc_right	(angle_mid-20)	//������ת�������Ҽ���

extern	uint16_t	angle_now;	//���ʵ�ʴ��
extern 	uint16_t angle_fore;	//���ǰ���״̬
extern 	uint16_t	angle_target;	//���Ŀ����




//�������
extern uint16_t	motor_speed;		//��������ٶ�
extern uint16_t speed_left;			//���ֵ��ת��
extern uint16_t speed_right;		//���ֵ��ת��

#define speed_lookfor 200		//�ű���Ѱ�ٶ�
#define speed_go	 320			//�������ת�ٶȣ��ű����	
#define speed_turn 280			//�ű�б�й��ٶ�
#define speed_back -200			//�����ٶ�
//����������
#define cir_imp  5829		//һ��������
#define speed_k 1.1658	//�ٶȱ任����


//ͼ�����
#define Find_Cnt 5	//�ҵ��ű�ͼ����С������
#define Turn_H	76//�����ű�λ�ã�����б�й�
#define Turn_Cnt 100	//�ж��ű�б�й���С����
#define Pass_H	43	//�����ű���й�

extern	uint16_t	target_door;	//�ű���ֵ
//���ϲ���
#define EVADE_DELTA 45	//������ֵ̧��
#define EVADE_LIMIT 240	//������ֵ����
#define Evade_Cnt_up	1000	//���ϵ�������
#define Evade_Cnt_down	50	//���ϵ�������
#define Evade_door_up	113		//������ֵ���ޣ��˳��׵ذ��Ӱ��

//�ű����
extern	int target_H;	//Ŀ����
extern	int target_W;	//Ŀ����
extern uint32_t barrier_H;	//�ϰ���
extern uint32_t barrier_W;	//�ϰ���
extern	int target_flag;		//�ҵ��ű��־
extern	int turn_flag;		//�ű�б���й���־
extern	int pass_flag;		//�ű�����й���־
extern	int barrier_flag;	//�ű���ϱ�־

extern int cnt_lookfor;	//û���ҵ��Ƽ����������˳�50HZ���ţ�����20ms�����źŵ�����

//ǰ״̬��¼
extern 	int target_H_pre;	
extern	int target_W_pre;
extern	int target_flag_pre;		//�ҵ��ű�
extern	int turn_flag_pre;	//�ű��й�
extern	int pass_flag_pre;
extern 	int barrier_flag_pre;
extern	int count_pre;	//�ű��й�


//·��ѡ��
//1��0��
extern  int route_select[10];
extern  int turn_select[10];
//1��0��

extern int barrier_turn_select;	//����·��ѡ��
extern	int count_record;	//�ű��й�������¼



extern uint8_t Img_Mid[10];	//ͼ���������飬��������ͷ��ֵ

#endif
