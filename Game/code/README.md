程式主要是透過操作板子的register完成，參考stm32f429i-disc1的reference manual。  
其中，Error_Handler(void)和assert_failed(uint8_t *file, uint32_t line)是stm32cubeide自動生成的。  
SD.h,SD.c實作sd卡的初始化和讀資料等操作，LCD.h,LCD.c實作LCD的初始化,顯示圖形,顯示字串等操作。   
ASCII.h是我用應用程式提取的 16X16 ASCII 字模，Music.h,Music.c是撥放和切換音樂的程式，  
Tetris.h,Tetris.c整合所有程式來實作俄羅斯方塊的遊戲。  

