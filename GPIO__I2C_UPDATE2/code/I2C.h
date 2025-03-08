
#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

// I2C_start 函式: 用來發送 I2C 啟動信號
extern void I2C_start(void * pvParameters);
// I2C_stop 函式: 用來發送 I2C 停止信號
extern void I2C_stop(void * pvParameters);
// I2C_tran 函式: 用來發送資料（byte）到從機，傳回值為傳輸成功或失敗的狀態
extern void I2C_tran(void * pvParameters);
// I2C_read 函式: 用來從 I2C 裝置讀取資料，rx 是用來儲存讀取結果的變數，end 用來指示是否為最後一次讀取
extern void I2C_read(void * pvParameters);

#endif /* INC_I2C_H_*/
