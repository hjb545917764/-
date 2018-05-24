#include "head.h"

uint8_t speed_cnt=0;
//�����ű�

uint16_t speed_tran(uint16_t speed);//�ٶȱ任

void Lookfor()
{
	
	//�ı����Ƕȣ�����תȦ
	if(route_select[count_record] == 1)
	{
		angle_target	= angle_lookfor_left;	//��ת
		motor_speed =	speed_lookfor;		//����
		speed_left=speed_tran(motor_speed-10);
		speed_right=speed_tran(motor_speed+10);
	}
	else
	{
		angle_target	= angle_lookfor_right;//��ת	
		motor_speed =	speed_lookfor;		//����
		speed_left=speed_tran(motor_speed+10);
		speed_right=speed_tran(motor_speed-10);
	}
	
	
	
	if( barrier_flag==1 )
	{
		//���Ϸ�ת
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

//ǰ���ű�λ��
void GotoTarget()
{
		//����ͼ���趨Ŀ����
		angle_target = angle_mid + (target_W-Img_Mid[target_H/10])*angle_K;	
		//��������С��Ѹ�ټ��ټ���
    if(angle_target>angle_acc_right && angle_target<angle_acc_left)
    {                           
			//����
			motor_speed = speed_go;
			speed_left=speed_tran(motor_speed);
			speed_right=speed_tran(motor_speed);
    }
    else
    {
		 //�������Ǵ��ڵ����µ���������
      motor_speed	= speed_turn;
			speed_left=speed_tran(motor_speed);
			speed_right=speed_tran(motor_speed);
    }
}

//�ű��й�
void Turn()
{
		if(turn_select[count_record]==1){
			//���й�
			angle_target = angle_mid + (target_W-(Img_Mid[target_H/10]-70))*angle_K;	
		}
		else
		{
			//���й�			
			angle_target = angle_mid + (target_W-(Img_Mid[target_H/10]+70))*angle_K;	
		}
		//����
		motor_speed =	speed_turn;
		speed_left=speed_tran(motor_speed);
		speed_right=speed_tran(motor_speed);
}

//�Ҳ����ű꣬���е���
void Back()
{
		//���Լ���ѭ��ʱ��
			if(target_flag ==0)
		{
			loop_count++;
		}else
		{
			loop_count = 0;
		}
		
		 // �Ҳ����ű���е���
		if(loop_count >150)
		{
			CMT_PWM_ChangDuty(angle_mid);
			PID_speed_set(speed_back,speed_back);
			DelayMs(1000);
			loop_count = 0;
		}
		
		
		 // �ű꿨ס����
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



//�ű����
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


//����
void Evade()
{
	
		if(barrier_W <Img_Mid[barrier_H/10]){
			angle_target = angle_mid + 30; //�����

			if(target_W!=0 && target_W <Img_Mid[target_H/10]){
				route_select[count_record]=0;
			}
		}
		else
		{
			angle_target = angle_mid - 30;	//�ұ���

			if(target_W!=0 && target_W >Img_Mid[target_H/10]){
				route_select[count_record]=1;
			}
		}
			motor_speed =	speed_turn;
			speed_left=speed_tran(motor_speed);
			speed_right=speed_tran(motor_speed);
}

//��cmÿ��ת��Ϊ����ֵ,�ٶȱ任
uint16_t speed_tran(uint16_t speed)
{
	uint16_t tran_speed;
	tran_speed=speed_k*speed;
	return tran_speed;
}
