/*(1<<6)的是scl,(1<<7)的是sda
 */
#include "I2C.h"

extern TaskHandle_t I2C_tran_Handle, I2C_start_Handle, I2C_stop_Handle,
I2C_read_Handle;
extern uint8_t rxd[6], temp, hum, I2C_tx;
extern char end;
uint8_t ind=0;
uint8_t tran_done=0,read_done=0,start_done=0,stop_done=0;

void I2C_start(void * pvParameters)
{
	while(1)
	{
		start_done = 0;
		GPIOA -> ODR |= (1<<6);//先將scl和sda都拉高
		GPIOA -> ODR |= (1<<7);
		vTaskDelay(10);
		GPIOA -> ODR &= ~(1<<7);//再將sda拉低
		vTaskDelay(10);
		GPIOA -> ODR &= ~(1<<6);//拉低scl
		start_done = 1;
		vTaskSuspend(I2C_start_Handle);
	}
}
void I2C_stop(void * pvParameters)
{
	while(1)
	{
		stop_done = 0;
		vTaskDelay(2);
		GPIOA -> MODER |= (1<<14);//將sda轉為輸出
		GPIOA->ODR &= ~(1<<7);//將sda拉低
		vTaskDelay(8);
		GPIOA->ODR |= (1<<6);//將scl拉高
		vTaskDelay(2);
		GPIOA->ODR |= (1<<7);//將sda拉高
		stop_done = 1;
		vTaskSuspend(I2C_stop_Handle);
	}
}
void I2C_tran(void * pvParameters)
{
	while(1)
	{
		tran_done = 0;
		GPIOA -> MODER |= (1<<14);//將sda轉為輸出
		for(int i=0;i<8;i++)
		{
			vTaskDelay(2);
			(I2C_tx&0x80)? (GPIOA->ODR |= (1<<7)) : (GPIOA->ODR &= ~(1<<7));//傳送資料
			I2C_tx = I2C_tx<<1;
			vTaskDelay(8);
			GPIOA -> ODR |= (1<<6);//將scl拉高讓從機接收資料
			vTaskDelay(10);
			GPIOA -> ODR &= ~(1<<6);//拉低scl以改變sda
		}
		GPIOA -> MODER &= ~(0X3<<14);//將sda改成輸入以接收ack訊號
		vTaskDelay(10);
		GPIOA -> ODR |= (1<<6);//拉高
		vTaskDelay(10);
		GPIOA -> ODR &= ~(1<<6);
		tran_done = 1;
		vTaskSuspend(I2C_tran_Handle);
	}
}
void I2C_read(void * pvParameters)
{
	uint8_t tem=0;
	while(1)
	{
		read_done = 0;
		tem = 0;
		GPIOA -> MODER &= ~(0X3<<14);//將sda轉為輸入
		for(int i=0;i<8;i++)
		{
			vTaskDelay(10);
			GPIOA -> ODR |= (1<<6);
			vTaskDelay(2);
			if(GPIOA->IDR&0x80) (tem|=1);//接收從機傳回的資料
			if(i!=7) tem = tem<<1;
			vTaskDelay(8);
			GPIOA -> ODR &= ~(1<<6);
		}
		vTaskDelay(2);
		GPIOA -> MODER |= (1<<14);//將sda轉成輸出以傳送ACK訊號
		if(end == 'f') GPIOA->ODR &= ~(1<<7);//如果不是最後一筆資料就要傳送ACK
		else GPIOA->ODR |= (1<<7);
		vTaskDelay(8);
		GPIOA -> ODR |= (1<<6);
		vTaskDelay(10);
		GPIOA -> ODR &= ~(1<<6);
		rxd[ind] = tem;
		ind++;
		if(ind == 6) ind = 0;
		read_done = 1;
		vTaskSuspend(I2C_read_Handle);
	}
}/*
 * I2C.c
 *
 *  Created on: Mar 1, 2025
 *      Author: User
 */


