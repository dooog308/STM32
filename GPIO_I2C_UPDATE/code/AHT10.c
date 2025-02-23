
#include "AHT10.h"
extern uint8_t tran_done,read_done,errow,start_done,stop_done,delay_done;
void AHT10_init(uint8_t *I2C_rxbuff)
{
	//AHT10_read(I2C_rxbuff);
	//if(I2C_rxbuff[0] & 0x8) return;
	I2C_start();
	while(!(start_done&0x1));//等待發送開始信號完成，這段時間是可以做其他事的
	I2C_tran(AHT10_ADDR);
	while(!(tran_done&0x1));//等待傳輸完成，這段時間是可以做其他事的
	I2C_tran(0xe1);
	while(!(tran_done&0x1));
	I2C_tran(0x08);
	while(!(tran_done&0x1));
	I2C_tran(0x00);
	while(!(tran_done&0x1));
	I2C_stop();
	while(!(stop_done&0x1));//等待發送結束信號完成，這段時間是可以做其他事的
}
void AHT10_read(uint8_t *I2C_rxbuff)
{
	I2C_start();
	while(!(start_done&0x1));
	I2C_tran(AHT10_ADDR|1);
	while(!(tran_done&0x1));
	for(int i=0;i<6;i++)
	{
		(i!=5) ? I2C_read(&I2C_rxbuff[i],'f') : I2C_read(&I2C_rxbuff[i],'t');
		while(!(read_done&0x1));//等待讀取完成，這段時間是可以做其他事的
	}
	I2C_stop();
	while(!(stop_done&0x1));
}
void AHT10_measure(uint32_t* temp, uint32_t* hum, uint8_t *I2C_rxbuff)
{
	for(int i=0;i<6;i++)
		I2C_rxbuff[i] = 0;
	I2C_start();
	while(!(start_done&0x1));
	I2C_tran(AHT10_ADDR);
	while(!(tran_done&0x1));
	I2C_tran(0xac);
	while(!(tran_done&0x1));
	I2C_tran(0x00);
	while(!(tran_done&0x1));
	I2C_tran(0x00);
	while(!(tran_done&0x1));
	I2C_stop();
	while(!(stop_done&0x1));
	delay(40000);
	while(!(delay_done&0x1));
	AHT10_read(I2C_rxbuff);
	uint32_t data=0;
	//根據AHT10datasheet的溫溼度換算公式
	data = (uint32_t)(I2C_rxbuff[3]>>4)+(uint32_t)(I2C_rxbuff[2]<<4)+(uint32_t)(I2C_rxbuff[1]<<12);
	*hum = (data*100)/(1<<20);
	data = (uint32_t)(I2C_rxbuff[5])+(uint32_t)(I2C_rxbuff[4]<<8)+(uint32_t)((I2C_rxbuff[3]&0x0f)<<16);
	*temp = ((data*200)/(1<<20))-50;
	PRINTFC("溫度: ");
	PRINTFI(*temp);
	PRINTFC("c 濕度: ");
	PRINTFI(*hum);
	PRINTFC("%\r\n");
}
