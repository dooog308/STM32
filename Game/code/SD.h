/*
 * SD 卡操作標頭檔（透過 SPI 模式）
 *
 * 函式說明：
 *
 * void SD_cs(uint8_t t);
 *   控制 SD 卡的 CS（Chip Select）腳位
 *   t = 0：選取（啟用 SD）
 *   t = 1：釋放（取消選取）
 *
 * uint8_t SD_spi_send(uint8_t data);
 *   傳送 1 個 byte 資料至 SD，並接收回傳資料（full-duplex SPI 傳輸）
 *   傳入：要傳送的 8-bit 資料
 *   回傳：SD 回傳的資料
 *
 * uint8_t SD_spi_read(void);
 *   從 SD 卡讀取 1 個 byte（寫入 0xFF）
 *   回傳：SD 回傳的資料
 *
 * void SD_sendcmd(uint8_t cmd, uint32_t arg, uint8_t crc);
 *   傳送一個 SD 命令（CMD0, CMD17 等）
 *   cmd：命令碼
 *   arg：命令參數（32-bit）
 *   crc：CRC 檢查值（SPI 幾乎不使用）
 *
 * uint8_t SD_init(void);
 *   初始化 SD 卡
 *   回傳：1 表示成功，其它值表示失敗
 *
 * uint8_t SD_read(uint8_t *buffer, uint64_t addr, uint16_t blocksize);
 *   從 SD 卡讀取一個資料區塊
 *   buffer：儲存讀取資料的緩衝區
 *   addr：SD 卡地址（位元組位址或區塊位址）
 *   blocksize：每個區塊大小（一般為 512）
 *   回傳：1 表示成功，其它值表示錯誤
 *
 * void SD_error(void);
 *   SD 操作失敗時可呼叫此函式執行錯誤處理（例如顯示錯誤畫面）
 */
#ifndef INC_SD_H_
#define INC_SD_H_
#include "stm32f4xx_hal.h"
#include "main.h"

void SD_cs(uint8_t t);
uint8_t SD_spi_send(uint8_t data);
uint8_t SD_spi_read(void);
void SD_sendcmd(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t SD_init(void);
uint8_t SD_read(uint8_t *buffer,uint64_t addr,uint16_t blocksize);
void SD_error(void);
#endif
