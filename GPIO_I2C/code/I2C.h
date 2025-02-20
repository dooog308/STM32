/*
 * I2C.h
 *
 *  Created on: Feb 19, 2025
 *      Author: User
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_
#include "main.h"

// delay 函式: 用來實現延遲，參數 time 是延遲的時間長度
extern void delay(uint32_t time);
// I2C_start 函式: 用來發送 I2C 啟動信號
extern void I2C_start(void);
// I2C_stop 函式: 用來發送 I2C 停止信號
extern void I2C_stop(void);
// I2C_tran 函式: 用來發送資料（byte）到從機，傳回值為傳輸成功或失敗的狀態
extern uint8_t I2C_tran(uint8_t tx);
// I2C_read 函式: 用來從 I2C 裝置讀取資料，rx 是用來儲存讀取結果的變數，end 用來指示是否為最後一次讀取
extern void I2C_read(uint8_t *rx, char end);

#endif /* INC_I2C_H_ */
