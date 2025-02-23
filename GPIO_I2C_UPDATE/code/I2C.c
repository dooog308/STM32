/*(1<<6)的是scl,(1<<7)的是sda
 */
/*
 * 	tran_done:  用來標記數據傳輸是否完成。它會在每次傳輸開始時設為0，完成後設為1。
 * 	read_done:  用來標記讀取操作是否完成。當從I2C設備讀取數據時，開始時設為0，完成後設為1。
 *  ind:        跟蹤當前傳輸或接收的位索引。它從0開始，每次傳輸或接收一個位時增加。
 *	cmd:        指示當前執行的操作命令。根據不同的命令，I2C協議進行不同的操作（例如起始、傳輸、停止）。
 *	pot:        控制SCL引腳的狀態（高或低）。它會在不同的命令中切換，從而模擬I2C協議的位元傳輸。
 *	I2C_txbuff: 存儲待傳輸的字節數據。每次發送一個字節時，它會保存要傳送的數據並進行位操作。
 *	errow:      標記錯誤的發生。如果I2C接收到錯誤的ACK或其他問題，會將其設置為1。
 *	start_done: 指示I2C起始信號是否已經完成。它會在發送起始信號後設為0，直到起始信號的傳送完成後設為1。
 *	stop_done:  指示I2C停止信號是否已經完成。它會在發送停止信號後設為0，直到停止信號的傳送完成後設為1。
 *	delay_done: 標記延遲是否完成。用於在特定的時間延遲操作時，等待延遲過程的結束。
 *	tick:       用來計數定時器滴答的數量。每當定時器中斷觸發時，tick會增加。這有助於追蹤時間流逝。
 *	dlay:       設定的延遲時間，用於確保在適當的時機進行操作，通常在定時器中進行遞減。
 *	en:         用來確認當下傳輸或接收的是不是最後一筆資料，f代表不是。
 *	I2C_rxbuff: 指向接收數據緩衝區的pointer，將接收到的數據存儲在這個緩衝區中。
 */
#include "I2C.h"

uint8_t tran_done=1,read_done=1,ind=0,cmd=0,pot=1,I2C_txbuff=0,errow=0,
start_done=1,stop_done=1,delay_done=1,tick = 0,dlay=0;
char en='f';
uint8_t *I2C_rxbuff;
void delay(uint32_t time)
{
	delay_done = 0;
	dlay = time;
	cmd = 6;
	TIM1 -> CR1 |= (1);
}
void tim_init(void)
{
	RCC -> APB2ENR |= (1);
	TIM1 -> CR1 |= (1<<7);
	TIM1 -> DIER |= (1);
	TIM1 -> PSC |= 89; // 設定預分頻器`,頻率是1M
	TIM1 -> ARR &= ~(0xffff);
	TIM1 -> ARR |= 4;// 設定自動重裝值為4，計時器每4個時鐘週期觸發一次`,所以目前頻率是100K
	TIM1 -> EGR |= (1);
	HAL_NVIC_SetPriorityGrouping(4);
	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}
void TIM1_UP_TIM10_IRQHandler(void)
{
	TIM1 -> SR &= ~(1);
	tick++;
	dlay--;
	switch(cmd)
	{
		case 0: // 起始信號
		{
			(!(pot))? (GPIOA->ODR |= (0x0040)) : (GPIOA->ODR &= (0xffbf)); // 切換SCL引腳狀態
			pot = !pot;
			start_done = 1;// 標記起始信號完成
			break;
		}
		case 1:// 進行傳輸
		{
			(!(pot))? (GPIOA->ODR |= (0x0040)) : (GPIOA->ODR &= (0xffbf));
			pot = !pot;
			tim_tran();
			break;
		}
		case 2:// 傳輸完成
		{
			(!(pot))? (GPIOA->ODR |= (0x0040)) : (GPIOA->ODR &= (0xffbf));
			pot = !pot;
			tran_done = 1;// 標記傳輸完成
			break;
		}
		case 3: // 停止信號
		{
			(!(pot))? (GPIOA->ODR |= (0x0040)) : (GPIOA->ODR &= (0xffbf));
			pot = !pot;
			tim_stop();
			break;
		}
		case 4:// 讀取數據
		{
			(!(pot))? (GPIOA->ODR |= (0x0040)) : (GPIOA->ODR &= (0xffbf));
			pot = !pot;
			tim_read();
			break;
		}
		case 5:// 讀取完成
		{
			(!(pot))? (GPIOA->ODR |= (0x0040)) : (GPIOA->ODR &= (0xffbf));
			pot = !pot;
			read_done = 1;// 標記讀取完成
			break;
		}
		case 6:// 延遲完成
		{
			if(dlay==0)
			{
				delay_done = 1;// 標記延遲完成
				TIM1 -> CR1 &= ~(1); // 停止定時器
			}
		}
	}
}
void I2C_start(void)
{
	cmd = 0;// 設定命令為起始信號
	start_done = 0;
	pot=1;
	GPIOA -> MODER |= (1<<14);
	GPIOA -> ODR &= ~(1<<7);//將sda拉低
	TIM1 -> CR1 |= (1); // 啟動定時器
}
void I2C_stop(void)
{
	cmd = 3;// 設定命令為停止信號
	stop_done = 0;
	GPIOA -> MODER |= (1<<14);//將sda轉為輸出
	GPIOA->ODR &= ~(1<<7);//將sda拉低
}
void tim_stop(void)
{
	int c=TIM1->CNT+1;
	while(TIM1->CNT!=c);
	GPIOA -> ODR |= (1<<7);
	c=TIM1->CNT+1;
	while(TIM1->CNT!=c);
	TIM1 -> CR1 &= ~(1);
	errow = 0;
	pot=1;
	stop_done = 1;
}
void I2C_tran(uint8_t tx)
{
	TIM1 -> CR1 |= (1);
	GPIOA -> MODER |= (1<<14);
	I2C_txbuff = tx;// 設置傳送的數據
	cmd = 1;
	tran_done = 0;
	ind = 0;
	tim_tran();
}
void tim_tran(void)
{
	if(ind!=8 && pot== 0)
	{
		(I2C_txbuff&0x80)? (GPIOA->ODR |= (1<<7)) : (GPIOA->ODR &= ~(1<<7));
		I2C_txbuff = I2C_txbuff<<1;
	}
	else if(ind==8 && pot==0)GPIOA -> MODER &= ~(0X3<<14);
	else if(ind==8 && pot==1)//接收ack訊號
	{
		if((GPIOA->IDR & 0x80)) errow=1;//沒收到ack訊號，傳輸發生錯誤
		cmd = 2;// 設定命令為傳輸完成
	}
	else ind++;
}
void I2C_read(uint8_t *rx, char end)
{
	en = end;
	GPIOA -> MODER &= ~(0X3<<14);
	I2C_rxbuff = rx;
	cmd = 4;
	read_done = 0;
	ind = 0;
}
void tim_read(void)
{
	if(ind!=8 && pot== 1)
	{
		if(GPIOA->IDR&0x80) (*I2C_rxbuff|=1);// 如果SDA為高，將接收緩衝區的對應位設為1
		if(ind!=7)*I2C_rxbuff = *I2C_rxbuff<<1;
		ind++;
	}
	else if(ind==8 && pot==0)
	{
		GPIOA -> MODER |= (1<<14);
		if(en == 'f') GPIOA -> ODR &= ~(1<<7);//傳送ack訊號
		else GPIOA -> ODR |= (1<<7);//最後一筆資料傳nack
	}
	else if(ind==8 && pot==1) cmd = 5;
}
