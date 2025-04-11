/*
 * Tetris.h
 *
 * 俄羅斯方塊遊戲的核心程式，整合了時間控制、方塊繪製、碰撞檢查、遊戲邏輯等。
 *
 * 核心概念：
 *
 * 1. 方塊組成方式：
 *    方塊（block）是由 4 個基本單位（square）所組成，。
 *    每個方塊形狀使用 `uint16_t` 來儲存，每個 `block` 由四位（高四位代表第一行，低四位代表最後一行）來表示：
 *      - 每一位元為 1 代表該位置有'square'，0 代表該位置是空
 *    例如：L 型方塊 0xE800 表示為 1110 1000 0000 0000，即由四個 `square` 所組成，形成 L 形。
 *                               1110
 *                               1000
 *                               0000
 *                               0000
 *
 * 2. 時間與速度控制：
 *    透過 Timer 控制遊戲中方塊的下落速度及其他遊戲事件（如處理玩家操作、音樂切換）。
 *
 * 3. 函式說明：
 *    函式主要負責方塊生成、移動、旋轉、碰撞檢查、地圖刷新等遊戲邏輯。
 *
 * 全域變數：
 *   - uint16_t block[type_of_block]：存儲不同類型的方塊形狀（每個方塊是由四個 `square` 組成）
 *   - uint16_t color[type_of_block]：存儲每個方塊的顏色
 *   - uint16_t cur_color, cur_block：目前顯示的方塊顏色與類型
 *   - uint16_t map[top]：遊戲地圖，儲存每行的方塊資料，map 的每個元素是 16 位元，代表一行
 *                       1代表該位置有方塊，所以0XFFFF代表整行填滿了
 *   - uint16_t delay：時間延遲，用於控制方塊下落速度
 *   - uint8_t square_xy1[2], square_xy2[2]：當前方塊的座標（x, y）
 *   - uint8_t tick, command, flag, cur_Music：遊戲邏輯控制與音樂狀態
 *
 * 函式列表：
 *   void TIM1_UP_TIM10_IRQHandler(void);
 *     用於 Timer 中斷處理，控制遊戲時間與事件
 *
 *   void tim1_init(void);
 *     初始化 Timer 1，用來控制方塊的下落速度等
 *
 *   void start(void);
 *     開始遊戲，初始化遊戲狀態與方塊
 *
 *   void end(void);
 *     結束遊戲，顯示遊戲結束畫面
 *
 *   void draw_square(uint8_t x, uint8_t y, uint16_t rgb);
 *     在畫面上指定位置繪製單一方塊（`square`）
 *
 *   void create_block(void);
 *     創建一個新的隨機方塊（由多個 `square` 組成）
 *
 *   void show_block(void);
 *     顯示當前方塊在畫面上的位置
 *
 *   void clear_block(void);
 *     清除當前顯示的方塊
 *
 *   uint8_t che_collision(void);
 *     檢查方塊是否與其他方塊或邊界發生碰撞
 *
 *   uint8_t che_overlapping(uint8_t type);
 *     檢查當前方塊是否與地圖上的已存在方塊重疊
 *
 *   uint8_t che_full(void);
 *     檢查地圖是否有一整行已經填滿
 *
 *   void refresh_map(void);
 *     更新遊戲地圖（如消除填滿行）
 *
 *   void turn_left(uint16_t *b);
 *     將方塊逆時針旋轉 90 度
 *
 *   void turn_right(uint16_t *b);
 *     將方塊順時針旋轉 90 度
 *
 *   void move_right(void);
 *     讓方塊向右移動
 *
 *   void move_left(void);
 *     讓方塊向左移動
 *
 *   void draw_background(uint8_t start, uint8_t end);
 *     繪製遊戲背景
 *
 *   void delay_ms(uint16_t t);
 *     延遲函式，讓時間控制更加精確
 */
#ifndef INC_TETRIS_H_
#define INC_TETRIS_H_

#include "stm32f4xx_hal.h"
#include "LCD.h"
#include "Music.h"
#include <stdlib.h>

#define square_lenth     8     // square的邊長（單位：像素）
#define init_y           0     // 方塊的初始 Y 座標
#define init_x           56    // 方塊的初始 X 座標
#define top              20    // 地圖的行數（20行）
#define width            16    // 地圖的列數（16列）
#define background_color 0xffff// 背景顏色（白色）
#define line_color       0x0000// 線條顏色（黑色）
#define type_of_block    7     // 方塊的種類數（7種基本方塊）

extern uint16_t block[type_of_block], color[type_of_block], cur_color,cur_block, map[top], delay;
extern uint8_t square_xy1[2], tick, command, flag, cur_Music;

void TIM1_UP_TIM10_IRQHandler(void);
void tim1_init(void);
void start(void);
void end(void);
void draw_square(uint8_t x, uint8_t y, uint16_t rgb);
void create_block(void);
void show_block(void);
void clear_block(void);
uint8_t che_collision(void);
uint8_t che_overlapping(uint8_t type);
uint8_t che_full(void);
void refresh_map(void);
void turn_left(uint16_t *b);
void turn_right(uint16_t *b);
void move_right(void);
void move_left(void);
void draw_background(uint8_t start, uint8_t end);
void delay_ms(uint16_t t);

#endif /* INC_TETRIS_H_ */
