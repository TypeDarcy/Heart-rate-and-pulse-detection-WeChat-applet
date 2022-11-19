#ifndef __GUI_H
#define __GUI_H

#include "stm32f1xx_hal.h"

//color std define 
#define RED  	  0xf800
#define GREEN	  0x07e0
#define BLUE   	0x001f
#define WHITE	  0xffff
#define BLACK	  0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   			//��ɫ0 3165 00110 001011 00101
#define GRAY1   0x8410      	//��ɫ1      00000 000000 00000
#define GRAY2   0x4208      	//��ɫ2  1111111111011111
#define GRAY3		0x1082
//Bresenham�㷨 ����Բ
void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc);
//���ߺ�����ʹ��Bresenham �����㷨
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color);
//��������
void gui_draw_Block(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t lineColor,uint16_t blockColor);
//���ƾ���
void gui_draw_square(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t lineColor);

//�����ַ���
void Gui_DrawFont_GBK16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);

void Gui_DrawFont_GBK24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);

void Gui_DrawFont_Num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num);


#endif /*__GUI_H*/

