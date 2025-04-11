
#include "Tetris.h"
#include "main.h"

uint16_t block[type_of_block]={0X8888,0XCC00,0XE400,0XC600,0XE800,0X6C00,0XE200},
color[type_of_block]={0xf000, 0x000f, 0x00f0, 0xd00d, 0xfaa0, 0x0f00, 0xffc0},
cur_color=0, cur_block=0, map[top]={0},
delay=0;
uint8_t square_xy1[2]={init_x, init_y}, tick=0,
command=0, flag=0, cur_Music=0;
void TIM1_UP_TIM10_IRQHandler(void)
{
	TIM1->CR1 &= ~(1);
	tick++;
	if(delay) delay--;
	// 音樂播放處理（DMA 傳送完成）
	if((DMA1->HISR&(0X1<<11)))
	{
		DMA1->HIFCR |= (0X1<<11);
		DMA1_Stream5->NDTR = 512;
		DMA1_Stream6->NDTR = 512;
		play_Music(cur_Music);
	}
	// 處理玩家輸入
	if(command=='a'||command=='A')
	{
		if(che_overlapping(2)&&flag!=3)
		{
			clear_block();
			turn_left(&cur_block);
			show_block();
		}
		else if(flag == 3)
		{
			flag = 0;
			start();
		}
	}
	else if(command=='d'||command=='D')
	{
		if(che_overlapping(3)&&flag!=3)
		{
			clear_block();
			turn_right(&cur_block);
			show_block();
		}
	}
	else if((command=='j'||command=='J')&&flag!=3)move_left();
	else if((command=='l'||command=='L')&&flag!=3)move_right();
	 // 控制方塊落下
	if((tick == 200 || (command=='s'||command=='S'))&&flag!=3)
	{
		flag = che_collision();
		if(flag==1)
		{
			while(che_full()) refresh_map();
			create_block();
		}
		else if(flag==2) end();
		else if(flag==0)
		{
			clear_block();
			square_xy1[1] += square_lenth;
		}
		show_block();
		tick=0;
	}
	TIM1->CR1 |= (1);
	TIM1->SR &= ~(1);
	command=0;
}
void tim1_init(void)
{
	RCC -> APB2ENR |= (1);
	TIM1 -> CR1 |= (1<<7);
	TIM1 -> DIER |= (1);
	TIM1 -> PSC |= 71; // 設定預分頻器`,頻率是1M
	TIM1 -> ARR &= ~(0xffff);
	TIM1 -> ARR |= 1000;// 設定自動重裝值為4，計時器每4個時鐘週期觸發一次`,所以目前頻率是100K
	TIM1->CR1 |= (1<<3);
	TIM1 -> EGR |= (1);
	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}
void draw_square(uint8_t x, uint8_t y, uint16_t rgb)
{
	set_area(x, x+square_lenth-1, y, y+square_lenth-1);
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			if(j==0 || j==7 || i==0 || i==7) spi_16bit(line_color);
			else spi_16bit(rgb);
		}
	}
}
void create_block(void)
{
	uint8_t tem;
	tem = rand()%type_of_block ;
	srand(SysTick->VAL);
	cur_block = block[tem];
	cur_color = color[tem];
	square_xy1[0] = init_x;
	square_xy1[1] = init_y;
}
void show_block(void)
{
	uint8_t x=square_xy1[0], y=square_xy1[1];
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(cur_block&(0x8000>>(i*4+j)))
			{
				draw_square(x, y, cur_color);
			}
			x += square_lenth;
		}
		x = square_xy1[0];
		y += square_lenth;
	}
}
void clear_block(void)
{
	uint8_t x=square_xy1[0], y=square_xy1[1];
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if((cur_block&(0x8000>>(i*4+j)))&&(!(map[y/square_lenth]&(0x8000>>(x/square_lenth)))))
			{
				draw_square(x, y, background_color);
			}
			x += square_lenth;
		}
		x = square_xy1[0];
		y += square_lenth;
	}
}
/**
 * 檢查碰撞，回傳：
 * 0：無碰撞
 * 1：碰撞成功，固定方塊
 * 2：頂部撞擊，遊戲結束
 */
uint8_t che_collision(void)
{
	uint8_t p=3, x=square_xy1[0]/square_lenth, y=square_xy1[1]/square_lenth;
	if(cur_block==0x8888||cur_block==0x000f||cur_block==0x1111||cur_block==0xf000)
		p=4;
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			if((cur_block&(0x8000>>(i*4+j))) && (y==19||(map[y+1]&(0x8000>>(x)))))
			{
				uint8_t temy=square_xy1[1]/square_lenth, temx=square_xy1[0]/square_lenth,
						fix=0;
				for(int k=0;k<4;k++)
				{
					if(temx>15)
					{
						fix = 32-temx;
						temx = 0;
					}
					if((cur_block&(0xf000>>(k*4)))>0)
					{
						if(temx>(4*k))
							map[temy+k] |= ((cur_block&(0xf000>>(k*4)))>>(temx-(k*4)));
						else
							map[temy+k] |= ((cur_block&(0xf000>>(k*4)))<<((k*4)-temx+fix));
					}
				}
				if(y == 0) return 2;
				return 1;
			}
			x++;
			if(x>31) x=0;
		}
		x = square_xy1[0]/square_lenth;
		y++;
	}
	return 0;
}
uint8_t che_overlapping(uint8_t type)
{
	uint16_t tem=cur_block;
	uint8_t x=square_xy1[0]/square_lenth, y=square_xy1[1]/square_lenth, temx;
	switch(type)
	{
		case 0:
		{
			x--;
			break;
		}
		case 1:
		{
			x++;
			break;
		}
		case 2:
		{
			turn_left(&tem);
			break;
		}
		case 3:
		{
			turn_right(&tem);
			break;
		}
	}
	temx = x;
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<4;j++)
		{
			if((x>15)&&(tem&(0x8000>>(i*4+j)))) return 0;
			if((tem&(0x8000>>(i*4+j)))&&(map[y]&(0x8000>>(x))))return 0;
			x++;
			if(x>31) x = 0;
		}
		x = temx;
		y++;
	}
	return 1;
}
uint8_t che_full(void)
{
	for(int i=0;i<20;i++)
	{
		if(map[i]==0xffff) return 1;
	}
	return 0;
}
/**
 * 刷新地圖：將填滿的行清除，並下移方塊
 * 同時播放勝利音效
 */
void refresh_map(void)
{
	uint16_t t_map[20]={0}, t=0, empty=0, ind=0;
	for(int i=0;i<20;i++)
	{
		if(map[i]!=0x0000&&map[i]!=0xffff&&empty!=0) empty=i;
		if(map[i]==0xffff)
		{
			uint8_t temy=i*square_lenth;
			map[i] = 0x0000;
			for(int i=0;i<16;i++)draw_square(i*square_lenth, temy, background_color);
		}
		else if(map[i]!=0x0000)
		{
			t_map[t] = map[i];
			t++;
		}
		map[i] = 0x0000;
	}
	while(!(DMA1->HISR&(0X1<<11)));
	play_Music(2);
	/*
	 * 下面這段以後會加上閃爍的特效
	 */
	while(ind<victory_size)
	{
		if(DMA1->HISR&(0X1<<11))
		{
			DMA1->HIFCR |= (0X1<<11);
			DMA1_Stream5->NDTR = 512;
			play_Music(2);
			ind++;
		}
	}
	for(int i=top-t;i<20;i++)
	{
		map[i] = t_map[i+t-top];
	}
	for(int i=empty;i<20;i++)
	{
		for(int j=0;j<16;j++)
		{
			if(map[i]&(0x8000>>(j))) draw_square(j*square_lenth, i*square_lenth, cur_color);
			else draw_square(j*square_lenth, i*square_lenth, background_color);
		}
	}
}
void turn_left(uint16_t *b)
{
	uint16_t tem=0;
	uint8_t p=3;
	if(*b==0x8888||*b==0x000f||*b==0x1111||*b==0xf000)
		p=4;
	else if(*b==0xcc00) p=0;
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			if(*b&(0x8000>>(p-1-i+4*j))) tem |= (0x8000>>(i*4+j));
		}
	}
	if(p!=0) *b = tem;
	TIM1->CR1 |= (1);
}
void turn_right(uint16_t *b)
{
	uint16_t tem=0;
	uint8_t p=3;
	if(*b==0x8888||*b==0x000f||*b==0x1111||*b==0xf000)
		p=4;
	else if(*b==0xcc00) p=0;
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<p;j++)
		{
			if(*b&(0x8000>>(i+4*(p-j-1)))) tem |= (0x8000>>(i*4+j));
		}
	}
	if(p!=0)*b = tem;
	TIM1->CR1 |= (1);
}
void start(void)
{
	draw_background(0, top);
	create_block();
	tick = 0;
	square_xy1[0] = init_x;
	square_xy1[1] = init_y;
	for(int i=0;i<top;i++)
		map[i] = 0;
	play_Music(cur_Music);
	TIM1->CR1 |= (1);
}
void draw_background(uint8_t start, uint8_t end)
{
	for(int i=start;i<end;i++)
	{
		for(int j=0;j<16;j++)
		{
			draw_square(j*square_lenth, i*square_lenth, background_color);
		}
	}
}
void end(void)
{
	set_area(0, 127, 0, 159);
	fill_area(background_color);
	LCD_printf("a to re", line_color);
	flag = 3;
}
void move_right(void)
{
	if(che_overlapping(1))
	{
		clear_block();
		square_xy1[0] += square_lenth;
		show_block();
	}
	TIM1->CR1 |= (1);
}
void move_left(void)
{
	if(che_overlapping(0))
	{
		clear_block();
		square_xy1[0] -= square_lenth;
		show_block();
	}
	TIM1->CR1 |= (1);
}
void delay_ms(uint16_t t)
{
	delay = t;
}
