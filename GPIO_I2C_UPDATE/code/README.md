程式主要是透過操作板子的register完成，參考stm32f429i-disc1的reference manual。  
其中，Error_Handler(void)和assert_failed(uint8_t *file, uint32_t line)是stm32cubeide自動生成的。  
是以之前的"GOIO_I2C"作為框架，改變實作方式。    
新增的程式主要是I2C.h和I2C.c。
