
#include "AHT10.h"

extern uint8_t rxd[6], temp, hum, I2C_tx;
extern char txd[50],end;
extern void* t;
extern TaskHandle_t PC_Handle, I2C_tran_Handle, I2C_start_Handle, I2C_stop_Handle,
I2C_read_Handle, AHT10_init_Handle, AHT10_read_Handle, AHT10_measure_Handle;
extern QueueHandle_t q1_Handle;
extern uint8_t tran_done,read_done,start_done,stop_done, uart_tran_done;
uint8_t AHT10_read_done=0;

void AHT10_init(void * pvParameters)
{
	while(1)
	{
		vTaskDelay(1100);
		vTaskResume(AHT10_read_Handle);
		while(!(AHT10_read_done&0x1));
		AHT10_read_done = 0;
		if(rxd[0] & 0x8 == 0)
		{
			vTaskResume(AHT10_measure_Handle);
			vTaskSuspend(AHT10_init_Handle);
		}
		else
		{
			vTaskResume(I2C_start_Handle);
			while(!(start_done&0x1));
			start_done = 0;
			I2C_tx = AHT10_ADDR;
			vTaskResume(I2C_tran_Handle);
			while(!(tran_done&0x1));
			tran_done = 0;
			I2C_tx = 0xe1;
			vTaskResume(I2C_tran_Handle);
			while(!(tran_done&0x1));
			tran_done = 0;
			I2C_tx = 0x08;
			vTaskResume(I2C_tran_Handle);
			while(!(tran_done&0x1));
			tran_done = 0;
			I2C_tx = 0x00;
			vTaskResume(I2C_tran_Handle);
			while(!(tran_done&0x1));
			tran_done = 0;
			vTaskResume(I2C_stop_Handle);
			while(!(stop_done&0x1));
			tran_done = 0;
			vTaskResume(AHT10_measure_Handle);
			vTaskSuspend(AHT10_init_Handle);
		}
	}
}
void AHT10_read(void * pvParameters)
{
	while(1)
	{
		AHT10_read_done = 0;
		vTaskResume(I2C_start_Handle);
		while(!(start_done&0x1));
		start_done = 0;
		I2C_tx = AHT10_ADDR|1;
		vTaskResume(I2C_tran_Handle);
		while(!(tran_done&0x1));
		tran_done = 0;
		for(int i=0;i<6;i++)
		{
			(i!=5)? (end='f') : (end='t');
			vTaskResume(I2C_read_Handle);
			while(!(read_done&0x1));
			read_done = 0;
		}
		vTaskResume(I2C_stop_Handle);
		while(!(stop_done&0x1));
		stop_done = 0;
		AHT10_read_done = 1;
		vTaskSuspend(AHT10_read_Handle);
	}
}
void AHT10_measure(void * pvParameters)
{
	while(1)
	{
		vTaskResume(I2C_start_Handle);
		while(!(start_done&0x1));
		start_done = 0;
		I2C_tx = AHT10_ADDR;
		vTaskResume(I2C_tran_Handle);
		while(!(tran_done&0x1));
		tran_done = 0;
		I2C_tx = 0xac;
		vTaskResume(I2C_tran_Handle);
		while(!(tran_done&0x1));
		tran_done = 0;
		I2C_tx = 0x00;
		vTaskResume(I2C_tran_Handle);
		while(!(tran_done&0x1));
		tran_done = 0;
		I2C_tx = 0x00;
		vTaskResume(I2C_tran_Handle);
		while(!(tran_done&0x1));
		tran_done = 0;
		vTaskResume(I2C_stop_Handle);
		while(!(stop_done&0x1));
		stop_done = 0;
		vTaskDelay(2300);
		vTaskResume(AHT10_read_Handle);
		while(!(AHT10_read_done&0x1));
		AHT10_read_done = 0;
		uint32_t data=0;
		//根據AHT10datasheet的溫溼度換算公式
		data = (uint32_t)(rxd[3]>>4)+(uint32_t)(rxd[2]<<4)+(uint32_t)(rxd[1]<<12);
		hum = (data*100)/(1<<20);
		data = (uint32_t)(rxd[5])+(uint32_t)(rxd[4]<<8)+(uint32_t)((rxd[3]&0x0f)<<16);
		temp = ((data*200)/(1<<20))-50;
		strcpy(txd, "temperature: ");
		xQueueSend(q1_Handle, &t, 10);
		vTaskResume(PC_Handle);
		while(!(uart_tran_done&0x1));
		uart_tran_done = 0;
		PRINTFI(temp);
		strcpy(txd, " humidity: ");
		xQueueSend(q1_Handle, &t, 10);
		vTaskResume(PC_Handle);
		while(!(uart_tran_done&0x1));
		uart_tran_done = 0;
		PRINTFI(hum);
		strcpy(txd, "%\r\n");
		xQueueSend(q1_Handle, &t, 10);
		vTaskResume(PC_Handle);
		while(!(uart_tran_done&0x1));
		uart_tran_done = 0;
		vTaskSuspend(AHT10_measure_Handle);
	}
}
