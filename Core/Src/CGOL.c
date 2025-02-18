/*
 * Conway_s_game_of_life.c
 *
 * Created: 2020. 08. 10. 20:23:22
 * Author : Póti Szabolcs
 *
 * //Conway's game of life
 *
 */

#ifndef _CGOL_C
#define _CGOL_C

#include "main.h"

#ifdef STM32_bikecomputer_2
#include "ST7565_64x128_LCD.h"
#include "disp_fgv.h"
#include "Fonts_and_bitmaps_FLASH.h"
#include <string.h>

void Conway_s_game_of_life(void);
void setpix(uint8_t** mat, uint8_t x, uint8_t y, uint8_t Pixel_status);

extern volatile uint8_t btn;
extern uint8_t disp_mat[pixels_y][pixels_x/8];

constant uint8_t bmp_conway_infinite_growth[]={26,49,0x38,0x24,0x20,0x20,0x14,0x00,0x03,0x04,0x04,0x08,0x00,0x04,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
														0x40,0x82,0x83,0xFA,0x00,0x00,0x00,0xFA,0x83,0x82,0x40,0x00,0x00,0x00,0x00,0x00,0x00,
														0x03,0x04,0x00,0x08,0x04,0x04,0x03,0x00,0x14,0x20,0x20,0x24,0x38,0x38,0x48,0x08,0x08,
														0x50,0x00,0x80,0x40,0x40,0x20,0x00,0x40,0x80,0x20,0x70,0x58,0x0C,0x08,0x0C,0x2B,0x6A,
														0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0x6A,0x2B,0x0C,0x08,0x0C,0x58,0x70,0x20,0x80,0x40,
														0x00,0x20,0x40,0x40,0x80,0x00,0x50,0x08,0x08,0x48,0x38,0x00,0x00,0x00,0x00,0x00,0x00,
														0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA1,0xA6,0xB7,
														0x87,0xC0,0x87,0xB7,0xA6,0xA1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
														0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
														0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x80,0x00,0x00,
														0x00,0x80,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
														0x00,0x00,0x00,0x00,0x00,0x00,0x00
												};

void Conway_s_game_of_life(void)
{
	uint8_t cgy, cgx;
	uint8_t tt_[pixels_y][pixels_x/8]={0};
	uint8_t* tt[pixels_y];
	int8_t iy=0, ix=0, db=0;
	uint8_t jy=0, jx=0;
	while(jy<(pixels_y))
	{
		tt[jy]=tt_[jy];
		jy++;
	}
	uint8_t backup[pixels_y][pixels_x/8];
	delete_disp_mat();

	print_bmp_H(38, 42, bmp_conway_infinite_growth, Pixel_on, Overwrite);//make a starting pattern //todo kéne csinálni egy pattern könyvtárat

	print_disp_mat();
	while(1)
	{
		btn=0;
		cgy=64;//cursor at center
		cgx=32;//
		while(1)//draw seed
		{
			while(!btn)	{ asm("nop");}
			LL_mDelay(80);//dupla gombnyomáshoz
			if( (cgy < (pixels_y-1)) && (btn == entergomb))	{ cgy++;}
			if( (cgy > 0) && (btn == exitgomb))				{ cgy--;}
			if( (cgx < (pixels_x-1)) && (btn == jobbgomb))	{ cgx++;}
			if( (cgy > 0) && (btn == balgomb))				{ cgx--;}
			if( btn == (entergomb|exitgomb) )
			{
				if( disp_mat[cgy][cgx/8] & (1<<(cgx%8)) )	{ setpixel(cgx, cgy, Pixel_off);}	else{ setpixel(cgx, cgy, Pixel_on);}//pixel rajzolás
				print_disp_mat();
			}
			if( btn == (balgomb | jobbgomb) )	{ btn=0; break;}//start
			btn=0;
		}
		for(jy=0; jy<pixels_y; jy++)//copy backup for re edit
		{
			for(jx=0; jx<(pixels_x/8); jx++)
			{
				backup[jy][jx] = disp_mat[jy][jx];
			}
		}
		LL_mDelay(300);
		btn=0;
		while(1)
		{
			if(btn == jobbgomb)		{ LL_mDelay(1000); btn=0; while(btn!=jobbgomb)	{ asm("nop");} }//pause
			if(btn == entergomb)	{ btn=0; break;}//reset
			if(btn == exitgomb)		{ btn=0; LL_mDelay(300); return;}//exit
			if(btn == balgomb)		{ //clear pattern
										btn=0;
										delete_disp_mat();
										for(jy=0; jy<pixels_y; jy++)
										{
											for(jx=0; jx<(pixels_x/8); jx++)
											{
												tt[jy][jx] = 0;
												backup[jy][jx] = 0;
											}
										}
										break;
									}
			btn=0;
			LL_mDelay(50);
			for(cgy=0; cgy < pixels_y; cgy++)//generate mew state
			{
				for(cgx=0; cgx < pixels_x; cgx++)
				{
					for(db=0,iy=-1; iy<2; iy++)
					{
						for(ix=-1; ix<2; ix++)
						{
							if( (((cgy+iy) >= 0) && ((cgy+iy) < pixels_y)) && (((cgx+ix) >= 0) && ((cgx+ix) < pixels_x)) && (iy || ix) )//túlcímzés elkerülése, önmagát ne számolja bele
							{
								if( disp_mat[cgy+iy][(cgx+ix)/8] & (1<<((cgx+ix)%8)) )
								{
									db++;
								}
							}
						}
					}
					if( (db==3) || ((db==2) && (disp_mat[cgy][cgx/8] & (1<<(cgx%8)))) )	{ setpix(tt, cgx, cgy, Pixel_on);}//become alive, stay alive
					else
					{
						if((db<2) || (db>3))	{ setpix(tt, cgx, cgy, Pixel_off);}	else{}//die
					}
				}
			}
			delete_disp_mat();//delete previous state
			for(jy=0; jy<pixels_y; jy++)//copy new state and delete
			{
				for(jx=0; jx<(pixels_x/8); jx++)
				{
					disp_mat[jy][jx] = tt[jy][jx];
					tt[jy][jx] = 0;
				}
			}
			print_disp_mat();
		}
		for(jy=0; jy<pixels_y; jy++)//copy backup for editing
		{
			for(jx=0; jx<(pixels_x/8); jx++)
			{
				disp_mat[jy][jx] = backup[jy][jx];
			}
		}
		print_disp_mat();
		LL_mDelay(500);
	}
}


void setpix(uint8_t** mat, uint8_t x, uint8_t y, uint8_t Pixel_status)
{
	if((/*(x>=0)&&*/(x<pixels_x))&&(/*(y>=0)&&*/(y<pixels_y)))
	{
		uint8_t page=0,dotinpage=0;
		page=x/8;
		dotinpage=(0x01<<(x%8));
		if(Pixel_status == Pixel_on)
		{
			mat[y][page] |= dotinpage;
		}
		else
		{
			if(Pixel_status == Pixel_off)
			{
				mat[y][page] &= (~dotinpage);
			}	else{}
		}
	}	else{}
}

#endif//STM32_bikecomputer_2

#endif//_CGOL_C


