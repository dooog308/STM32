
#include<SD.h>

void SD_cs(uint8_t t)
{
	(t)? (GPIOB->ODR|=(1<<1)) : (GPIOB->ODR&=~(1<<1));
	for(int i=0;i<50;i++);
}
uint8_t SD_spi_send(uint8_t data)
{
	uint8_t read=0xff;
	SPI1->DR = data;
	while(!(SPI1->SR&0X2));
	SPI1->SR &= ~(1<<1);
	while(!(SPI1->SR&0X1));
	read = SPI1->DR;
	SPI1->SR &= ~(1);
	return read;
}
uint8_t SD_spi_read(void)
{
	uint8_t read=0xff;
	SPI1->DR = 0xff;
	while(!(SPI1->SR&0X2));
	SPI1->SR &= ~(1<<1);
	while(!(SPI1->SR&0X1));
	read = SPI1->DR;
	SPI1->SR &= ~(1);
	return read;
}
void SD_sendcmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t data[6];
	data[0] = (cmd | 0x40);
	data[1] = (uint8_t)(arg>>24);
	data[2] = (uint8_t)(arg>>16);
	data[3] = (uint8_t)(arg>>8);
	data[4] = (uint8_t)(arg);
	data[5] = crc;
	for(int i=0;i<6;i++) SD_spi_send(data[i]);
}
uint8_t SD_init(void)
{
	uint8_t read=0xff;
	uint32_t i=0xfff;
	SD_cs(0);
	SD_sendcmd(12, 0x00, 0xff);
	SD_cs(1);
	do{
		read = SD_spi_read();
		i--;
	}while(read!=0x01&&i);
	if(read == 0x01) return 1;
	read = 0xff;
	i = 0xfff;
	SD_cs(1);
	for(int j=0;j<11;j++)SD_spi_send(0xff);
	SD_cs(0);
	SD_sendcmd(0x00, 0, 0x95);
	do{
		read = SD_spi_read();
		i--;
	}while(read!=0x01&&i);
	if(read!=0x01&&i==0)
	{
		PRINTFC("go idle state fail\r\n");
		SD_errow();
		return 0;
	}
	SD_cs(1);
	SD_spi_send(0xff);
	SD_cs(0);
	i=10;
	read = 0xff;
	SD_sendcmd(0x08, 0x1aa, 0xff);
	do{
		read = SD_spi_read();
		i--;
	}while(read==0xff&&i!=0);
	if(i == 0)
	{
		PRINTFC("get card type fail\r\n");
		SD_errow();
		return 0;
	}
	else if(read == 1)
	{
		uint8_t re[4]={0};
		for(int j=0;j<4;j++) re[i] = SD_spi_read();
		for(int j=0;j<4;j++)
		{
			PRINTFI(re[i]);
			PRINTFC("\r\n");
		}
		SD_sendcmd(55, 0, 0xff);
		SD_sendcmd(41, 0x40000000, 0xFF);
		do{
			read=SD_spi_read();
		}while(read==0xff);
		PRINTFI(read);
		PRINTFC("\r\n");
		SD_cs(1);
		SD_spi_send(0xff);
		SD_cs(0);
		SD_sendcmd(58, 0, 0xff);
		do{
			read=SD_spi_read();
		}while(read==0xff);
		for(int i=0;i<4;i++)
			re[i]=SD_spi_read();
		for(int i=0;i<4;i++)
		{
			PRINTFI(re[i]);
			PRINTFC("\r\n");
		}
		SD_cs(1);
		SD_spi_send(0xff);
		SD_cs(0);
		SD_sendcmd(16, 512, 0xff);
		do{
			read = SD_spi_read();
		}while(read == 0xff);
		PRINTFI(read);
		PRINTFC("\r\n");
		SD_cs(1);
		SPI1->CR1 &= ~(0X7<<3);
		SPI1->CR1 |= (0X1<<3);
	}
	return 1;
}
uint8_t SD_read(uint8_t *buffer,uint64_t addr,uint16_t blocksize)
{
	SPI1->CR1 &= ~(0X7<<3);
	SPI1->CR1 |= (0X2<<3);
	SD_cs(1);
	SD_spi_send(0xff);
	SD_spi_send(0xff);
	SD_spi_send(0xff);
	uint8_t dt=0,count=0xff;
	addr=addr/512;
	SD_cs(0);
	SD_sendcmd(17, addr, 0xff);
	do{
		dt=SD_spi_read();
	}while(dt==0xff);
	do{
		dt=SD_spi_read();
		count--;
	}while(dt!=0xfe&&count);
	//PRINTFI(dt);
	if(count==0) return 0;
	for(int i=0;i<blocksize;i++)
	{
		*buffer=SD_spi_read();
		buffer++;
	}
	SD_spi_read();
	SD_spi_read();
	SD_sendcmd(12, 0x00, 0xff);
	SD_cs(1);
	SPI1->CR1 &= ~(0X7<<3);
	SPI1->CR1 |= (0X1<<3);
	SD_spi_send(0xff);
	SD_spi_send(0xff);
	SD_spi_send(0xff);
	return 1;
}
void SD_errow(void)
{
	while(1);
}
