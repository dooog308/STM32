
#include "main.h"
#include <string.h>
#include "AHT10.h"
#include "I2C.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void clock_init(void);
void gpio_init(void);
void uart_init(void);
void tim_init(void);
void button_init(void);
void user_button(void);
void EXTI0_IRQHandler(void);
void PRINTFC(void * pvParameters);
void PRINTFI(uint32_t tx);
void USART1_IRQHandler(void);
void TIM1_UP_TIM10_IRQHandler(void);
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );
void LED_BLINK(void * pvParameters);
void send(void * pvParameters);

uint16_t tick=0, s=0, LED_B=0, temp=0, hum=0;
uint8_t rxd[6]={0}, I2C_tx=0;
char txd[50]="hehe\r\n\0", end='f';
void* t=txd;

TaskHandle_t PC_Handle=NULL, I2C_tran_Handle, I2C_start_Handle, I2C_stop_Handle,
I2C_read_Handle, AHT10_init_Handle, AHT10_read_Handle, AHT10_measure_Handle;
QueueHandle_t q1_Handle=NULL;

uint8_t uart_tran_done=0;
void PRINTFC(void * pvParameters)
{
	uint16_t len=0;
	char *tx;
	while(1)
	{
		uart_tran_done = 0;
		xQueueReceive(q1_Handle, &tx, 20);
		len = strlen(tx);
		while(len)
		{
			while(!(USART1->SR&0X40));//等待傳送完成
			USART1->DR = *tx;
			tx++;
			len--;
		}
		uart_tran_done = 1;
		vTaskSuspend(PC_Handle);
	}
}
void PRINTFI(uint32_t tx)
{
	if(tx/10!=0) PRINTFI(tx/10);
	while(!(USART1 -> SR&0X40));//等待傳送完成
	USART1 -> DR = (tx%10)+'0';
}
void EXTI0_IRQHandler(void)
{
	if(GPIOA->IDR & 1) user_button();
	EXTI -> PR |= (1);
}
void user_button(void)
{
	if(GPIOA->IDR & 1)
		vTaskResume(AHT10_measure_Handle);
}
void send(void * pvParameters)
{
	char rx[2]="\0\0";
	void *r=rx;
	while(1)
	{
		if(USART1->SR&0x20)
		{
			rx[0] = USART1->DR;
			xQueueSend(q1_Handle, &r, 10);
			vTaskResume(PC_Handle);
		}
		vTaskDelay(10);
	}
}
void TIM1_UP_TIM10_IRQHandler(void)
{
	tick++;
	TIM1->SR &= ~(1);
}
int main(void)
{
  clock_init();
  uart_init();
  tim_init();
  gpio_init();
  button_init();
  //AHT10_init(rxd);
  q1_Handle = xQueueCreate(10, sizeof(void*));//用來讓uart傳送東西的queue
  xTaskCreate(PRINTFC, "printfc", 100, (void*)1, 2, &PC_Handle);
  xTaskCreate(LED_BLINK, "LED", 100, (void*)1, 1, NULL);
  xTaskCreate(send, "send", 100, (void*)1, 2, NULL);
  xTaskCreate(I2C_start, "I2C_start", 100, (void*)1, 2, &I2C_start_Handle);
  xTaskCreate(I2C_stop, "I2C_stop", 100, (void*)1, 2, &I2C_stop_Handle);
  xTaskCreate(I2C_tran, "I2C_tran", 100, (void*)1, 2, &I2C_tran_Handle);
  xTaskCreate(I2C_read, "I2C_read", 100, (void*)1, 2, &I2C_read_Handle);
  xTaskCreate(AHT10_init, "AHT10_init", 100, (void*)1, 2, &AHT10_init_Handle);
  xTaskCreate(AHT10_read, "AHT10_read", 100, (void*)1, 2, &AHT10_read_Handle);
  xTaskCreate(AHT10_measure, "AHT10_measure", 100, (void*)1, 2, &AHT10_measure_Handle);
  vTaskSuspend(PC_Handle);
  vTaskSuspend(I2C_start_Handle);
  vTaskSuspend(I2C_stop_Handle);
  vTaskSuspend(I2C_tran_Handle);
  vTaskSuspend(I2C_read_Handle);
  vTaskSuspend(AHT10_read_Handle);
  vTaskSuspend(AHT10_measure_Handle);
  vTaskStartScheduler();

  while (1)
  {

  }
}
void clock_init(void)
{
	RCC->CR |= (1<<16);
	while(RCC->CR&0x20000);
	FLASH->ACR |= (1<<8);
	FLASH->ACR |= (0x1);
	RCC -> PLLCFGR &= ~(0x7fff);
	RCC->PLLCFGR |= (1<<22);
	RCC->PLLCFGR |= (0x48<<6);
	RCC->PLLCFGR |= (0X4);
	RCC->CFGR |= (0X4<<10);
	RCC->CR |= (1<<24);
	while(RCC->CR&0x2000000);
	RCC->CFGR |= (0X2);
}
void gpio_init(void)
{
	RCC -> AHB1ENR |= (1);//GPIOA
	RCC->AHB1ENR |= (1<<6);//GPIOG
	//USART TX
	GPIOA->MODER |= (0X2<<18);
	GPIOA->OSPEEDR |= (0x3<<18);
	GPIOA->AFR[1] |= (0x7<<4);
	//USART RX
	GPIOA->MODER |= (0X2<<20);
	GPIOA->OSPEEDR |= (0x3<<20);
	GPIOA->AFR[1] |= (0x7<<8);
	//LED1,2
	GPIOG->MODER |= (0x1<<26);
	GPIOG->MODER |= (0x1<<28);
	//sda_PA7
	GPIOA->MODER |= (1<<14);
	GPIOA->OSPEEDR |= (0x3<<14);
	GPIOA->OTYPER |= (1<<7);
	GPIOA->PUPDR |= (1<<14);
	GPIOA->ODR |= (1<<7);
	//scl_PA6
	GPIOA->MODER |= (1<<12);
	GPIOA->OSPEEDR |= (0x3<<12);
	GPIOA->OTYPER |= (1<<6);
	GPIOA->PUPDR |= (1<<12);
	GPIOA->ODR |= (1<<6);
	//USER BUTTON
	GPIOA->PUPDR |= (0x2);
}
void uart_init(void)
{
	RCC->APB2ENR |= (1<<4);
	USART1->BRR = ((0x4d<<4) | 5);
	USART1->CR1 |= (1<<15);
	//USART1->CR1 |= (1<<5);
	USART1->CR1 |= (1<<2);
	USART1->CR1 |= (1<<3);
	HAL_NVIC_SetPriorityGrouping(4);
	//HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
	//HAL_NVIC_EnableIRQ(USART1_IRQn);
	USART1->CR1 |= (1<<13);
}
void tim_init(void)
{
	RCC->APB2ENR |= (1);
	TIM1->CR1 |= (1<<7);
	TIM1->DIER |= (1);
	TIM1->PSC = 71;
	TIM1->ARR = 999;
	TIM1->EGR |= (1);
	TIM1->EGR &= ~(1);
	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	TIM1->CR1 |= (1);
}
void button_init(void)
{
	EXTI -> IMR |= (1);
	EXTI -> RTSR |= (1);
	HAL_NVIC_SetPriorityGrouping(4);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
void LED_BLINK(void * pvParameters)
{
	while(1)
	{
		(LED_B)? (GPIOG->ODR &= ~(0x3<<13)) : (GPIOG->ODR |= (0x3<<13));
		LED_B = !LED_B;
		vTaskDelay(5000);
	}
}
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
void vApplicationTickHook( void )
{

}
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
