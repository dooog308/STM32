/*
 * AHT10.h
 *
 *  Created on: Feb 20, 2025
 *      Author: User
 */

#ifndef INC_AHT10_H_
#define INC_AHT10_H_

#include "I2C.h"
#include "main.h"

// 定義AHT10的I2C位址
#define AHT10_ADDR    0X70

// 函式宣告：初始化AHT10感測器
// 參數：
// I2C_rxbuff - 用於接收AHT10初始化狀態的緩衝區
void AHT10_init(uint8_t *I2C_rxbuff);
// 函式宣告：測量溫濕度數據
// 參數：
// temp - 儲存溫度測量結果
// hum  - 儲存濕度測量結果
// I2C_rxbuff - 用於存放從AHT10讀取的資料的緩衝區
void AHT10_measure(uint32_t* temp, uint32_t* hum, uint8_t *I2C_rxbuff);
// 函式宣告：讀取AHT10的測量資料
// 參數：
// I2C_rxbuff - 用於存放從AHT10讀取的資料的緩衝區
void AHT10_read(uint8_t *I2C_rxbuff);

#endif /* INC_AHT10_H_ */
