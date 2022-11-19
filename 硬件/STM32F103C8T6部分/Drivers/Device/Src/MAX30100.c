
/*--Include-start-------------------------------------------------------------*/
#include "MAX30100.h"
#include "IICcom.h"

/*define ---------------------------------------------------------------------*/
#define max30100_WR_address 0xAE

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */


/*Global data space ----------------------------------------------------------*/

uint8_t max30100_Bus_Write(uint8_t Register_Address, uint8_t Word_Data)
{

	/* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */

	/* ��1��������I2C���������ź� */
	I2C_Start();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	I2C_SendByte(max30100_WR_address | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ */
	I2C_SendByte(Register_Address);
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	
	/* ��5������ʼд������ */
	I2C_SendByte(Word_Data);

	/* ��6��������ACK */
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ����I2C����ֹͣ�ź� */
	I2C_Stop();
	return 1;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	I2C_Stop();
	return 0;
}


uint8_t max30100_Bus_Read(uint8_t Register_Address)
{
	uint8_t  data;


	/* ��1��������I2C���������ź� */
	I2C_Start();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	I2C_SendByte(max30100_WR_address | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ�� */
	I2C_SendByte((uint8_t)Register_Address);
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	

	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	I2C_Start();

	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	I2C_SendByte(max30100_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */

	/* ��8��������ACK */
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��9������ȡ���� */
	{
		data = I2C_RadeByte();	/* ��1���ֽ� */

		I2C_NoAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
	}
	/* ����I2C����ֹͣ�ź� */
	I2C_Stop();
	return data;	/* ִ�гɹ� ����dataֵ */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	I2C_Stop();
	return 0;
}

void max30100_FIFO_Read(uint8_t Register_Address,uint16_t  Word_Data[][2],uint8_t count)
{
	uint8_t i=0;
	uint8_t no = count;
	uint8_t data1, data2;
	/* ��1��������I2C���������ź� */
	I2C_Start();

	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	I2C_SendByte(max30100_WR_address | I2C_WR);	/* �˴���дָ�� */

	/* ��3��������ACK */
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��4���������ֽڵ�ַ�� */
	I2C_SendByte((uint8_t)Register_Address);
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	

	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	I2C_Start();

	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	I2C_SendByte(max30100_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */

	/* ��8��������ACK */
	if (I2C_WaitAck() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}

	/* ��9������ȡ���� */
	while (no)
	{
		data1 = I2C_RadeByte();	
		I2C_Ack();
		data2 = I2C_RadeByte();
		I2C_Ack();
		Word_Data[i][0] = (((uint16_t)data1 << 8) | data2);  //

		
		data1 = I2C_RadeByte();	
		I2C_Ack();
		data2 = I2C_RadeByte();
		if(1==no)
			I2C_NoAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		else
			I2C_Ack();
		Word_Data[i][1] = (((uint16_t)data1 << 8) | data2); 

		no--;	
		i++;
	}
	/* ����I2C����ֹͣ�ź� */
	I2C_Stop();

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	I2C_Stop();
}

void max30100_init(void)
{
	max30100_Bus_Write(0x06, 0x0b);  //mode configuration : temp_en[3]      MODE[2:0]=010 HR only enabled    011 SP02 enabled
	//max30100_Bus_Write(0x06, 0x0a);  //MODE[2:0]=010 HR only enabled     when used is mode ,the red led is not used.
	max30100_Bus_Write(0x01, 0xF0); //open all of interrupt
	max30100_Bus_Write(INTERRUPT_REG, 0x00); //all interrupt clear
	max30100_Bus_Write(0x09, 0x33); //r_pa=3,ir_pa=3

#if (SAMPLES_PER_SECOND == 50)
	max30100_Bus_Write(0x07, 0x43); //SPO2_SR[4:2]=000   50 per second    LED_PW[1:0]=11  16BITS
#elif (SAMPLES_PER_SECOND == 100)
	max30100_Bus_Write(0x07, 0x47); //SPO2_SR[4:2]=001  100 per second    LED_PW[1:0]=11  16BITS
#elif (SAMPLES_PER_SECOND == 200)
	max30100_Bus_Write(0x07, 0x4F); 
#elif (SAMPLES_PER_SECOND == 400)
	max30100_Bus_Write(0x07, 0x53); 
#endif
	
	max30100_Bus_Write(0x02, 0x00); //set FIFO write Pointer reg = 0x00 for clear it
	max30100_Bus_Write(0x03, 0x00);	//set Over Flow Counter  reg = 0x00 for clear it
	max30100_Bus_Write(0x04, 0x0F);	//set FIFO Read Pointer  reg = 0x0f for   
											//waitting  write pointer eq read pointer   to   interrupts  INTERRUPT_REG_A_FULL
}


