#include "head.h"


uint16_t loop_count = 0;		//寻找信标途中循环执行次数，用于倒车

uint8_t state	=	0;	//小车运行状态标识

//舵机参数
uint16_t angle_now = angle_mid;		//舵机实际打角
uint16_t angle_fore = angle_mid;	//舵机前打角状态
uint16_t angle_target = angle_mid;		//舵机目标打角


//电机参数
uint16_t  motor_speed = 200;		//电机驱动速度
uint16_t speed_left=200;	//左轮电机转速
uint16_t speed_right=200;	//右轮电机转速



//图像参数
uint16_t target_door = 240;	//信标阈值


//信标参数
int target_H = 0;			//目标行
int target_W = 0;			//目标列
uint32_t barrier_H;		//障碍行
uint32_t barrier_W;		//障碍列
int target_flag = 0;		//找到信标标志
int turn_flag	=	0;			//信标斜向切过标志
int pass_flag	=	0;			//信标近处切过标志
int barrier_flag=0;			//信标避障标志

int cnt_lookfor=0;		//没有找到灯计数，用以滤除50HZ干扰，消除20ms内无信号的误判

//前状态记录
int target_H_pre = 0;	
int target_W_pre = 0;
int target_flag_pre = 0;
int turn_flag_pre	=	0;	
int pass_flag_pre	=	0;
int barrier_flag_pre=0;
int count_pre	=	0;


//路径选择
//1左0右
int route_select[10]={1,1,1,1,1,1,1,1,1,1};
int turn_select[10] ={1,1,0,1,0,1,1,0,1,0};
//1左0右
int barrier_turn_select=0; //避障路线选择
int count_record=0;	//信标切过数量记录



//图像中心数组，用以摄像头中值
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


