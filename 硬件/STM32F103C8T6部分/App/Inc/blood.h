#ifndef __BLOOD_H
#define __BLOOD_H

#include "stm32f1xx_hal.h"
#include "ds18b20.h"
typedef enum
{
	BLD_NORMAL,		//����
	BLD_ERROR,		//������
	
}BloodState;//ѪҺ״̬

typedef struct
{
	uint16_t 		heart;		//��������
	float 			SpO2;			//Ѫ������
	BloodState	state;		//״̬
	uint8_t   	update;		//��Ϣ���±�־λ
	uint8_t   	display;	//���ݸ��±�־λ
}BloodData;



void blood_Setup(void);
void blood_Loop(void);
void blood_Interrupt(void);


#endif /*__BLOOD_H*/
