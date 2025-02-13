
#include "main.h"
#include <string.h>
#include <AHT10.h>//我寫的AHT10感測器驅動程式

// 配置系統時鐘
void clock_init(void);
// 初始化GPIO
void gpio_init(void);
// 初始化I2C
void i2c_init(void);
// 初始化USART
void usart_init(void);
//初始化用於pwm的時鐘
void tim_init(void);
// 初始化用於外部中斷的按鈕
void button_exti_init(void);
// 外部中斷處理程序，當外部中斷觸發時會進入此函數
// 這裡會呼叫USER_BUTTON函式來觸發溫度測量
void EXTI0_IRQHandler(void);
// 當用戶按下按鈕時，觸發此函式進行溫度測量
// 用戶按鈕的功能為啟動溫濕度感測器測量
void USER_BUTTON(void);
// 通過UART傳送字元資料（字符）
// 參數：tx - 要傳送的字元
void PRINTFC(char* tx);
// 通過UART傳送32位無符號整數
// 參數：tx - 要傳送的數字
void PRINTFI(uint32_t tx);
//overflow中斷
void TIM1_UP_TIM10_IRQHandler(void);

uint32_t temp=0,hum=0;//溫度和濕度
uint8_t I2C_rxbuff[6]={0};
uint16_t DUTY = 2500;//控制亮度的參數
uint8_t st =0;//控制變亮或變暗

void TIM1_UP_TIM10_IRQHandler(void)
{
	TIM1 -> SR &= ~(1);
	if(st)
	{
		DUTY--;
		TIM1 -> CCR1 = DUTY;
		if(DUTY == 0)
			st=0;
	}
	else
	{
		DUTY++;
		TIM1 -> CCR1 = DUTY;
		if(DUTY == 5000)
			st=1;
	}
}
void EXTI0_IRQHandler(void)
{

	if((GPIOA->IDR&(0x0001)))
	{
		USER_BUTTON();
	}
	EXTI -> PR |= (1);
}
void USER_BUTTON(void)
{
	if((GPIOA->IDR&(0x0001)))
	{
		AHT10_measure(&temp, &hum, I2C_rxbuff);
	}
}
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
int main(void)
{
  HAL_Init();//為了使用HAL_Delay()
  clock_init();
  gpio_init();
  usart_init();
  i2c_init();
  button_exti_init();
  HAL_Delay(21);//根據AHT10的datasheet，上電後要等最多20ms
  AHT10_init(I2C_rxbuff);
  tim_init();
  PRINTFC("Push user button to measure.\r\n");
  while (1)
  {

  }
}
/*使用HSE
 * SYSCLK= 72MHZ
 * HCLK=   72MHz
 * PCLK1=  36MHz
 * PCLK2=  72MHz
 */
void clock_init(void)
{
	RCC -> CR |= (1<<16);//HSE ENABLE
	while(!(RCC -> CR & 0x20000));//wait
	FLASH -> ACR |= (1<<8);//prefetch enable
	FLASH -> ACR |= (0x1);
	RCC -> CFGR |= (0x4<<10);
	RCC -> PLLCFGR &= ~(0x7fff);
	RCC -> PLLCFGR |= (1<<22);
	RCC -> PLLCFGR |= (0x48<<6);
	RCC -> PLLCFGR |= (0x4);
	RCC -> CR |= (1<<24);//PLL enable
	while(!(RCC -> CR & 0x2000000));//wait
	RCC -> CFGR |= (0x2);
}
void gpio_init(void)
{
	RCC -> AHB1ENR |= (1);//GPIOA CLOCK
	RCC -> AHB1ENR |= (1<<1);//GPIOB CLOCK
	//usart tx
	GPIOA -> MODER |= (0x2<<18);//alternate mode
	GPIOA -> OSPEEDR |= (0x3<<18);//speed high
	GPIOA -> AFR[1] |= (0X7<<4);//alternative function
	//i2c SDA
	GPIOB -> MODER |= (0x2<<18);
	GPIOB -> PUPDR |= (0x1<<18);//上拉
	GPIOB -> OTYPER |= (1<<9);//open drain
	GPIOB -> AFR[1] |= (0x4<<4);
	//SCL
	GPIOB -> MODER |= (0x2<<16);
	GPIOB -> PUPDR |= (0x1<<16);//上拉
	GPIOB -> OTYPER |= (1<<8);//open drain
	GPIOB -> AFR[1] |= (0x4);
	//user_button
	GPIOA -> PUPDR |= (0X2);//下拉
	//tim1_channl1
	GPIOA -> MODER |= (0x2<<16);
	//GPIOA -> PUPDR |= (0x2<<16);
	GPIOA -> OSPEEDR |= (0x3<<16);
	GPIOA -> AFR[1] |= (1);
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
}
void i2c_init(void)
{
	RCC -> APB1ENR |= (1<<21);//CLOCK
	I2C1 -> CR2 |= (0x08);//I2C1的頻率設置
	I2C1 -> CCR |= (0X80);//I2C1的頻率設置
	//設定,enable
	I2C1 -> CR1 |= (1);
}
void tim_init(void)
{
	RCC -> APB2ENR |= (1);//clock
	TIM1 -> CR1 |= (1<<7);//preload
	TIM1 -> CR1 |= (0x2<<8);
	TIM1 -> DIER |= (1);//it
	TIM1 -> CCMR1 |= (0x6<<4);//PWM_mode1
	TIM1 -> CCMR1 |= (1<<3);//preload
	//TIM1 -> CCER |= (1<<1);//active high
	TIM1 -> CCER |= (1<<2);
	TIM1 -> PSC = 1;
	TIM1 -> ARR = DUTY*2;
	TIM1 -> CCR1 = DUTY;
	TIM1 -> EGR |= (1);
	TIM1 -> CCER |= (1);//ch1 enable
	TIM1 -> BDTR |= (1<<15);
	TIM1 -> CR1 |= (1);
	HAL_NVIC_SetPriorityGrouping(4);
	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}
void button_exti_init(void)
{
	EXTI -> IMR |= (1);
	EXTI -> RTSR |= (1);//電流上升時觸發
	HAL_NVIC_SetPriorityGrouping(4);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 2);  //將SysTick_IRQn的中斷順位調高，
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);//這樣才能正常使用HAL_Delay()
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
