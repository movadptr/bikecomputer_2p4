/*
 * disp_fgv.c
 *
 *  Created on: 2020. aug. 14.
 *  Author: Póti Szabolcs
 */

#ifndef DISP_FGV_C_
#define DISP_FGV_C_

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include "disp_fgv.h"
#include "Fonts_and_bitmaps_FLASH.h"
#include "ST7565_64x128_LCD.h"

extern constant uint8_t _5x8chars[450];
extern constant uint8_t _10x16chars[1800];

uint8_t disp_mat[pixels_y][pixels_x/8]={0x00};
uint16_t firstcharbyte=0;
uint8_t charwidth=0;

#ifdef _AVR_IO_H_
#define constant	const PROGMEM
#ifdef pgm_read_byte_far
#define read_progmem	pgm_read_byte_far
#else
#define read_progmem	pgm_read_byte_near
#endif
#else//nem AVR
#define constant const
#endif

//____________________print in vertical orientation________________________________//


//TODO test this fn
//TODO make _H variant
uint8_t write_hex_byte_V(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{
	uint8_t i=0, numt[2]={0};
	//hexa számjegylefejtés
	numt[0]=((num/16)%16);
	numt[1]=((num/1)%16);
	if(align==ALIGN_RIGHT)//annyival arrébb kezdjük balra kiírni amennyi pixel széles a számsor elválasztó oszlopokkal együtt
	{
		while(i<2)
		{
			if(numt[i]>9)
			{
				character_info(numt[i]+('A'-10),size);
			}
			else
			{
				character_info(numt[i]+'0',size);
			}
			col-=charwidth;
			i++;
		}
		if(col>(pixels_x-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=0;
	while(i<2)
	{
		if(numt[i]>9)
		{
			col=write_character_V(col, row, numt[i]+('A'-10), Pixel_Status, size);
		}
		else
		{
			col=write_character_V(col, row, numt[i]+'0', Pixel_Status, size);
		}
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return col;
}

uint8_t write_dec_num_uint32_t_V(uint8_t col, uint8_t row, uint32_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{
	uint8_t i=0, numt[10]={0};
	numt[0]=((num/(10000*10000*10))%10);
	numt[1]=((num/(10000*10000))%10);
	numt[2]=((num/(10000*1000))%10);
	numt[3]=((num/(10000*100))%10);
	numt[4]=((num/(10000*10))%10);
	numt[5]=((num/10000)%10);
	numt[6]=((num/1000)%10);
	numt[7]=((num/100)%10);
	numt[8]=((num/10)%10);
	numt[9]=((num/1)%10);
	uint8_t first_not_zero=9;
	for(i=0; i<10; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	if(i==10)	{i--;}	else{}
	if(align==ALIGN_RIGHT)//annyival arrébb kezdjük balra kiírni amennyi pixel széles a számsor, elválasztó oszlopokkal együtt
	{
		col-=((9-i)-1);//ennyi space kell
		while(i<10)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col>(pixels_x-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=first_not_zero;
	while(i<10)
	{
		col=write_character_V(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}





#ifdef _AVR_IO_H_
void print_bmp_V(uint8_t col, uint8_t row, constant uint8_t *bmp, uint8_t Pixel_Status, uint8_t write_mode)
{	//bmp[0]=magasság pixelekben, bmp[1]=szélesség pixelekben
	uint16_t i=2, j=0, notwholelastpage=0;
	uint8_t start_col=col, start_row=row, pages=(read_progmem(bmp)/8), lastpageheight=(read_progmem(bmp)%8), c_page=1;
	if(lastpageheight!=0) { pages++; notwholelastpage=1;}	else{}
	for(i=2; ((i-2)<(read_progmem(bmp+1)*pages)) && (col<pixels_x+1); i++,col++)
	{
		if(((i-2)% read_progmem(bmp+1)==0) && ((i-2)!=0))	{col=start_col; start_row-=8; c_page++;}	else{}
		for(row=start_row,j=0; (j<8)&&(row<pixels_y); j++,row--)
		{
			if((c_page==pages)&&(j==lastpageheight)&&notwholelastpage)	{break;}	else{}
			if(Pixel_Status==Pixel_on)
			{
				if( (read_progmem(bmp+i) << j) & 0x80)	{ setpixel(col,row,Pixel_on);}
				else if(write_mode == Overwrite) { setpixel(col,row,Pixel_off);}	else{}
			}
			else if(Pixel_Status==Pixel_off)
			{
				if( (read_progmem(bmp+i) << j) & 0x80)	{ setpixel(col,row,Pixel_off);}
				else if(write_mode == Overwrite) { setpixel(col,row,Pixel_on);}	else{}
			}	else{}
		}
	}
}
#else
void print_bmp_V(uint8_t col, uint8_t row, constant uint8_t *bmp, uint8_t Pixel_Status, uint8_t write_mode)
{	//bmp[0]=magasság pixelekben, bmp[1]=szélesség pixelekben
	uint16_t i=2, j=0, notwholelastpage=0;
	uint8_t start_col=col, start_row=row, pages=(*(bmp)/8), lastpageheight=(*(bmp)%8), c_page=1;
	if(lastpageheight!=0) { pages++; notwholelastpage=1;}	else{}
	for(i=2; ((i-2)<(*(bmp+1)*pages)) && (col<pixels_x+1); i++,col++)
	{
		if(((i-2)% *(bmp+1)==0) && ((i-2)!=0))	{col=start_col; start_row-=8; c_page++;}	else{}
		for(row=start_row,j=0; (j<8)&&(row<pixels_y); j++,row--)
		{
			if((c_page==pages)&&(j==lastpageheight)&&notwholelastpage)	{break;}	else{}
			if(Pixel_Status==Pixel_on)
			{
				if( (*(bmp+i) << j) & 0x80)	{ setpixel(col,row,Pixel_on);}
				else if(write_mode == Overwrite)	{ setpixel(col,row,Pixel_off);}	else{}
			}
			else if(Pixel_Status==Pixel_off)
			{
				if( (*(bmp+i) << j) & 0x80)	{ setpixel(col,row,Pixel_off);}
				else if(write_mode == Overwrite) { setpixel(col,row,Pixel_on);} else{}
			}	else{}
		}
	}
}
#endif

uint8_t write_dec_num_float_V(uint8_t col, uint8_t row, float num, uint8_t digits_after_dot, uint8_t Pixel_Status, uint8_t size)
{
	uint8_t i=0, numt[9]={0}, start_col=col, neg=0;
	if(num != fabs(num))
	{
		num=fabs(num);
		neg=1;
	}	else{}
	numt[0]=(uint8_t)fmod(num/100/100, 10);
	numt[1]=(uint8_t)fmod(num/1000, 10);
	numt[2]=(uint8_t)fmod(num/100, 10);
	numt[3]=(uint8_t)fmod(num/10, 10);
	numt[4]=(uint8_t)fmod(num/1, 10);
	numt[5]=(uint8_t)fmod(num*10, 10);
	numt[6]=(uint8_t)fmod(num*100, 10);
	numt[7]=(uint8_t)fmod(num*1000, 10);
	numt[8]=(uint8_t)fmod(num*100*100, 10);
	uint8_t first_not_zero=4, last_not_zero=5;
	for(i=0; i<4; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	for(i=8; i>5; i--)
	{
		if(numt[i])	{last_not_zero=i; break;}	else{}
	}
	digits_after_dot+=4;//tömb indexet csinál az értékből
	if(digits_after_dot<last_not_zero)	{last_not_zero=digits_after_dot;}	else{}
	col=write_character_V(col, row, '.', Pixel_Status, size);
	col++;
	for(i=5; i < last_not_zero+1; )
	{
		col=write_character_V(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{return lcd_err;}	else{}//nem fér ki a karakter
		i++;
		if(i<last_not_zero+1)	{ col++;}	else{}
	}
	col=start_col;
	for(last_not_zero=first_not_zero; last_not_zero<=4; last_not_zero++)//last_not_zero-t használom futó változónak, már úgyse kell, minek hoznék létre másikat
	{
		character_info(numt[last_not_zero]+'0',size);
		col-=charwidth;
	}
	col-=((4-first_not_zero)+1);//ennyi space kell
	if(neg)
	{
		character_info('-',size);
		col-=(charwidth+1);
		col=write_character_V(col, row, '-', Pixel_Status, size);
		col++;
	}
	for(i=first_not_zero; i <=4; )
	{
		col=write_character_V(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}

uint8_t write_dec_num_time_format_V(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{//egy nullát rak a szám elé ha a szám kisebb mint 10
	uint8_t i=0, numt[2]={0};
	num=num&0b0111111;
	numt[0]=((num/10)%10);
	numt[1]=((num/1)%10);
	if(align==ALIGN_RIGHT)//annyival arrébb kezdjük balra kiírni amennyi pixel széles a számsor elválasztó oszlopokkal együtt
	{
		while(i<2)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col>(pixels_x-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=0;
	while(i<2)
	{
		col=write_character_V(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return col;
}

uint8_t write_dec_num_uint8_t_V(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{
	uint8_t i=0, numt[3]={0};
	numt[0]=((num/100)%10);
	numt[1]=((num/10)%10);
	numt[2]=((num/1)%10);
	uint8_t first_not_zero=2;
	for(i=0; i<3; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	if(i==3)	{i--;}	else{}
	if(align==ALIGN_RIGHT)//annyival arr�bb kezdj�k balra ki�rni amennyi pixel sz�les a sz�msor elv�laszt� oszlopokkal egy�tt
	{
		col-=((2-i)-1);//ennyivelvel kevesebb space kell
		while(i<3)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col>(pixels_x-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=first_not_zero;
	while(i<3)
	{
		col=write_character_V(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}

uint8_t write_dec_num_int16_t_V(uint8_t col, uint8_t row, int16_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{
	uint8_t i=0, numt[5]={0}, neg=0;
	if(num != abs(num))
	{
		num=abs(num);
		neg=1;
	}	else{}
	numt[0]=(uint8_t)((num/10000)%10);
	numt[1]=(uint8_t)((num/1000)%10);
	numt[2]=(uint8_t)((num/100)%10);
	numt[3]=(uint8_t)((num/10)%10);
	numt[4]=(uint8_t)((num/1)%10);
	uint8_t first_not_zero=4;
	for(i=0; i<5; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	if(i==5)	{i--;}	else{}
	if(align==ALIGN_RIGHT)//annyival arrébb kezdjük balra kiírni amennyi pixel széles a számsor, elválasztó oszlopokkal együtt
	{
		if(neg)
		{
			character_info('-',size);
			col-=charwidth;
		}else{ col++;}
		col-=(4-i);//ennyi space kell
		while(i<5)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col > (pixels_x-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=first_not_zero;
	if(neg)	{ col=write_character_V(col, row, '-', Pixel_Status, size); col++;}
	while(i<5)
	{
		col=write_character_V(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}

uint8_t write_dec_num_uint16_t_V(uint8_t col, uint8_t row, uint16_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{
	uint8_t i=0, numt[5]={0};
	numt[0]=((num/10000)%10);
	numt[1]=((num/1000)%10);
	numt[2]=((num/100)%10);
	numt[3]=((num/10)%10);
	numt[4]=((num/1)%10);
	uint8_t first_not_zero=4;
	for(i=0; i<5; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	if(i==5)	{i--;}	else{}
	if(align==ALIGN_RIGHT)//annyival arrébb kezdjük balra kiírni amennyi pixel széles a számsor, elválasztó oszlopokkal együtt
	{
		col-=((4-i)-1);//ennyi space kell
		while(i<5)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col>(pixels_x-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=first_not_zero;
	while(i<5)
	{
		col=write_character_V(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}

uint8_t write_text_V(uint8_t col, uint8_t row, char* text, uint8_t Pixel_Status, uint8_t size)
{
	while(*(text))
	{
		if(*(text) != ' ')
		{
			col=write_character_V(col, row, *(text), Pixel_Status, size);
		}
		else{ col+=2;}
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ text++; col++;}
	}
	return 0;
}

#ifdef _AVR_IO_H_
uint8_t write_character_V(uint8_t start_col, uint8_t start_row, char character, uint8_t Pixel_Status, uint8_t size)
{
	uint8_t row=start_row, col=start_col;
	uint8_t xi=0, yi=0;//futó változók
	character_info(character, size);//karakter tömb infó // [0] -line in block	//[1] -width
	if( ((start_col + charwidth) > pixels_x) )
	{
		return lcd_err;
	}	else{}
	if(size==size_5x8)
	{
		for(xi=0; xi<charwidth; xi++,col++)
		{
			for(yi=0,row=start_row; yi<8; yi++,row++)
			{
				if(Pixel_Status==Pixel_on)
				{
					if( ( read_progmem(_5x8chars+firstcharbyte+xi) >> yi) & 0b1)
					{
						setpixel(col,row,Pixel_on);
					}
					else
					{
						setpixel(col,row,Pixel_off);
					}
				}	else{}
				if(Pixel_Status==Pixel_off)
				{
					if( ( read_progmem(_5x8chars+firstcharbyte+xi) >> yi) & 0b1)
					{
						setpixel(col,row,Pixel_off);
					}
					else
					{
						setpixel(col,row,Pixel_on);
					}
				}	else{}
			}
		}
	}
	else if(size==size_10x16)
	{
		uint8_t chr_half=0;//fut� v�ltoz�; a 10x16 os karakter k�t b�jt magass�g�...
		start_row += 8;
		for(chr_half=0; chr_half<2; chr_half++,start_row-=8)
		{
			for(xi=0,col=start_col; xi<charwidth; xi++, col++)
			{
				for(yi=0,row=start_row; yi<8; yi++,row++)
				{
					if(Pixel_Status==Pixel_on)
					{
						if( ( read_progmem(_10x16chars+firstcharbyte+(chr_half*10)+xi) >> yi) & 0b1)
						{
							setpixel(col,row,Pixel_on);
						}
						else
						{
							setpixel(col,row,Pixel_off);
						}
					}	else{}
					if(Pixel_Status==Pixel_off)
					{
						if( ( read_progmem(_10x16chars+firstcharbyte+(chr_half*10)+xi) >> yi) & 0b1)
						{
							setpixel(col,row,Pixel_off);
						}
						else
						{
							setpixel(col,row,Pixel_on);
						}
					}	else{}
				}
			}
		}
	}	else{}
	return col;
}
#else
uint8_t write_character_V(uint8_t start_col, uint8_t start_row, char character, uint8_t Pixel_Status, uint8_t size)
{
	uint8_t row=start_row, col=start_col;
	uint8_t xi=0, yi=0;//futó változók
	character_info(character, size);
	if( ((start_col + charwidth) > pixels_x) )
	{
		return lcd_err;
	}	else{}
	if(size==size_5x8)
	{
		for(xi=0; xi<charwidth; xi++,col++)
		{
			for(yi=0,row=start_row; yi<8; yi++,row++)
			{
				if(Pixel_Status==Pixel_on)
				{
					if( ( *(_5x8chars+firstcharbyte+xi) >> yi) & 0b1)
					{
						setpixel(col,row,Pixel_on);
					}
					else
					{
						setpixel(col,row,Pixel_off);
					}
				}
				else if(Pixel_Status==Pixel_off)
				{
					if( ( *(_5x8chars+firstcharbyte+xi) >> yi) & 0b1)
					{
						setpixel(col,row,Pixel_off);
					}
					else
					{
						setpixel(col,row,Pixel_on);
					}
				}	else{}
			}
		}
	}
	else if(size==size_10x16)
	{
		uint8_t chr_half=0;//futó változó; a 10x16 os karakter két bájt magasságú...
		start_row += 8;
		for(chr_half=0; chr_half<2; chr_half++,start_row-=8)
		{
			for(xi=0,col=start_col; xi<charwidth; xi++,col++)
			{
				for(yi=0,row=start_row; yi<8; yi++,row++)
				{
					if(Pixel_Status==Pixel_on)
					{
						if( ( *(_10x16chars+firstcharbyte+(chr_half*10)+xi) >> yi) & 0b1)
						{
							setpixel(col,row,Pixel_on);
						}
						else
						{
							setpixel(col,row,Pixel_off);
						}
					}
					else if(Pixel_Status==Pixel_off)
					{
						if( ( *(_10x16chars+firstcharbyte+(chr_half*10)+xi) >> yi) & 0b1)
						{
							setpixel(col,row,Pixel_off);
						}
						else
						{
							setpixel(col,row,Pixel_on);
						}
					}	else{}
				}
			}
		}
	}	else{}
	return col;
}
#endif
//____________________print in vertical orientation________________________________//



//____________________print in horizontal orientation________________________________//

#ifdef _AVR_IO_H_
void print_bmp_H(uint8_t col, uint8_t row, constant uint8_t *bmp, uint8_t Pixel_Status, uint8_t write_mode)
{	//bmp[0]=magasság pixelekben, bmp[1]=szélesség pixelekben
	uint16_t i=2, j=0, notwholelastpage=0;
	uint8_t start_col=col, start_row=row, pages=(read_progmem(bmp)/8), lastpageheight=(read_progmem(bmp)%8), c_page=1;
	if(lastpageheight!=0) { pages++; notwholelastpage=1;}	else{}
	for(i=2; ((i-2)<(read_progmem(bmp+1)*pages)) && (col<pixels_y+1); i++,col++)
	{
		if(((i-2)% read_progmem(bmp+1)==0) && ((i-2)!=0))	{col=start_col; start_row-=8; c_page++;}	else{}
		for(row=start_row,j=0; (j<8)&&(row<pixels_x); j++,row--)
		{
			if((c_page==pages)&&(j==lastpageheight)&&notwholelastpage)	{break;}	else{}
			if(Pixel_Status==Pixel_on)
			{
				if( (read_progmem(bmp+i) << j) & 0x80)	{ setpixel(pixels_x-1-row,col,Pixel_on);}
				else if(write_mode == Overwrite) { setpixel(pixels_x-1-row,col,Pixel_off);}	else{}
			}
			else if(Pixel_Status==Pixel_off)
			{
				if( (read_progmem(bmp+i) << j) & 0x80)	{ setpixel(pixels_x-1-row,col,Pixel_off);}
				else if(write_mode == Overwrite) { setpixel(pixels_x-1-row,col,Pixel_on);} else{}
			}	else{}
		}
	}
}
#else
void print_bmp_H(uint8_t col, uint8_t row, constant uint8_t *bmp, uint8_t Pixel_Status, uint8_t write_mode)
{	//bmp[0]=magasság pixelekben, bmp[1]=szélesség pixelekben
	uint16_t i=2, j=0, notwholelastpage=0;
	uint8_t start_col=col, start_row=row, pages=(*(bmp)/8), lastpageheight=(*(bmp)%8), c_page=1;
	if(lastpageheight!=0) { pages++; notwholelastpage=1;}	else{}
	for(i=2; ((i-2)<(*(bmp+1)*pages)) && (col<pixels_y+1); i++,col++)
	{
		if(((i-2)% *(bmp+1)==0) && ((i-2)!=0))	{col=start_col; start_row-=8; c_page++;}	else{}
		for(row=start_row,j=0; (j<8)&&(row<pixels_x); j++,row--)
		{
			if((c_page==pages)&&(j==lastpageheight)&&notwholelastpage)	{break;}	else{}
			if(Pixel_Status==Pixel_on)
			{
				if( (*(bmp+i) << j) & 0x80)	{ setpixel(pixels_x-1-row,col,Pixel_on);}
				else if(write_mode == Overwrite) { setpixel(pixels_x-1-row,col,Pixel_off);}	else{}
			}
			else if(Pixel_Status==Pixel_off)
			{
				if( (*(bmp+i) << j) & 0x80)	{ setpixel(pixels_x-1-row,col,Pixel_off);}
				else if(write_mode == Overwrite) { setpixel(pixels_x-1-row,col,Pixel_on);}	else{}
			}	else{}
		}
	}
}
#endif

uint8_t write_dec_num_float_H(uint8_t col, uint8_t row, float num, uint8_t digits_after_dot, uint8_t Pixel_Status, uint8_t size)
{
	uint8_t i=0, numt[9]={0}, start_col=col, neg=0;
	if(num != fabs(num))
	{
		num=fabs(num);
		neg=1;
	}	else{}
	numt[0]=fmod(num/100/100, 10);
	numt[1]=fmod(num/1000, 10);
	numt[2]=fmod(num/100, 10);
	numt[3]=fmod(num/10, 10);
	numt[4]=fmod(num/1, 10);
	numt[5]=fmod((num*10)/1, 10);
	numt[6]=fmod((num*100)/1, 10);
	numt[7]=fmod((num*1000)/1, 10);
	numt[8]=fmod((num*100*100)/1, 10);
	uint8_t first_not_zero=4, last_not_zero=5;
	for(i=0; i<4; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	for(i=8; i>5; i--)
	{
		if(numt[i])	{last_not_zero=i; break;}	else{}
	}
	digits_after_dot+=4;
	if(digits_after_dot<last_not_zero)	{last_not_zero=digits_after_dot;}	else{}
	col=write_character_H(col, row, '.', Pixel_Status, size);
	col++;
	for(i=5; i < last_not_zero+1; )
	{
		col=write_character_H(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{return lcd_err;}	else{}//nem fér ki a karakter
		i++;
		if(i<last_not_zero+1)
		{
			col++;
		}else{}
	}
	col=start_col;
	for(last_not_zero=first_not_zero; last_not_zero<=4; last_not_zero++)//last_not_zero-t használom futó változónak, már úgyse kell, minek hoznék létre másikat
	{
		character_info(numt[last_not_zero]+'0',size);
		col-=charwidth;
	}
	col-=((4-first_not_zero)+1);//ennyi space kell
	if(neg)
	{
		character_info('-',size);
		col-=(charwidth+1);
		col=write_character_H(col, row, '-', Pixel_Status, size);
		col++;
	}
	for(i=first_not_zero; i <=4; )
	{
		col=write_character_H(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}

uint8_t write_dec_num_time_format_H(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{//egy nullát rak a szám elé ha a szám kisebb mint 10
	uint8_t i=0, numt[2]={0};
	numt[0]=((num/10)%10);
	numt[1]=((num/1)%10);
	if(align==ALIGN_RIGHT)//annyival arr�bb kezdj�k balra ki�rni amennyi pixel sz�les a sz�msor elv�laszt� oszlopokkal egy�tt
	{
		while(i<2)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col>(pixels_y-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=0;
	while(i<2)
	{
		col=write_character_H(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return col;
}

uint8_t write_dec_num_uint8_t_H(uint8_t col, uint8_t row, uint8_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{
	uint8_t i=0, numt[3]={0};
	numt[0]=((num/100)%10);
	numt[1]=((num/10)%10);
	numt[2]=((num/1)%10);
	uint8_t first_not_zero=2;
	for(i=0; i<3; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	if(i==3)	{i--;}	else{}
	if(align==ALIGN_RIGHT)//annyival arr�bb kezdj�k balra ki�rni amennyi pixel sz�les a sz�msor elv�laszt� oszlopokkal egy�tt
	{
		col-=((2-i)-1);//ennyivelvel kevesebb space kell
		while(i<3)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col>(pixels_y-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=first_not_zero;
	while(i<3)
	{
		col=write_character_H(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}

uint8_t write_dec_num_int16_t_H(uint8_t col, uint8_t row, int16_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{
	uint8_t i=0, numt[5]={0}, neg=0;
	if(num != abs(num))
	{
		num=abs(num);
		neg=1;
	}	else{}
	numt[0]=((num/10000)%10);
	numt[1]=((num/1000)%10);
	numt[2]=((num/100)%10);
	numt[3]=((num/10)%10);
	numt[4]=((num/1)%10);
	uint8_t first_not_zero=4;
	for(i=0; i<5; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	if(i==5)	{i--;}	else{}
	if(align==ALIGN_RIGHT)//annyival arrébb kezdjük balra kiírni amennyi pixel széles a számsor, elválasztó oszlopokkal együtt
	{
		if(neg)
		{
			character_info('-',size);
			col-=charwidth;
		}else{ col++;}
		col-=(4-i);//ennyi space kell
		while(i<5)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col > (pixels_y-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=first_not_zero;
	if(neg)	{ col=write_character_H(col, row, '-', Pixel_Status, size); col++;}
	while(i<5)
	{
		col=write_character_H(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}

uint8_t write_dec_num_uint16_t_H(uint8_t col, uint8_t row, uint16_t num, uint8_t Pixel_Status, uint8_t size, uint8_t align)
{
	uint8_t i=0, numt[5]={0};
	numt[0]=((num/10000)%10);
	numt[1]=((num/1000)%10);
	numt[2]=((num/100)%10);
	numt[3]=((num/10)%10);
	numt[4]=((num/1)%10);
	uint8_t first_not_zero=4;
	for(i=0; i<5; i++)
	{
		if(numt[i])	{first_not_zero=i; break;}	else{}
	}
	if(i==5)	{i--;}	else{}
	if(align==ALIGN_RIGHT)//annyival arrébb kezdjük balra kiírni amennyi pixel széles a számsor, elválasztó oszlopokkal együtt
	{
		col-=((4-i)-1);//ennyi space kell
		while(i<5)
		{
			character_info(numt[i]+'0',size);
			col-=charwidth;
			i++;
		}
		if(col>(pixels_y-1))	{return lcd_err;}	else{}//alulcsordult
	}	else{}
	i=first_not_zero;
	while(i<5)
	{
		col=write_character_H(col, row, numt[i]+'0', Pixel_Status, size);
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ i++; col++;}
	}
	return 0;
}

uint8_t write_text_H(uint8_t col, uint8_t row, char* text, uint8_t Pixel_Status, uint8_t size)
{
	while(*(text))
	{
		if(*(text) != ' ')
		{
			col=write_character_H(col, row, *(text), Pixel_Status, size);
		}
		else{ col+=2;}
		if(col==lcd_err)	{ return lcd_err;}//nem fér ki a karakter
		else{ text++; col++;}
	}
	return 0;
}

#ifdef _AVR_IO_H_
uint8_t write_character_H(uint8_t start_col, uint8_t start_row, char character, uint8_t Pixel_Status, uint8_t size)
{
	uint8_t row=start_row, col=start_col;
	uint8_t xi=0, yi=0;//futó változók
	character_info(character, size);//karakter tömb infó // [0] -line in block	//[1] -width
	if( ((start_col + charwidth) > pixels_y) )
	{
		return lcd_err;
	}	else{}
	if(size==size_5x8)
	{
		for(xi=0; xi<charwidth; xi++,col++)
		{
			for(yi=0,row=start_row; yi<8; yi++,row++)
			{
				if(Pixel_Status==Pixel_on)
				{
					if( ( read_progmem(_5x8chars+firstcharbyte+xi) >> yi) & 0b1)
					{
						setpixel(pixels_x-1-row,col,Pixel_on);
					}
					else
					{
						setpixel(pixels_x-1-row,col,Pixel_off);
					}
				}
				else if(Pixel_Status==Pixel_off)
				{
					if( ( read_progmem(_5x8chars+firstcharbyte+xi) >> yi) & 0b1)
					{
						setpixel(pixels_x-1-row,col,Pixel_off);
					}
					else
					{
						setpixel(pixels_x-1-row,col,Pixel_on);
					}
				}	else{}
			}
		}
	}
	else if(size==size_10x16)
	{
		uint8_t chr_half=0;//fut� v�ltoz�; a 10x16 os karakter k�t b�jt magass�g�...
		start_row += 8;
		for(chr_half=0; chr_half<2; chr_half++,start_row-=8)
		{
			for(xi=0,col=start_col; xi<charwidth; xi++,col++)
			{
				for(yi=0,row=start_row; yi<8; yi++,row++)
				{
					if(Pixel_Status==Pixel_on)
					{
						if( ( read_progmem(_10x16chars+firstcharbyte+(chr_half*10)+xi) >> yi) & 0b1)
						{
							setpixel(pixels_x-1-row,col,Pixel_on);
						}
						else
						{
							setpixel(pixels_x-1-row,col,Pixel_off);
						}
					}
					else if(Pixel_Status==Pixel_off)
					{
						if( ( read_progmem(_10x16chars+firstcharbyte+(chr_half*10)+xi) >> yi) & 0b1)
						{
							setpixel(pixels_x-1-row,col,Pixel_off);
						}
						else
						{
							setpixel(pixels_x-1-row,col,Pixel_on);
						}
					}	else{}
				}
			}
		}
	}	else{}
	return col;
}
#else
uint8_t write_character_H(uint8_t start_col, uint8_t start_row, char character, uint8_t Pixel_Status, uint8_t size)
{
	uint8_t row=start_row, col=start_col;
	uint8_t xi=0, yi=0;//futó változók
	character_info(character, size);
	if( ((start_col + charwidth) > pixels_y) )
	{
		return lcd_err;
	}	else{}
	if(size==size_5x8)
	{
		for(xi=0; xi<charwidth; xi++,col++)
		{
			for(yi=0,row=start_row; yi<8; yi++,row++)
			{
				if(Pixel_Status==Pixel_on)
				{
					if( ( *(_5x8chars+firstcharbyte+xi) >> yi) & 0b1)
					{
						setpixel(pixels_x-1-row,col,Pixel_on);
					}
					else
					{
						setpixel(pixels_x-1-row,col,Pixel_off);
					}
				}
				else if(Pixel_Status==Pixel_off)
				{
					if( ( *(_5x8chars+firstcharbyte+xi) >> yi) & 0b1)
					{
						setpixel(pixels_x-1-row,col,Pixel_off);
					}
					else
					{
						setpixel(pixels_x-1-row,col,Pixel_on);
					}
				}	else{}
			}
		}
	}
	else if(size==size_10x16)
	{
		uint8_t chr_half=0;//futó változó; a 10x16 os karakter két bájt magasságú...
		start_row += 8;
		for(chr_half=0; chr_half<2; chr_half++,start_row-=8)
		{
			for(xi=0,col=start_col; xi<charwidth; xi++,col++)
			{
				for(yi=0,row=start_row; yi<8; yi++,row++)
				{
					if(Pixel_Status==Pixel_on)
					{
						if( ( *(_10x16chars+firstcharbyte+(chr_half*10)+xi) >> yi) & 0b1)
						{
							setpixel(pixels_x-1-row,col,Pixel_on);
						}
						else
						{
							setpixel(pixels_x-1-row,col,Pixel_off);
						}
					}
					else if(Pixel_Status==Pixel_off)
					{
						if( ( *(_10x16chars+firstcharbyte+(chr_half*10)+xi) >> yi) & 0b1)
						{
							setpixel(pixels_x-1-row,col,Pixel_off);
						}
						else
						{
							setpixel(pixels_x-1-row,col,Pixel_on);
						}
					}	else{}
				}
			}
		}
	}	else{}
	return col;
}
#endif
//____________________print in horizontal orientation________________________________//


#ifdef _AVR_IO_H_
void character_info( char characterf, uint8_t size)
{	// 0-first byte of char in block	//1-width
	if( (characterf >= '!') && (characterf <= 'z') )
	{
		uint16_t ind=0;
		switch(size)
		{
			case size_5x8:	firstcharbyte = ( (characterf - '!')*5 );
							while( (read_progmem(_5x8chars+firstcharbyte+ind) != 0xCC) && (ind<5) )	{ ind++;}
							charwidth = ind;
							break;

			case size_10x16:	firstcharbyte = ( (characterf - '!')*20 );
								while( (read_progmem(_10x16chars+firstcharbyte+ind) != 0xCC) && (ind<10) )	{ ind++;}
								charwidth = ind;
								break;
		}
	}else{}
}
#else
void character_info( char characterf, uint8_t size)
{
	if( (characterf >= '!') && (characterf <= 'z') )
	{
		uint16_t ind=0;
		switch(size)
		{
			case size_5x8:	firstcharbyte = ( (characterf - '!')*5 );
							while( (*(_5x8chars+firstcharbyte+ind) != 0xCC) && (ind<5) )	{ ind++;}
							charwidth = ind;
							break;

			case size_10x16:	firstcharbyte = ( (characterf - '!')*20 );
								while( (*(_10x16chars+firstcharbyte+ind) != 0xCC) && (ind<10) )	{ ind++;}
								charwidth = ind;
								break;
		}
	}else{}
}
#endif

void fill_rectangle_x1y1_x2y2(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t pixel_status)
{
	for(; y1<=y2; y1++)
	{
		draw_line_x(x1,x2,y1,pixel_status);
	}
}

void fill_rectangle_xy_height_width(uint8_t x, uint8_t y, uint8_t height, uint8_t width, uint8_t pixel_status)
{
	uint8_t y0=y;
	for(; y<y0+height; y++)
	{
		draw_line_x(x,x+width-1,y,pixel_status);
	}
}

void draw_rectangle_x1y1_x2y2(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t pixel_status)
{
	draw_line_x(x1+1,x2-1,y1,pixel_status);
	draw_line_x(x1+1,x2-1,y2,pixel_status);
	draw_line_y(y1,y2,x1,pixel_status);
	draw_line_y(y1,y2,x2,pixel_status);
}

void draw_rectangle_xy_height_width(uint8_t x, uint8_t y, uint8_t height, uint8_t width, uint8_t pixel_status)
{
	draw_line_x(x, x+width-1,  y,			pixel_status);
	draw_line_x(x, x+width-1,  y+height-1,  pixel_status);
	draw_line_y(y, y+height-1, x,			pixel_status);
	draw_line_y(y, y+height-1, x+width-1,   pixel_status);
}

void draw_line_y(uint8_t y1, uint8_t y2,uint8_t x, uint8_t pixel_status)
{
	for(; y1<=y2; y1++)
	{
		setpixel(x,y1,pixel_status);
	}
}

void draw_line_x(uint8_t x1, uint8_t x2,uint8_t y, uint8_t pixel_status)
{
	for(; x1<=x2; x1++)
	{
		setpixel(x1,y,pixel_status);
	}
}

void setpixel(uint8_t x, uint8_t y, uint8_t Pixel_status)
{
	if((/*(x>=0)&&*/(x<pixels_x))&&(/*(y>=0)&&*/(y<pixels_y)))
	{
		uint8_t page=0,dotinpage=0;
		page=x/8;
		dotinpage=(0x01<<(x%8));
		if(Pixel_status == Pixel_on)
		{
			disp_mat[y][page] |= dotinpage;
		}
		else
		{
			if(Pixel_status == Pixel_off)
			{
				disp_mat[y][page] &= (~dotinpage);
			}	else{}
		}
	}	else{}
}

void delete_disp_mat(void)
{
	uint8_t j=0,k=0;
	for(j=0; j<(pixels_x/8); j++)
	{
		for(k=0; k<pixels_y; k++)
		{
			disp_mat[k][j]=0;
		}
	}
}

#endif //DISP_FGV_C_
