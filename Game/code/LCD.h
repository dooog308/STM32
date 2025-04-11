
#ifndef INC_LCD_H_
#define INC_LCD_H_

/*
 * LCD 控制標頭檔
 *
 * 函式說明：
 *
 * void rst(uint8_t t);
 *   控制 RESET 腳位，t=1 釋放，t=0 重置
 *
 * void dc(uint8_t t);
 *   控制 DC（Data/Command）腳位，t=1 表示資料，t=0 表示指令
 *
 * void cs(uint8_t t);
 *   控制 CS（Chip Select）腳位，t=1 釋放，t=0 選取
 *
 * void spi_swapData(uint8_t data);
 *   透過 SPI 傳送一個 8-bit 的資料
 *
 * void spi_16bit(uint16_t data);
 *   透過 SPI 傳送一個 16-bit 資料（通常為 RGB565 顏色資料
 *
 * void LCD_WriteCommand(uint8_t cmd);
 *   寫入 LCD 的指令（DC=0 模式）
 *
 * void LCD_WriteByte(uint8_t data);
 *   寫入 LCD 的資料（DC=1 模式）
 *
 * void LCD_init(void);
 *   初始化 LCD
 *
 * void set_area(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye);
 *   設定顯示區域（從 xs, ys 到 xe, ye）
 *
 * void fill_area(uint16_t rgb);
 *   填滿目前設定區域的顏色（RGB565 格式）
 *
 * void LCD_printf(char *str, uint16_t rgb);
 *   在畫面上顯示字串（搭配 cursor 指定位置，使用指定顏色）
 *
 * 變數：
 *
 * extern uint8_t cursor[2];
 *   用來記錄游標在 LCD 上的 X 與 Y 位置，配合顯示函式使用
 */

#include "stm32f4xx_hal.h"
#include <string.h>

extern uint8_t cursor[2];
void rst(uint8_t t);
void dc(uint8_t t);
void cs(uint8_t t);
void spi_swapData(uint8_t data);
void spi_16bit(uint16_t data);
void LCD_WriteCommand(uint8_t cmd);
void LCD_WriteByte(uint8_t data);
void LCD_init(void);
void set_area(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye);
void fill_area(uint16_t rgb);
void LCD_printf(char *str, uint16_t rgb);
#endif /* INC_LCD_H_ */
