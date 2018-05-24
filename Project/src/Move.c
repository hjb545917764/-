#include "head.h"

uint8_t speed_cnt=0;
//搜索信标

uint16_t speed_tran(uint16_t speed);//速度变换

void Lookfor()
{
	
	//改变舵机角度，进行转圈
	if(route_select[count_record] == 1)
	{
		angle_target	= angle_lookfor_left;	//左转
		motor_speed =	speed_lookfor;		//减速
		speed_left=speed_tran(motor_speed-10);
		speed_right=speed_tran(motor_speed+10);
	}
	else
	{
		angle_target	= angle_lookfor_right;//右转	
		motor_speed =	speed_lookfor;		//减速
		speed_left=speed_tran(motor_speed+10);
		speed_right=speed_tran(motor_speed-10);
	}
	
	
	
	if( barrier_flag==1 )
	{
		//避障反转
		if(barrier_W < Img_Mid[barrier_H/10])
		{
			angle_target	= angle_lookfor_right;	
		}
		else
		{
			angle_target	= angle_lookfor_left;	
		}
	}
	
		barrier_flag_pre = barrier_flag;
}

//前往信标位置
void GotoTarget()
{
		//根据图像设定目标打角
		angle_target = angle_mid + (target_W-Img_Mid[target_H/10])*angle_K;	
		//若舵机打角小，迅速加速减速
    if(angle_target>angle_acc_right && angle_target<angle_acc_left)
    {                           
			//加速
			motor_speed = speed_go;
			speed_left=speed_tran(motor_speed);
			speed_right=speed_tran(motor_speed);
    }
    else
    {
		 //若舵机打角大，在低速下调整正方向
      motor_speed	= speed_turn;
			speed_left=speed_tran(motor_speed);
			speed_right=speed_tran(motor_speed);
    }
}

//信标切过
void Turn()
{
		if(turn_select[count_record]==1){
			//左切过
			angle_target = angle_mid + (target_W-(Img_Mid[target_H/10]-70))*angle_K;	
		}
		else
		{
			//右切过			
			angle_target = angle_mid + (target_W-(Img_Mid[target_H/10]+70))*angle_K;	
		}
		//减速
		motor_speed =	speed_turn;
		speed_left=speed_tran(motor_speed);
		speed_right=speed_tran(motor_speed);
}

//找不到信标，进行倒车
void Back()
{
		//用以计算循环时间
			if(target_flag ==0)
		{
			loop_count++;
		}else
		{
			loop_count = 0;
		}
		
		 // 找不到信标进行倒车
		if(loop_count >150)
		{
			CMT_PWM_ChangDuty(angle_mid);
			PID_speed_set(speed_back,speed_back);
			DelayMs(1000);
			loop_count = 0;
		}
		
		
		 // 信标卡住倒车
		if(avespeed<30){
			speed_cnt++;
			if(speed_cnt>=30 && motor_speed>0)
			{
				CMT_PWM_ChangDuty(angle_mid);
				PID_speed_set(speed_back,speed_back);
				DelayMs(1000);
				loop_count = 0;
				speed_cnt=0;
			}
		}
		else
		{
			speed_cnt=0;
		}
}



//信标避障
void Pass()
{
		if(target_W < Img_Mid[target_H/10] ){
			angle_target = angle_pass_left;
		}
		else
		{
			angle_target = angle_pass_right;	
		}
}


//避障
void Evade()
{
	
		if(barrier_W <Img_Mid[barrier_H/10]){
			angle_target = angle_mid + 30; //左避障

			if(target_W!=0 && target_W <Img_Mid[target_H/10]){
				route_select[count_record]=0;
			}
		}
		else
		{
			angle_target = angle_mid - 30;	//右避障

			if(target_W!=0 && target_W >Img_Mid[target_H/10]){
				route_select[count_record]=1;
			}
		}
			motor_speed =	speed_turn;
			speed_left=speed_tran(motor_speed);
			speed_right=speed_tran(motor_speed);
}

//将cm每秒转换为给定值,速度变换
uint16_t speed_tran(uint16_t speed)
{
	uint16_t tran_speed;
	tran_speed=speed_k*speed;
	return tran_speed;
}
