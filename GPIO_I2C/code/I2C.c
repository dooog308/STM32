/*(1<<6)的是scl,(1<<7)的是sda
 */
#include "I2C.h"
void delay(uint32_t time)
{
	for(int i=0;i<time;i++);
}
void I2C_start(void)
{
	GPIOA -> ODR |= (1<<6);//先將scl和sda都拉高
	GPIOA -> ODR |= (1<<7);
	delay(200);
	GPIOA -> ODR &= ~(1<<7);//再將sda拉低
	delay(200);
	GPIOA -> ODR &= ~(1<<6);//拉低scl
}
void I2C_stop(void)
{
	delay(13);
	GPIOA -> MODER |= (1<<14);//將sda轉為輸出
	GPIOA->ODR &= ~(1<<7);//將sda拉低
	delay(187);
	GPIOA->ODR |= (1<<6);//將scl拉高
	delay(20);
	GPIOA->ODR |= (1<<7);//將sda拉高
}
uint8_t I2C_tran(uint8_t tx)
{
	uint8_t e=0;
	GPIOA -> MODER |= (1<<14);//將sda轉為輸出
	for(int i=0;i<8;i++)
	{
		delay(20);
	    (tx&0x80)? (GPIOA->ODR |= (1<<7)) : (GPIOA->ODR &= ~(1<<7));//傳送資料
		tx = tx<<1;
		delay(180);
		GPIOA -> ODR |= (1<<6);//將scl拉高讓從機接收資料
		delay(200);
		GPIOA -> ODR &= ~(1<<6);//拉低scl以改變sda
	}
	GPIOA -> MODER &= ~(0X3<<14);//將sda改成輸入以接收ack訊號
	delay(200);
	GPIOA -> ODR |= (1<<6);//拉高
	if((GPIOA->IDR & 0x80)) e=1;//沒收到ack代表傳輸錯誤，或是最後一筆資料
	delay(200);
	GPIOA -> ODR &= ~(1<<6);
	return e;
}
void I2C_read(uint8_t *rx, char end)
{
	uint8_t tem=0;
	GPIOA -> MODER &= ~(0X3<<14);//將sda轉為輸入
	for(int i=0;i<8;i++)
	{
		delay(200);
		GPIOA -> ODR |= (1<<6);
		delay(20);
		if(GPIOA->IDR&0x80) (tem|=1);//接收從機傳回的資料
		if(i!=7) tem = tem<<1;
		delay(180);
		GPIOA -> ODR &= ~(1<<6);
	}
	delay(20);
	GPIOA -> MODER |= (1<<14);//將sda轉成輸出以傳送ACK訊號
	if(end == 'f') GPIOA->ODR &= ~(1<<7);//如果不是最後一筆資料就要傳送ACK
	else GPIOA->ODR |= (1<<7);
	delay(180);
	GPIOA -> ODR |= (1<<6);
	delay(200);
	GPIOA -> ODR &= ~(1<<6);
	*rx = tem;
}
