#ifndef _CARTEST_H
#define _CARTEST_H
#include "head.h"

void Test_module( uint32_t data );
void Uart_test(	void );
void Servo_Test(	void );
void Motor_Test(	void );
void Image_Test(	void );
void LCD_Test(	void );
void PID_Test(	void );
void barrier_Test(	void );
void send_PC_data(uint16_t data1,uint16_t data2,uint16_t data3,uint16_t data4);


extern uint8_t PID_test_flag;	
extern uint16_t speed;
#endif
