
#include "LCD.h"
#include "ASCII.h"

extern const unsigned char number[95][32];
uint8_t cursor[2]={0}, xs=0, xe=0, ys=0, ye=0;
void rst(uint8_t t)
{
	(t)? (GPIOA->ODR|=(1)) : (GPIOA->ODR&=~(1));
}
void dc(uint8_t t)
{
	(t)? (GPIOC->ODR|=(1)) : (GPIOC->ODR&=~(1));
}
void cs(uint8_t t)
{
	(t)? (GPIOA->ODR|=(1<<3)) : (GPIOA->ODR&=~(1<<3));
}
void spi_swapData(uint8_t data)
{
	cs(0);
	SPI1->DR = data;
	while(!(SPI1->SR&0X2));
	SPI1->SR &= ~(1<<1);
	cs(1);
}
void spi_16bit(uint16_t data)
{
	dc(1);
	spi_swapData(data>>8);
	spi_swapData(data);
}
void LCD_WriteCommand(uint8_t cmd)
{
	dc(0);
	spi_swapData(cmd);
}
void LCD_WriteByte(uint8_t data)
{
	dc(1);
	spi_swapData(data);
}
void LCD_init(void)
{
	  rst(1);
	  HAL_Delay(5);
	  rst(0);
	  HAL_Delay(5);
	  rst(1);
	  HAL_Delay(120);
	  LCD_WriteCommand(0x01);
	  HAL_Delay(120);
	  LCD_WriteCommand(0x11);
	  HAL_Delay(120);
	  LCD_WriteCommand(0X0C);
	  LCD_WriteByte(0x00);
	  LCD_WriteByte(0x05);
	  LCD_WriteCommand(0x29);
	  HAL_Delay(50);
}
void set_area(uint8_t dxs, uint8_t dxe, uint8_t dys, uint8_t dye)
{
	xs = dxs;
	xe = dxe;
	ys = dys;
	ye = dye;
	LCD_WriteCommand(0x2a);
	LCD_WriteByte(0X00);
	LCD_WriteByte(dxs);
	LCD_WriteByte(0X00);
	LCD_WriteByte(dxe);
	LCD_WriteCommand(0X2B);
	LCD_WriteByte(0X00);
	LCD_WriteByte(dys);
	LCD_WriteByte(0X00);
	LCD_WriteByte(dye);
	LCD_WriteCommand(0X2C);
}
void fill_area(uint16_t rgb)
{
	for(int i=ys;i<=ye;i++)
	{
		for(int j=xs;j<=xe;j++)
			spi_16bit(rgb);
	}
}
void LCD_printf(char *str, uint16_t rgb)
{
	uint8_t len=strlen(str);
	for(int i=0;i<len;i++)
	{
		set_area(cursor[0], cursor[0]+15, cursor[1], cursor[1]+15);
		for(int j=0;j<32;j++)
		{
			for(int k=0;k<8;k++)
			{
				(number[(uint8_t)str[i]-32][j]&(0x80>>k)) ? (spi_16bit(rgb)) : (spi_16bit(0xffff));
			}
		}
		cursor[0] += 16;
		if(cursor[0]>127)
		{
			cursor[0]=0;
			cursor[1] += 16;
		}
		if(cursor[0]>159)
		{
			cursor[0]=0;
			cursor[1]=0;
		}
	}
}
