#ifndef _LCD_H
#define _LCD_H
#include "common.h"
#include <stdint.h>

 void LCD_Init(void);
 void LCD_CLS(void);
 void LCD_P6x8Str(uint8_t x,uint8_t y,uint8_t ch[]);
 void LCD_P8x16Str(uint8_t x,uint8_t y,uint8_t ch[]);
 void LCD_PutPixel(uint8_t x,uint8_t y);
 void LCD_Rectangle(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t gif);
 void LCD_Fill(uint8_t dat);
 void LCD_Dis_Num(uint8_t x,uint8_t y,uint16_t num,uint8_t N);
 void LCD_Dis_Float(uint8_t x,uint8_t y,double FL,uint8_t N);
#endif

