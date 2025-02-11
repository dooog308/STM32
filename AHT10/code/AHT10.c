
#include "AHT10.h"
void I2C_sendaddr(uint8_t addr, char cmd)
{
	I2C1->CR1 |= (1);
	if(cmd == 'r')
	{
		addr |= 1;
		I2C1->CR1 |= (1<<10);//ack enable
	}
	I2C1->CR1 |= (1<<8);//輸出開始信號
	while(!(I2C1->SR1&0x1));//等待開始信號輸出完成
	I2C1->DR = addr;
	while(!(I2C1->SR1 & 0X2));//等待地址傳送完成
	while(!(I2C1->SR2 & 0X1));
}

void I2C_senddata(uint8_t data, char end)
{
	while(!(I2C1->SR1 & 0x80));
	I2C1->DR = data;
	while(!(I2C1->SR1 & 0x4));
	if(end == 't') I2C1->CR1 |= (1<<9);//結束信號
}

void I2C_readdata(uint8_t *I2C_rxbuff)
{
	uint8_t i=0;
	for(i=0;i<6;i++)
	{
		while(!(I2C1->SR1 & 0x4));
		I2C_rxbuff[i] = I2C1->DR;
		if(i == 4)//根據i2c的通訊協定，在倒數第二個資料讀取完畢後要做以下動作，再讀最後一筆資料
		{
			I2C1->CR1 &= ~(1<<10);//ack disable
			I2C1->CR1 |= (1<<9);//結束信號
		}
	}
}

void AHT10_init(uint8_t *I2C_rxbuff)
{
	AHT10_read(I2C_rxbuff);
	if(I2C_rxbuff[0] & 0x8) return;
	I2C_sendaddr('w', AHT10_ADDR);
	I2C_senddata(0xe1, 'f');//0xe1: 初始化指令
	I2C_senddata(0x08,'f');//參數
	I2C_senddata(0x00,'t');//參數
}

void AHT10_measure(uint32_t* temp, uint32_t* hum, uint8_t *I2C_rxbuff)
{
	I2C_sendaddr(AHT10_ADDR, 'w');
	I2C_senddata(0xac, 'f');//0xac: 測量指令
	I2C_senddata(0x00,'f');//參數
	I2C_senddata(0x00,'t');//參數
	HAL_Delay(40);//等待測量
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

void AHT10_read(uint8_t *I2C_rxbuff)
{
	I2C_sendaddr(AHT10_ADDR, 'r');
	I2C_readdata(I2C_rxbuff);
}
void AHT10_reset(void)
{
	I2C_sendaddr(AHT10_ADDR, 'w');
	I2C_senddata(0xba, 'e');//oxba: reset指令
	HAL_Delay(21);//根據AHT10datasheet，reset後要等待最多20ms
}
