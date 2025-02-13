
#ifndef INC_AHT10_H_
#define INC_AHT10_H_

#include "main.h"
// 定義AHT10的I2C位址
#define AHT10_ADDR     0x70
// 函式宣告：發送I2C位址與指令到設備
// 參數：
// addr - 目標設備的I2C地址
// cmd  - 讀或寫(讀傳'r'，寫傳'w')
void I2C_sendaddr(uint8_t addr, char cmd);
// 函式宣告：發送資料至設備
// 參數：
// data - 要發送的資料
// end  - 發送結束信號（如最後一個字節）
void I2C_senddata(uint8_t data, char end);
// 函式宣告：讀取設備傳至I2C的資料
// 參數：
// I2C_rxbuff - 儲存接收到資料的緩衝區
void I2C_readdata(uint8_t *I2C_rxbuff);
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
// 函式宣告：重置AHT10感測器
void AHT10_reset(void);


#endif
