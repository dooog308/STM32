程式主要是透過操作板子的register完成，參考stm32f429i-disc1的reference manual。  
其中，Error_Handler(void)和assert_failed(uint8_t *file, uint32_t line)是stm32cubeide自動生成的。  
是以之前的"AHT10"作為框架，新增呼吸燈的功能。  
新增的程式主要是void tim_init(void)和void TIM1_UP_TIM10_IRQHandler(void)
