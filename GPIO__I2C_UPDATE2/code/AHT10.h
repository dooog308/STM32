
#ifndef INC_AHT10_H_
#define INC_AHT10_H_

#include "I2C.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "queue.h"

// 定義AHT10的I2C位址
#define AHT10_ADDR    0X70

// 函式宣告：初始化AHT10感測器
void AHT10_init(void * pvParameters);
// 函式宣告：測量溫濕度數據
void AHT10_measure(void * pvParameters);
// 函式宣告：讀取AHT10的測量資料
void AHT10_read(void * pvParameters);

#endif /* INC_AHT10_H_ */
