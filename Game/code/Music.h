/*
 * Music.h
 *
 * Created on: Mar 22, 2025
 * Author: User
 *
 * 功能說明：
 * 控制從 SD 卡撥放音樂的流程，搭配 DMA 將資料傳送到 DAC 播放音效。
 * 撥放邏輯流程：
 *   1. 從 SD 卡讀取一個 block（512 bytes）資料到 buff
 *   2. 使用 DMA 傳送 buff 資料到 DAC（類比音訊輸出）
 *   3. DMA 完成後發出置起 flag，外部程式檢查 flag 狀態
 *   4. 確認完成後再讀下一個 block，如此循環
 *
 * 宏定義：
 *   fight1         - fight1 音樂在 SD 卡中的起始地址
 *   fight2         - fight2 音樂在 SD 卡中的起始地址
 *   victory        - victory 音樂在 SD 卡中的起始地址
 *   buff_size      - 每次傳送的資料大小（512 bytes）
 *   fight2_size    - fight2 音樂的 block 數（1916 blocks）
 *   victory_size   - victory 音樂的 block 數（66 blocks）
 *
 * 外部變數：
 *   uint8_t flag         - 撥放流程中用來檢查 DMA 是否完成
 *   uint8_t cur_Music    - 紀錄目前正在播放哪一首音樂
 *   uint8_t buff[512]    - 音樂資料暫存 buffer
 *   uint16_t fight_ind   - fight2 音樂播放進度（目前讀到第幾 block）
 *   uint16_t victory_ind - victory 音樂播放進度
 *
 * 函式：
 *   void play_Music(uint8_t music);
 *     依據傳入參數選擇並開始撥放指定音樂（fight1、fight2、victory）
 */

#ifndef INC_MUSIC_H_
#define INC_MUSIC_H_

#include "Tetris.h"
#include "SD.h"

#define fight1    		  0x001008020
#define fight2    		  0x001156020
#define victory   	      0x00114c020
#define buff_size         512
#define fight2_size       1916
#define victory_size      66

extern uint8_t flag, cur_Music;
extern uint8_t buff[512];
extern uint16_t fight_ind, victory_ind;
void play_Music(uint8_t music);



#endif /* INC_MUSIC_H_ */
