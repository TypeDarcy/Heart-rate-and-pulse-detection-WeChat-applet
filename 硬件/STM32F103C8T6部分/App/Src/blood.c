
/*--Include-start-------------------------------------------------------------*/
#include "blood.h"
#include "MAX30100.h"
#include "algorithm.h"
#include "math.h"
#include "gui.h"
#include "stdio.h"
#include "ST7735.h"
#include "beep.h"
/*Global data space ----------------------------------------------------------*/
uint16_t g_fft_index = 0;         	 	//fft��������±�

struct compx s1[FFT_N+16];           	//FFT������������S[1]��ʼ��ţ����ݴ�С�Լ�����
struct compx s2[FFT_N+16];           	//FFT������������S[1]��ʼ��ţ����ݴ�С�Լ�����

struct
{
	float 	Hp	;			//Ѫ�쵰��	
	float 	HpO2;			//����Ѫ�쵰��
	
}g_BloodWave;//ѪҺ��������

BloodData g_blooddata = {0};					//ѪҺ���ݴ洢

#define CORRECTED_VALUE			47   			//�궨ѪҺ��������

/*funcation start ------------------------------------------------------------*/


/*Sensor func -----------------------------------------------------------------*/
//ѪҺ�����Ϣ����
void blood_data_update(void)
{
	uint16_t temp_num=0;
	uint16_t fifo_word_buff[1][2];
	
	temp_num = max30100_Bus_Read(INTERRUPT_REG);
	
	//��־λ��ʹ��ʱ ��ȡFIFO
	if (INTERRUPT_REG_A_FULL&temp_num)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,1);
		//��ȡFIFO
		max30100_FIFO_Read(0x05,fifo_word_buff,1); //read the hr and spo2 data form fifo in reg=0x05
		
		//������д��fft���벢������
		for(int i = 0;i < 1;i++)
		{
			if(g_fft_index < FFT_N)
			{
				s1[g_fft_index].real = fifo_word_buff[i][0];
				s1[g_fft_index].imag= 0;
				s2[g_fft_index].real = fifo_word_buff[i][1];
				s2[g_fft_index].imag= 0;
				g_fft_index++;
			}
		}
		//��Ϣ���±�־λ
		g_blooddata.update++;
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,0);
	}
}
//ѪҺ��Ϣת��
void blood_data_translate(void)
{	
	//������д�����
	if(g_fft_index>=FFT_N)
	{
		//��ʼ�任��ʾ
		Gui_DrawFont_GBK16(102,2,BLACK,GREEN,"FFT");
		
		g_fft_index = 0;
		
		//���ݸ��±�־λ
		g_blooddata.display = 1;
		
		//���ٸ���Ҷ�任
		FFT(s1);
		FFT(s2);
		
		//��ƽ��
		for(int i = 0;i < FFT_N;i++) 
		{
			s1[i].real=sqrtf(s1[i].real*s1[i].real+s1[i].imag*s1[i].imag);
			s2[i].real=sqrtf(s2[i].real*s2[i].real+s2[i].imag*s2[i].imag);
		}
		
		//��ȡ��ֵ�� �������������Ϊ 
		uint16_t s1_max_index = find_max_num_index(s1, 60);
		uint16_t s2_max_index = find_max_num_index(s2, 60);
		
		//���HbO2��Hb�ı仯Ƶ���Ƿ�һ��
		if(s1_max_index == s2_max_index)
		{
			//���ʼ���
			uint16_t Heart_Rate = 60 * SAMPLES_PER_SECOND * 
														s2_max_index / FFT_N;
			
			g_blooddata.heart = Heart_Rate - 10;
			
			//Ѫ����������
			float sp02_num = (s2[s1_max_index].real * s1[0].real)
											/(s1[s1_max_index].real * s2[0].real);
			
			sp02_num = (1 - sp02_num) * SAMPLES_PER_SECOND + CORRECTED_VALUE;
			
			g_blooddata.SpO2 = sp02_num;
			//���²��������
			
			short temp = DS18B20_Get_Temp();
			
			double f_temp = (temp / 10.0) + ((temp % 10) / 10.0); 
			if((f_temp > 0.0 && f_temp < 50.0) && (g_blooddata.SpO2 > 0.0))
				printf("{%d,%.1f,%.1f}", g_blooddata.heart, g_blooddata.SpO2, f_temp);
			
			//״̬����
			g_blooddata.state = BLD_NORMAL;
		}
		else //���ݷ����쳣
		{
			g_blooddata.heart = 0;
			g_blooddata.SpO2 	= 0;
			g_blooddata.state = BLD_ERROR;
		}
		//�����任��ʾ
		Gui_DrawFont_GBK16(102,2,GREEN,BLACK,"FFT");
	}
}

//ѪҺ���ݸ���
void blood_wave_update(void)
{
	static DC_FilterData hbdc = {.w = 0,.init = 0,.a = 0};
	static DC_FilterData hbodc = {.w = 0,.init = 0,.a = 0};
	
	float hbag = 0;
	float hboag = 0;
	
	float hbag_d = 0;
	float hboag_d = 0;
	
	//ǰ8����ƽ��ֵ
	for(int i = 0;i < 8;i++)
	{
		hbag  += s1[g_fft_index - 8 + i].real;
		hboag += s2[g_fft_index - 8 + i].real;
	}
		
	//ֱ���˲�
	hbag_d = dc_filter(hbag,&hbdc) / 8;
	hboag_d = dc_filter(hboag,&hbodc) / 8;
	
	//�߶�����
	float hbhight  = 0;
	float hbohight = 0;
	
	//������ƫ��
	hbhight  = (-hbag_d / 40.0) + 5;
	hbohight  = (-hboag_d / 40.0) + 5;
	
	//�߶����ݷ�������
	hbhight = (hbhight > 27) ? 27 : hbhight;
	hbhight = (hbhight < 0) ?  0 : hbhight;
	
	hbohight = (hbohight > 27) ? 27 : hbohight;
	hbohight = (hbohight < 0) ?  0 : hbohight;
	
	//�����ݷ�����ȫ��
	g_BloodWave.Hp = hbhight;
	g_BloodWave.HpO2 = hbohight;
	
	//ѪҺ״̬���
	if(hbag < 5000 ||hbag < 5000)
	{
		//�������
		g_blooddata.heart = 0;
		g_blooddata.SpO2 	= 0;
		
		//ɾ������
		g_fft_index -= 7;
		
		if(g_blooddata.state != BLD_ERROR)
		{
			g_blooddata.state = BLD_ERROR;
			g_blooddata.display = 1;
		}
	}
	else
	{
		if(g_blooddata.state == BLD_ERROR)
		{
			g_blooddata.state = BLD_NORMAL;
			g_blooddata.display = 1;
		}
	}
}
/*tft display ---------------------------------------------------------------*/

//������ʾѪҺ��Ϣ
void tft_test_display(void)
{
	uint8_t str[50];

	if (g_blooddata.display == 1)
	{
		g_blooddata.display = 0;
		
		//��ʾѪ����Ϣ
		sprintf((char *)str,"heart = %3d",g_blooddata.heart);
		Gui_DrawFont_GBK16(8,8,0x00FF,BLACK,str);
		
		//��ʾ������Ϣ
		sprintf((char *)str,"SpO2 = %3.1f",g_blooddata.SpO2);
		Gui_DrawFont_GBK16(8,26,0x00FF,BLACK,str);
		
		//��ʾ״̬��Ϣ
		if(g_blooddata.state)
		{
			sprintf((char *)str,"ERROR     ");
			Gui_DrawFont_GBK16(8,44,0xF000,BLACK,str);
		}
		else
		{
			sprintf((char *)str,"NORMAL    ");
			Gui_DrawFont_GBK16(8,44,0x07E0,BLACK,str);
		}
	}
}

//���Ʊ߿�
void tft_draw_frame(uint16_t color)
{
	gui_draw_square(0,1,127,127,color);
	gui_draw_square(0,1,127,18,color);
	gui_draw_square(0,1,100,18,color);
	gui_draw_square(0,57,127,127,color);
}

//״̬��Ϣ��ʾ
void tft_draw_State(uint8_t st)
{
	switch(st)
	{
		case BLD_NORMAL:
		{
			tft_draw_frame(GRAY1);
			Gui_DrawFont_GBK16(3,2,0x001F,GRAY3,"Normal    ");
			break;
		}
		case BLD_ERROR:
		{
			tft_draw_frame(RED);
			Gui_DrawFont_GBK16(3,2,0xF000,GRAY3,"Caution   ");
			break;
		}
	}
}

//���Ʋ����߶�
void tft_draw_wave_line(uint8_t p,uint8_t n,uint16_t h0,uint16_t h1,uint16_t color)
{
	uint16_t x = 4;										// x ��ʼ����
	uint16_t y = (p == 0) ? 122 : 89; // y ��ʼ����
	
	//������
	if((n + 1) < 82)
	{
		Gui_DrawLine((x + n + 2),y - 28,(x + n + 2),y + 1,GRAY2);
		Gui_DrawLine((x + n + 1),y - 28,(x + n + 1),y + 1,BLACK);
	}
	//������
	Gui_DrawLine((x + n),(y - h0),(x + n + 1),(y - h1),color);
	
}

//���Ʋ���ͼ��
void tft_draw_wave(void)
{
	static uint16_t hist_n  = 0;	//ͬ����ʷ���
	
	static uint16_t hbhist_h  = 0;//Ѫ�쵰����ʷ��ֵ
	static uint16_t hbohist_h = 0;//����Ѫ�쵰����ʷ��ֵ

	//������
	tft_draw_wave_line(1, hist_n, hbhist_h , g_BloodWave.Hp, 0x8FF0);
	tft_draw_wave_line(0, hist_n, hbohist_h, g_BloodWave.HpO2, 0xFC1F);
	
	//�ƶ��±�
	hist_n = (hist_n < 81) ? (hist_n + 1) : 0;
	
	//������ʷ
	hbhist_h = g_BloodWave.Hp;
	hbohist_h = g_BloodWave.HpO2;
}

//����Ѫ������ˢ��
void tft_draw_hrsp(void)
{
	uint8_t str[50];

	//������Ϣ��ʾ
	sprintf((char *)str,"%3d  ",g_blooddata.heart);
	Gui_DrawFont_GBK16(40,20,0xffe0,GRAY3,str);
	//Ѫ�����Ͷ���ʾ
	g_blooddata.SpO2 = (g_blooddata.SpO2 > 99.99) ? 99.99:g_blooddata.SpO2;
	sprintf((char *)str," %2.2f%%  ",g_blooddata.SpO2);
	Gui_DrawFont_GBK16(40,38,0x07ff,GRAY3,str);
}


//��ʾ���ڻ���
void tft_draw_windows(void)
{
	Lcd_Clear(GRAY3);
	
	//���ƴ��ڱ���
	tft_draw_frame(GRAY1);

	
	//�������� h31 w84
	gui_draw_Block(3,60,87,91,GRAY2,BLACK);
	gui_draw_Block(3,93,87,124,GRAY2,BLACK);
	
	gui_draw_Block(89,60,125,91,GRAY2,BLACK);
	gui_draw_Block(89,93,125,124,GRAY2,BLACK);
	
	//�����ַ���
	Gui_DrawFont_GBK16(3,20,0xffe0,GRAY3,"HR  :");
	Gui_DrawFont_GBK16(3,38,0x07ff,GRAY3,"SpO2:");
	
	Gui_DrawFont_GBK16(91,67,0x8FF0,BLACK," Hb");
	Gui_DrawFont_GBK16(91,100,0xFC1F,BLACK,"HbO2");
	
	Gui_DrawFont_GBK16(102,2,GREEN,BLACK,"FFT");
	
	Gui_DrawFont_GBK16(3,2,0x001F,GRAY3,"Normal    ");
}

//tft��ʾˢ��
void tft_display_update(void)
{
	//ͼ�����
	if(g_blooddata.update >= 8)
	{
		//���ͼ����±�־λ
		g_blooddata.update = 0;
		//ѪҺ�������ݸ���
		blood_wave_update();
		//���Ʋ���
		tft_draw_wave();
	}
	
	//ת��������ݸ���
	if(g_blooddata.display >= 1)
	{
		//������±�־λ
		g_blooddata.display = 0;
		//��ʾѪҺ״̬��Ϣ
		tft_draw_State(g_blooddata.state);
		//����Ѫ������ˢ��
		tft_draw_hrsp();
	}
}
/*Beep and LED func -------------------------------------------------------*/

//LED�ͷ�����״̬����
void Led_beep_update(void)
{
	static uint32_t errorStartTick = 0;			//��������
	static BloodState blhist = BLD_NORMAL;	//��ʷ״̬
	static uint8_t beepstate = 0;						//������״̬
	
	
	switch(g_blooddata.state)
	{
		case BLD_NORMAL:
		{
			//LEDָʾ��
			Led_Control((g_BloodWave.Hp > 10) * 20);
			//������
			Beep_Control((g_BloodWave.HpO2 > 10) * 50);
			break;
		}
		case BLD_ERROR:
		{
			if(blhist == BLD_NORMAL)
			{
				errorStartTick = HAL_GetTick();
			}
			
			//����ǰ0-3�� ������˸
			if((HAL_GetTick() - errorStartTick) < 3000)
			{
				if(( (HAL_GetTick() / 100) % 
						 (4-(HAL_GetTick() - errorStartTick) / 1000)
				   ) == 0)
				{
					Led_Control(20);
					Beep_Control(50);
				}
				else
				{
					Led_Control(0);
					Beep_Control(0);
				}
				
			}
			//����ǰ3-6�� ����
			else if(3000 < (HAL_GetTick() - errorStartTick) &&
							6000 > (HAL_GetTick() - errorStartTick))
			{
				Led_Control(20);
				Beep_Control(50);
			}
			//����6�����
			else if((HAL_GetTick() - errorStartTick) > 6000)
			{
				Led_Control(0);
				Beep_Control(0);
			}
			break;
		}
		default:break;
	}
	
	blhist = g_blooddata.state;
}


/*Setup and loop func -----------------------------------------------------*/

void blood_Setup(void)
{
	//��ʼ��ѪҺ��Ϣ
	g_blooddata.heart = 0;
	g_blooddata.SpO2 = 0;
	g_blooddata.display = 1;
	
	//���ƴ���
	tft_draw_windows();
	
	//��ʼ��ָʾ�ƺͷ�����
	settone(14);
}

void blood_Loop(void)
{
	//ѪҺ��Ϣ��ȡ
	blood_data_update();
	//ѪҺ��Ϣת��
	blood_data_translate();
	//tft��ʾˢ��
	tft_display_update();
	//LED ��������Ϣ����
	Led_beep_update();
	
}

void blood_Interrupt(void)
{
	static int16_t div = 0;
	static int16_t div2 = 0;
	div++;
	if(div > 10)
	{
		div = 0;
		//����ѪҺ��Ϣ
		//blood_data_update();
	}
	
	div2++;
	if(div > 50)
	{
		div2 = 0;
		//tft��ʾˢ��
		//tft_display_update();
	}
}


