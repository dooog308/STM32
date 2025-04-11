
#include"Music.h"

uint8_t buff[512]={0};
uint16_t fight_ind=0, victory_ind=0;

void play_Music(uint8_t music)
{
	DMA1_Stream5->M0AR = buff;
	DMA1_Stream6->M0AR = buff;
	switch(music)
	{
		case 0:
		{
			while(!SD_read(buff, (fight1+fight_ind*buff_size), buff_size));
			fight_ind++;
			DMA1_Stream5->CR |= (0x1);
			DMA1_Stream6->CR |= (0x1);
			break;
		}
		case 1:
		{
			while(!SD_read(buff, (fight2+fight_ind*buff_size), buff_size));
			fight_ind++;
			DMA1_Stream5->CR |= (0x1);
			DMA1_Stream6->CR |= (0x1);
			if(fight_ind == fight2_size) fight_ind = 0;
			break;
		}
		case 2:
		{
			while(!SD_read(buff, (victory+victory_ind*buff_size), buff_size));
			victory_ind++;
			DMA1_Stream5->CR |= (0x1);
			DMA1_Stream6->CR |= (0x1);
			if(victory_ind == victory_size) victory_ind = 0;
			break;
		}
	}
}
