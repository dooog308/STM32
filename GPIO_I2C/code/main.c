
#include "main.h"
#include "I2C.h"
#include "AHT10.h"
#include <string.h>

void clock_init(void);
void gpio_init(void);
void usart_init(void);
void button_init(void);
void user_button(void);
void EXTI0_IRQHandler(void);
void PRINTFC(char* tx);
void PRINTFI(uint32_t tx);

uint32_t temp=0,hum=0;
uint8_t rx[6] = {0};
/*
 * GPIOA -> ODR約200ns
 * a=a+1 <100ns
 * if (==)<100ns
 * for()第一次約244ns,之後等差156ns
 */
void PRINTFC(char* tx)
{
	int len = strlen(tx);
	while(len)
	{
		while(!(USART1->SR&0X40));//等待傳送完成
		USART1->DR = *tx;
		tx++;
		len--;
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
		AHT10_measure(&temp, &hum, rx);
}
int main(void)
{
	clock_init();
	gpio_init();
	usart_init();
	button_init();
	delay(134615);
	PRINTFC("GPIO_I2C\r\n");
	PRINTFC("Push user button to measure.\r\n");
	AHT10_init(rx);
    while (1)
    {

    }
}
void clock_init(void)
{
	RCC -> CR |= (1<<16);
	while(!(RCC->CR & 0X20000));
	FLASH -> ACR |= (1<<8);//prefetch enable
	FLASH -> ACR |= (0x1);
	RCC -> PLLCFGR &= ~(0x7fff);
	RCC -> PLLCFGR |= (1<<22);
	RCC -> PLLCFGR |= (0x5a<<6);
	RCC -> PLLCFGR |= (0x4);
	RCC -> CFGR |= (0x4<<10);
	RCC -> CR |= (1<<24);
	while(!(RCC->CR & 0X2000000));
	RCC -> CFGR |= (0X2);
}
void gpio_init(void)
{
	RCC -> AHB1ENR |= (1);//GPIOA CLOCK
	//RCC -> AHB1ENR |= (1<<1);//GPIOB CLOCK
	//usart tx
	GPIOA -> MODER |= (0x2<<18);//alternate mode
	GPIOA -> OSPEEDR |= (0x3<<18);//speed high
	GPIOA -> AFR[1] |= (0X7<<4);//alternative function
	//sda_PA7
	GPIOA -> MODER |= (1<<14);
	GPIOA -> OSPEEDR |= (0x3<<14);
	GPIOA -> OTYPER |= (1<<7);
	GPIOA -> PUPDR |= (1<<14);
	GPIOA -> ODR |= (1<<7);
	//scl_PA6
	GPIOA -> MODER |= (1<<12);
	GPIOA -> OSPEEDR |= (0x3<<12);
	GPIOA -> OTYPER |= (1<<6);
	GPIOA -> PUPDR |= (1<<12);
	GPIOA -> ODR |= (1<<6);
	//USER BUTTON
	GPIOA -> PUPDR |= (0x2);

}
void usart_init(void)
{
	RCC->APB2ENR |= (1<<4);//clock
	USART1 -> BRR = (0x61<<4 | 0);//USART的頻率設置
	//設定,enable
	USART1 -> CR1 |= (1<<15);
	USART1 -> CR1 |= (1<<5);
	USART1 -> CR1 |= (1<<3);
	USART1 -> CR1 |= (1<<2);
	USART1 -> CR1 |= (1<<13);
}
void button_init(void)
{
	EXTI -> IMR |= (1);
	EXTI -> RTSR |= (1);
	HAL_NVIC_SetPriorityGrouping(4);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
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
