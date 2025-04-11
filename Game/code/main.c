
#include "main.h"
#include <string.h>
#include "wave.h"
#include "LCD.h"
#include "SD.h"
#include "Music.h"
#include "Tetris.h"


void clock_init(void);
void tim_init(void);
void gpio_init(void);
void usart_init(void);
void dac_init(void);
void dma_init(void);
void spi_init(void);
void TIM6_DAC_IRQHandler(void);
void USART1_IRQHandler(void);
void PRINTFC(char* tx);
void PRINTFI(uint32_t tx);


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
void USART1_IRQHandler(void)
{
	char rx=0;
	rx = USART1->DR;
	if(USART1->SR&0X8) rx = USART1->DR;
	command = rx;
	USART1->DR = rx;
}
int main(void)
{
  HAL_Init();
  clock_init();
  tim_init();
  gpio_init();
  usart_init();
  spi_init();
  SD_init();
  dac_init();
  dma_init();
  tim1_init();
  LCD_init();
  start();
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
	RCC -> PLLCFGR |= (0x48<<6);
	RCC -> PLLCFGR |= (0x4);
	RCC -> CFGR |= (0x4<<10);
	RCC -> CR |= (1<<24);
	while(!(RCC->CR & 0X2000000));
	RCC -> CFGR |= (0X2);
}
void gpio_init(void)
{
	RCC -> AHB1ENR |= (1);//GPIOA CLOCK
	RCC -> AHB1ENR |= (1<<1);
	RCC -> AHB1ENR |= (1<<2);
	//usart tx
	GPIOA -> MODER |= (0x2<<18);//alternate mode
	GPIOA -> OSPEEDR |= (0x3<<18);//speed high
	GPIOA -> AFR[1] |= (0X7<<4);//alternative function
	//uart rx
	GPIOA -> MODER |= (0x2<<20);//alternate mode
	GPIOA -> OSPEEDR |= (0x3<<20);//speed high
	GPIOA -> AFR[1] |= (0X7<<8);//alternative function
	//dac pa4
	GPIOA->MODER |= (0X3<<8);
	GPIOA->OSPEEDR |= (0X3<<8);
	//SCK  PA5
	GPIOA->ODR |= (1<<5);
	GPIOA->MODER |= (0X2<<10);
	GPIOA->AFR[0] |= (0X5<<20);
	GPIOB->ODR |= (1<<3);
	GPIOB->MODER |= (0X2<<6);
	GPIOB->AFR[0] |= (0X5<<12);
	//MOSI PA7
	GPIOA->ODR |= (1<<7);
	GPIOA->MODER |= (0X2<<14);
	GPIOA->AFR[0] |= (0X5<<28);
	GPIOB->ODR |= (1<<5);
	GPIOB->MODER |= (0X2<<10);
	GPIOB->AFR[0] |= (0X5<<20);
	//miso pa6
	GPIOA->ODR |= (1<<6);
	GPIOA->MODER |= (0X2<<12);
	GPIOA->AFR[0] |= (0X5<<24);
	//dc pC0
	GPIOC->MODER |= (0X1);
	GPIOC->OSPEEDR |= (0X3);
	GPIOC->ODR |= (1);
	//CS PA3
	GPIOA->MODER |= (0X1<<6);
	GPIOA->OSPEEDR |= (0X3<<6);
	GPIOA->ODR |= (1<<3);
	//RST PA0
	GPIOA->MODER |= (0X1);
	GPIOA->OSPEEDR |= (0X3);
	GPIOA->ODR |= (1);
	//sd cs PC4
	GPIOB->MODER |= (0X1<<2);
	GPIOB->OSPEEDR |= (0X3<<2);
	GPIOB->ODR |= (1<<1);
}
void usart_init(void)
{
	RCC->APB2ENR |= (1<<4);//clock
	USART1 -> BRR = (0x4d<<4 | 5);//USART的頻率設置
	//設定,enable
	USART1 -> CR1 |= (1<<15);
	USART1 -> CR1 |= (1<<5);
	USART1 -> CR1 |= (1<<3);
	USART1 -> CR1 |= (1<<2);
	USART1 -> CR1 |= (1<<13);
	HAL_NVIC_SetPriorityGrouping(4);
	HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}
void tim_init(void)
{
	RCC->APB1ENR |= (1<<4);
	TIM6->CR1 |= (1<<7);
	TIM6->CR2 |= (0x2<<4);
	TIM6->DIER |= (1);
	TIM6->PSC = 0;
	TIM6->ARR &= ~(0XFFFF);
	TIM6->ARR = (72000000/8000);
	TIM6->EGR |= (1);
	HAL_NVIC_SetPriorityGrouping(4);
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 1);
	//HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
void dac_init(void)
{
	RCC->APB1ENR |= (1<<29);
	DAC->CR |= (0x7<<24);
	DAC->CR |= (0x1<<18);
	DAC->CR |= (0x7<<8);
	DAC->CR |= (0x1<<2);
	DAC->CR |= (0X1<<12);
	DAC->CR |= (0x1);
	TIM6->CR1 |= (1);
}
void dma_init(void)
{
	RCC->AHB1ENR |= (0x1<<21);
	//dma1 ch7 st56
	DMA1_Stream5->CR |= (0x7<<25);
	DMA1_Stream6->CR |= (0X7<<25);
	DMA1_Stream5->CR |= (0x1<<10);
	DMA1_Stream6->CR |= (0x1<<10);
	DMA1_Stream5->CR |= (0x1<<6);
	DMA1_Stream6->CR |= (0x1<<6);
	DMA1_Stream5->CR |= (0x1<<4);
	DMA1_Stream6->CR |= (0x1<<4);
	DMA1_Stream5->NDTR = 512;
	DMA1_Stream6->NDTR = 512;
	//DMA1_Stream5->M0AR = buff;
	//DMA1_Stream6->M0AR = buff;
	DMA1_Stream5->PAR = (uint32_t*)&(DAC->DHR8R1);
	DMA1_Stream6->PAR = (uint32_t*)&(DAC->DHR8R2);
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 1, 1);
	//HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	//DMA1_Stream5->CR |= (0x1);
}
void spi_init(void)
{
	RCC->APB2ENR |= (0X1<<12);
	RCC->APB2ENR |= (0X1<<20);
	SPI1->CR1 |= (0X1<<9);
	SPI1->CR1 |= (0X7<<3);
	SPI1->CR1 |= (0X1<<2);
	SPI1->CR1 |= (0X1<<8);
	SPI1->CR1 |= (0X1<<1);
	SPI1->CR1 |= (0X1);
	SPI1->CR1 |= (0X1<<6);
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
