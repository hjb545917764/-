#ifndef _VARIABLE_H_
#define _VARIABLE_H_
#include "head.h"

extern uint16_t loop_count;	//寻找信标途中循环执行次数，用于倒车
extern	uint8_t state;//状态标识


//舵机参数
#define angle_mid		806		//舵机中位
#define angle_left (angle_mid+70) 	//舵机左极限
#define angle_right (angle_mid-70)		//舵机右极限

#define angle_lookfor_left angle_mid+65 //搜寻信标左打角
#define angle_lookfor_right angle_mid-65 	//搜寻信标右打角

#define angle_turn_left	(angle_mid+40)	//信标斜切过左打角
#define angle_turn_right	(angle_mid-40)	//信标斜切过右打角

#define angle_pass_left	(angle_mid+20)	//信标近切过左打角
#define angle_pass_right	(angle_mid-20)	//信标近切过右打角
#define angle_evade_left	(angle_mid+50)	//信标避障左打角
#define angle_evade_right	(angle_mid-50)	//信标避障右打角

#define angle_Kd 4	//舵机PD积分参数
#define angle_K 0.43		//舵机打角系数

#define angle_acc_left	(angle_mid+20)	//加速运转舵机打角左极限
#define angle_acc_right	(angle_mid-20)	//加速运转舵机打角右极限

extern	uint16_t	angle_now;	//舵机实际打角
extern 	uint16_t angle_fore;	//舵机前打角状态
extern 	uint16_t	angle_target;	//舵机目标打角




//电机参数
extern uint16_t	motor_speed;		//电机驱动速度
extern uint16_t speed_left;			//左轮电机转速
extern uint16_t speed_right;		//右轮电机转速

#define speed_lookfor 200		//信标搜寻速度
#define speed_go	 320			//最快速运转速度，信标加速	
#define speed_turn 280			//信标斜切过速度
#define speed_back -200			//倒车速度
//编码器参数
#define cir_imp  5829		//一米脉冲数
#define speed_k 1.1658	//速度变换常数


//图像参数
#define Find_Cnt 5	//找到信标图像最小点数
#define Turn_H	76//靠近信标位置，进行斜切过
#define Turn_Cnt 100	//判断信标斜切过最小点数
#define Pass_H	43	//进行信标近切过

extern	uint16_t	target_door;	//信标阈值
//避障参数
#define EVADE_DELTA 45	//避障阈值抬升
#define EVADE_LIMIT 240	//避障阈值限制
#define Evade_Cnt_up	1000	//避障点数上限
#define Evade_Cnt_down	50	//避障点数下限
#define Evade_door_up	113		//避障阈值上限，滤除白地板的影响

//信标参数
extern	int target_H;	//目标行
extern	int target_W;	//目标列
extern uint32_t barrier_H;	//障碍行
extern uint32_t barrier_W;	//障碍列
extern	int target_flag;		//找到信标标志
extern	int turn_flag;		//信标斜向切过标志
extern	int pass_flag;		//信标近处切过标志
extern	int barrier_flag;	//信标避障标志

extern int cnt_lookfor;	//没有找到灯计数，用以滤除50HZ干扰，消除20ms内无信号的误判

//前状态记录
extern 	int target_H_pre;	
extern	int target_W_pre;
extern	int target_flag_pre;		//找到信标
extern	int turn_flag_pre;	//信标切过
extern	int pass_flag_pre;
extern 	int barrier_flag_pre;
extern	int count_pre;	//信标切过


//路径选择
//1左0右
extern  int route_select[10];
extern  int turn_select[10];
//1左0右

extern int barrier_turn_select;	//避障路线选择
extern	int count_record;	//信标切过数量记录



extern uint8_t Img_Mid[10];	//图像中心数组，用以摄像头中值

#endif
