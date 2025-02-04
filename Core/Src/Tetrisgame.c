/*
 * Tetrisgame.c
 *
 * Created: 2019. 08. 19. 21:11:34
 * Author: Póti Szabolcs
 */ 

#ifndef _Tetrisgame_c
#define _Tetrisgame_c

#include "main.h"
#ifdef STM32_bikecomputer_2

#include "ST7565_64x128_LCD.h"
#include "disp_fgv.h"
#include "Fonts_and_bitmaps_FLASH.h"
#include "Alt_fuggvenyek.h"

#define LCD_128x64
#define _TETR_blocks_x	11
#define _TETR_blocks_y	31

int tetrisgame(void);
void TETR_print(unsigned char **envmx2f);
void add(unsigned char **envmx2f,unsigned char *item_posf);
unsigned char check_spawn(unsigned char **envmx2f, unsigned char *item_posf);
unsigned char *movdown(unsigned char **envmx2f, unsigned char *item_posf);
unsigned char *movleft(unsigned char **envmx2f, unsigned char *item_posf);
unsigned char *movright(unsigned char **envmx2f, unsigned char *item_posf);
unsigned char *putdown(unsigned char **envmx2f, unsigned char *item_posf);
void item_pos_reset(unsigned char *item_posf, unsigned char *itemstartpos);
void item_1_2(unsigned char **envmx2f, unsigned char *item_posf);
void rotateMatrix(unsigned char mat[][4]);/////
unsigned char *rotate(unsigned char **envmx2f, unsigned char *item_posf);
void Tgame_over(void);
void draw_next_item(void);

void Tgame_main_isr(void);
void Tgame_button_isr(void);

uint32_t rnd=0;
volatile unsigned char item=0, next_item=0;//, num1=0;
volatile unsigned char moving=0;
volatile uint16_t score=0;
volatile uint8_t Tgame_status=0;//0-nop, 1-reset, 2-in game

unsigned char *envmx2[_TETR_blocks_x]={0}; // **
unsigned char *posaddrs[7]={0};
unsigned char *startposaddrs[7]={0};

extern volatile uint8_t btn;

#ifndef constant
#ifdef _AVR_IO_H_
#include <avr/pgmspace.h>
#define constant	const PROGMEM
#else
#define constant	const
#endif
#endif
//tetris game start logo bitmap
constant uint8_t bmp_tetr[434]={67,48,0xFF,0xC0,0xC0,0xC0,0xF8,0x18,0x18,0x18,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xFF,0xC0,0xC0,
									0xC0,0xF8,0x18,0x18,0x18,0x1F,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1F,0x18,0x18,0x18,0x18,0x18,0x1F,
									0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1F,0xFF,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0xFF,0x00,0x00,0x00,
									0x00,0x07,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x1F,0x10,0x10,0x10,0xFF,0x00,0x00,0x00,0x3E,0x00,0x00,
									0x00,0xFF,0x40,0x40,0x40,0x40,0x40,0xFF,0x00,0x00,0x00,0x1E,0x02,0x02,0x02,0xFF,0xFF,0x00,0x00,0x00,
									0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,
									0x00,0xFF,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0x02,0x02,0x02,0x03,
									0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x80,0x80,0x80,
									0x80,0xFF,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0xFF,0x00,0x00,
									0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0xFF,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,
									0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC7,0xC4,0xC4,0xC4,
									0xC4,0xC7,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,
									0x1F,0x10,0x10,0x10,0x10,0x1F,0x1F,0x10,0x10,0x10,0x10,0x1F,0x1F,0x10,0x10,0x10,0x10,0x1F,0x00,0x00,
									0x00,0x00,0x00,0x00,0xFF,0x30,0x30,0x30,0x30,0xFF,0x1F,0x10,0x10,0x10,0x10,0x1F,0x1F,0x10,0x10,0x10,
									0x10,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0xFE,0xC2,0xC2,0xC2,0xC2,0xFE,
									0x80,0x80,0x80,0x80,0x80,0x80,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0xC3,0xC3,0xC3,0xC3,0xFF,0xFE,0xC2,
									0xC2,0xC2,0xC2,0xFE,0x81,0x81,0x81,0x81,0x81,0x81,0x7F,0x43,0x43,0x43,0x43,0x7F,0x00,0x00,0x00,0x00,
									0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x04,0x04,0x04,0x04,0x07,0xFF,0x0C,0x0C,0x0C,0x0C,0xFF,
									0xFF,0x0C,0x0C,0x0C,0x0C,0xFF,0x07,0x04,0x04,0x04,0x04,0x07,0xFF,0x0C,0x0C,0x0C,0x0C,0xFF,0xFF,0x0C,
									0x0C,0x0C,0x0C,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x20,0x20,0x20,
									0x20,0xE0,0xE0,0x20,0x20,0x20,0x20,0xE0,0xE0,0x20,0x20,0x20,0x20,0xE0,0xE0,0x20,0x20,0x20,0x20,0xE0,
									0xE0,0x20,0x20,0x20,0x20,0xE0,0xE0,0x20,0x20,0x20,0x20,0xE0};

int tetrisgame(void)//////////////////////
{
	delete_disp_mat();
	print_bmp_V(8, 97, bmp_tetr, Pixel_on, Merge);
	print_disp_mat();
	LL_mDelay(1000);

	unsigned char LLpos[9]	=	{(_TETR_blocks_x/2)-1, _TETR_blocks_y-3, (_TETR_blocks_x/2), _TETR_blocks_y-3, (_TETR_blocks_x/2), _TETR_blocks_y-2, (_TETR_blocks_x/2), _TETR_blocks_y-1, 1};
	unsigned char LLstartpos[9]={(_TETR_blocks_x/2)-1, _TETR_blocks_y-3, (_TETR_blocks_x/2), _TETR_blocks_y-3, (_TETR_blocks_x/2), _TETR_blocks_y-2, (_TETR_blocks_x/2), _TETR_blocks_y-1, 1};

	unsigned char LRpos[9]	=	{(_TETR_blocks_x/2)-1, _TETR_blocks_y-3, (_TETR_blocks_x/2), _TETR_blocks_y-3, (_TETR_blocks_x/2)-1, _TETR_blocks_y-2, (_TETR_blocks_x/2)-1, _TETR_blocks_y-1, 1};
	unsigned char LRstartpos[9]={(_TETR_blocks_x/2)-1, _TETR_blocks_y-3, (_TETR_blocks_x/2), _TETR_blocks_y-3, (_TETR_blocks_x/2)-1, _TETR_blocks_y-2, (_TETR_blocks_x/2)-1, _TETR_blocks_y-1, 1};

	unsigned char ZRpos[9]	=	{(_TETR_blocks_x/2)-1, _TETR_blocks_y-2, (_TETR_blocks_x/2), _TETR_blocks_y-2, (_TETR_blocks_x/2), _TETR_blocks_y-1, (_TETR_blocks_x/2)+1, _TETR_blocks_y-1, 1};
	unsigned char ZRstartpos[9]={(_TETR_blocks_x/2)-1, _TETR_blocks_y-2, (_TETR_blocks_x/2), _TETR_blocks_y-2, (_TETR_blocks_x/2), _TETR_blocks_y-1, (_TETR_blocks_x/2)+1, _TETR_blocks_y-1, 1};

	unsigned char ZLpos[9]	=	{_TETR_blocks_x/2, _TETR_blocks_y-2, (_TETR_blocks_x/2)+1, _TETR_blocks_y-2, (_TETR_blocks_x/2)-1, _TETR_blocks_y-1, _TETR_blocks_x/2, _TETR_blocks_y-1, 1};
	unsigned char ZLstartpos[9]={_TETR_blocks_x/2, _TETR_blocks_y-2, (_TETR_blocks_x/2)+1, _TETR_blocks_y-2, (_TETR_blocks_x/2)-1, _TETR_blocks_y-1, _TETR_blocks_x/2, _TETR_blocks_y-1, 1};

	unsigned char SQRpos[9]	=	 {(_TETR_blocks_x/2)-1, _TETR_blocks_y-2, _TETR_blocks_x/2, _TETR_blocks_y-2, (_TETR_blocks_x/2)-1, _TETR_blocks_y-1, _TETR_blocks_x/2, _TETR_blocks_y-1, 1};
	unsigned char SQRstartpos[9]={(_TETR_blocks_x/2)-1, _TETR_blocks_y-2, _TETR_blocks_x/2, _TETR_blocks_y-2, (_TETR_blocks_x/2)-1, _TETR_blocks_y-1, _TETR_blocks_x/2, _TETR_blocks_y-1, 1};

	unsigned char Tpos[9]	=  {_TETR_blocks_x/2, _TETR_blocks_y-2, (_TETR_blocks_x/2)-1, _TETR_blocks_y-1, _TETR_blocks_x/2, _TETR_blocks_y-1, (_TETR_blocks_x/2)+1, _TETR_blocks_y-1, 1};
	unsigned char Tstartpos[9]={_TETR_blocks_x/2, _TETR_blocks_y-2, (_TETR_blocks_x/2)-1, _TETR_blocks_y-1, _TETR_blocks_x/2, _TETR_blocks_y-1, (_TETR_blocks_x/2)+1, _TETR_blocks_y-1, 1};

	unsigned char Ipos[9]	=  {(_TETR_blocks_x/2)-1, _TETR_blocks_y-1, _TETR_blocks_x/2, _TETR_blocks_y-1, (_TETR_blocks_x/2)+1, _TETR_blocks_y-1, (_TETR_blocks_x/2)+2, _TETR_blocks_y-1, 1};
	unsigned char Istartpos[9]={(_TETR_blocks_x/2)-1, _TETR_blocks_y-1, _TETR_blocks_x/2, _TETR_blocks_y-1, (_TETR_blocks_x/2)+1, _TETR_blocks_y-1, (_TETR_blocks_x/2)+2, _TETR_blocks_y-1, 1};

	rnd=get_random_num();
	next_item=(rnd%7);

	moving=0;
	score=0;
	unsigned char envmx[_TETR_blocks_x][_TETR_blocks_y]={0};
	for(uint8_t j=0; j<_TETR_blocks_x; j++)	// **
	{
		envmx2[j]=envmx[j];
	}
	posaddrs[0] = LLpos;
	posaddrs[1] = LRpos;
	posaddrs[2] = ZLpos;
	posaddrs[3] = ZRpos;
	posaddrs[4] = SQRpos;
	posaddrs[5] = Tpos;
	posaddrs[6] = Ipos;
	startposaddrs[0] = LLstartpos;
	startposaddrs[1] = LRstartpos;
	startposaddrs[2] = ZLstartpos;
	startposaddrs[3] = ZRstartpos;
	startposaddrs[4] = SQRstartpos;
	startposaddrs[5] = Tstartpos;
	startposaddrs[6] = Istartpos;
	delete_disp_mat();
	draw_rectangle_x1y1_x2y2(0, 0, 47, 127, Pixel_on);//keret
	write_dec_num_int16_t_V(63, 121, score, Pixel_on, size_5x8, ALIGN_RIGHT);
	print_disp_mat();
	Tgame_status=2;
	while(1)
	{
		if(Tgame_status==1)	{LL_mDelay(4000); Tgame_status=0; break;}	else{}
		asm("nop");
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////függvények//////////////////////////////////////////////

void Tgame_button_isr(void)//a meghívást timer vagy külső megszakításba rakni//ahonnan a gomb értéke jön
{
	if(moving==1)
	{
		switch(btn)
		{
			case exitgomb:	posaddrs[item]=putdown(envmx2, posaddrs[item]);
							moving=0;
							item_pos_reset(posaddrs[item], startposaddrs[item]);
							break;
			case jobbgomb:	posaddrs[item]=movright(envmx2, posaddrs[item]);
							break;
			case entergomb: if(item!=4)//SQR-t nem forgatjuk
							{
								posaddrs[item]=rotate(envmx2, posaddrs[item]);
							}
							break;
			case balgomb: 	posaddrs[item]=movleft(envmx2, posaddrs[item]);
							break;
		}
	}else{}
 	TETR_print(envmx2);
 	btn=0;
}

void Tgame_main_isr(void)//a meghívást timer megszakításba rakni
{
	if(moving==0)
	{
		/*
		num1=item;
		item=next_item;
		while(1)
		{
			rnd=LFSR_random(rnd);
			if( ((rnd%7)==item) || ((rnd%7)==num1) ) {continue;} else{next_item=(rnd%7); break;}
		}*/
		item=next_item;
		rnd=get_random_num();
		next_item=(rnd%7);

		if(check_spawn(envmx2, startposaddrs[item])==0)
		{
			add(envmx2, posaddrs[item]);
			moving=1;
		}
		else{Tgame_over(); return;}
		draw_next_item();
	}
	else
	{
		posaddrs[item]=movdown(envmx2, posaddrs[item]);
		if(posaddrs[item][8]==0)
		{
			moving=0;
			item_pos_reset(posaddrs[item], startposaddrs[item]);
		} else{}
	}
	uint8_t y=0, x=0, ytemp=0;
	for(y = _TETR_blocks_y-1; (y>=0)&&(y <= _TETR_blocks_y-1); y--)
	{
		for(x=0; x<_TETR_blocks_x; x++)//sor ellenőrzése hogy megtelt-e
		{
			if(envmx2 [x] [y] != 2)	{ x=0xff; break;}	else{}//ha a sorban van olyan elem ami nem 2 akkor nem telt meg a sor
		}
		if(x!=0xff)
		{
			for(x=0; x<_TETR_blocks_x; x++)//megtelt sor kitörlése
			{
				envmx2[x][y]=0;
			}
			ytemp=y;//y mentése (kitörölt fölötti sor)
			y++;
			for(; y<_TETR_blocks_y; y++)//kitörölt sor fölötti sorok lentebb mozgatása
			{
				for(x=0; x<_TETR_blocks_x; x++)
				{
					if(envmx2[x][y]==2)
					{
						envmx2[x][y-1]=envmx2[x][y];
						envmx2[x][y]=0;
					}else{}
				}
			}
			y=ytemp;//y visszaállítása
			score++;
		} else{}
	}
	fill_rectangle_x1y1_x2y2(49,121,63,127,Pixel_off);
	write_dec_num_int16_t_V(63,121,score,Pixel_on,size_5x8,ALIGN_RIGHT);
	TETR_print(envmx2);
}
//////////////////////////////////////////////
//////////////////////////////////////////////


void draw_next_item(void)
{
	fill_rectangle_x1y1_x2y2(50, 100, 63, 109, Pixel_off);
	for(uint8_t i=0; i<8; i+=2)
	{
		draw_rectangle_xy_height_width( ((startposaddrs[next_item][i]-(_TETR_blocks_x/2))*3)+55, ((startposaddrs[next_item][i+1]-_TETR_blocks_y)*3)+110, 3, 3, Pixel_on);
	}
	/*//deprecated
	switch(next_item)
	{
		case 0:	draw_rectangle_xy_height_width(49, 	 90,   3, 3, Pixel_on);//LL
				draw_rectangle_xy_height_width(49+3, 90,   3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+3, 90+3, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+3, 90+6, 3, 3, Pixel_on);
				break;
		case 1:	draw_rectangle_xy_height_width(49,   90,   3, 3, Pixel_on);//LR
				draw_rectangle_xy_height_width(49+3, 90,   3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49,   90+3, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49,   90+6, 3, 3, Pixel_on);
				break;
		case 2:	draw_rectangle_xy_height_width(49+3, 90,   3, 3, Pixel_on);//ZR
				draw_rectangle_xy_height_width(49+6, 90,   3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49,   90+3, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+3, 90+3, 3, 3, Pixel_on);
				break;
		case 3:	draw_rectangle_xy_height_width(49,   90,   3, 3, Pixel_on);//ZL
				draw_rectangle_xy_height_width(49+3, 90,   3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+3, 90+3, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+6, 90+3, 3, 3, Pixel_on);
				break;
		case 4:	draw_rectangle_xy_height_width(49,   90,   3, 3, Pixel_on);//SQR
				draw_rectangle_xy_height_width(49+3, 90,   3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49,   90+3, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+3, 90+3, 3, 3, Pixel_on);
				break;
		case 5:	draw_rectangle_xy_height_width(49+3, 90,   3, 3, Pixel_on);//T
				draw_rectangle_xy_height_width(49,   90+3, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+3, 90+3, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+6, 90+3, 3, 3, Pixel_on);
				break;
		case 6:	draw_rectangle_xy_height_width(49,   90, 3, 3, Pixel_on);//I
				draw_rectangle_xy_height_width(49+3, 90, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+6, 90, 3, 3, Pixel_on);
				draw_rectangle_xy_height_width(49+9, 90, 3, 3, Pixel_on);
				break;
	}*/
}

void Tgame_over(void)
{
	Tgame_status=1;
	fill_rectangle_x1y1_x2y2(11, 54, 36, 72, Pixel_off);
	write_text_V(12, 65, "GAME", Pixel_on, size_5x8);
	write_text_V(12, 55, "OVER", Pixel_on, size_5x8);
	print_disp_mat();
}

unsigned char *rotate(unsigned char **envmx2f, unsigned char *item_posf)
{
	unsigned char tindex=0;
	unsigned char mat4[4][4]={0};
	unsigned char y=0, x=0, xmin=0, ymin=item_posf[1];//ymin nem változik
	for(xmin=_TETR_blocks_x-1,tindex=0; tindex<8; tindex+=2)
	{
		if(item_posf[tindex] < xmin)	{xmin = item_posf[tindex];}	else{}
	}
	for(tindex=0; tindex<8; tindex+=2 )//elem kimásolása
	{
		mat4 [item_posf[tindex+1]-ymin] [item_posf[tindex]-xmin] = 1; // ...=itemposf[tindex][tindex+1]
	}//	mat4	[y-ymin]				[x-xmin]
	rotateMatrix(mat4);
	unsigned char xmax=0;
	for(y=0; y<4; y++)
	{
		for(x=0; x<4; x++)
		{
			if((mat4[y][x]==1)&&(x>xmax))	{xmax=x;}	else{}
		}
	}
	if((xmin+xmax)<_TETR_blocks_x)
	{
		for(y=0; y<4; y++ )
		{
			for(x=0; (x<4) && (x+xmin<_TETR_blocks_x); x++ )
			{
				if( ((envmx2f [x+xmin] [y+ymin-2]==2) && (mat4[y][x])==1) || ((y+ymin-1)>_TETR_blocks_y) ) // akkor nem lehet forgatni
				{						// *!*!*!!!
					return item_posf; //visszatérés az eredeti item_posf-el
				}	else{}
			}
		}
	}
	else{ return item_posf;}// akkor nem lehet forgatni
	//itt már be lehet illeszteni az elforgatott mátrixot
	for(tindex=0; tindex<8; tindex+=2 )						  //
	{														  //
		envmx2f [item_posf[tindex]] [item_posf[tindex+1]] = 0;//előző elemek törlése
	}
	tindex=0;
	for(y=0; y<4; y++ )
	{
		for(x=0; (x<4) && (x+xmin<_TETR_blocks_x); x++ )
		{
			if(mat4 [y] [x] == 1)
			{
				item_posf[tindex]=x+xmin;  ///elforgatott mátrix elemek másolása a pozíció tömbbe
				item_posf[tindex+1]=y+ymin-2;// *!*!*!!!
				envmx2f [item_posf[tindex]] [item_posf[tindex+1]] = 1;//elforgatott mátrix elemek beillesztése a mátrixba
				tindex+=2;
			}	else{}
		}
	}
	return item_posf;
}

void rotateMatrix(unsigned char mat[][4]) //https://www.geeksforgeeks.org/inplace-rotate-square-matrix-by-90-degrees/
{
	unsigned char x,y;
	// Consider all squares one by one
	for (x = 0; x < 4 / 2; x++)
	{
		// Consider elements in group of 4 in
		// current square
		for (y = x; y < 4-x-1; y++)
		{   // store current cell in temp variable
			unsigned char tempf = mat[x][y];
			// move values from right to top
			mat[x][y] = mat[y][4-1-x];
			// move values from bottom to right
			mat[y][4-1-x] = mat[4-1-x][4-1-y];
			// move values from left to bottom
			mat[4-1-x][4-1-y] = mat[4-1-y][x];
			// assign temp to left
			mat[4-1-y][x] = tempf;
		}
	}
}

unsigned char *putdown(unsigned char **envmx2f, unsigned char *item_posf)
{
	while(item_posf[8]==1)
	{
		item_posf=movdown(envmx2f, item_posf);
	}
	return item_posf;
}

unsigned char *movright(unsigned char **envmx2f, unsigned char *item_posf)
{
	unsigned char tindex,xmax;
	for(xmax=0,tindex=0; tindex<8; tindex+=2)
	{
		if(item_posf[tindex] > xmax)	{xmax = item_posf[tindex];}
		else{}
	}
	if(xmax != (_TETR_blocks_x-1))//akkor lehet jobbra vinni, még nincs a legszélén
	{
		for(tindex=0; tindex<8; tindex+=2 )
		{
			if( envmx2f [item_posf[tindex]+1] [item_posf[tindex+1]] == 2)	{ return item_posf;}//akkor nem lehet jobbra vinni, lerakott elem van mellette
			else{}
		}
		for(tindex=7; (tindex<=7)&&(tindex>=0); tindex-=2 )
		{
			envmx2f [item_posf[tindex-1]+1] [item_posf[tindex]] = 1; //x+1 - edik és y - odik = 1
			envmx2f [item_posf[tindex-1]] [item_posf[tindex]] = 0;	//x -edik és y -odik =0
			item_posf[tindex-1]++;//x érték (koordináta) növelése
		}
	}
	return item_posf;
}

unsigned char *movleft(unsigned char **envmx2f, unsigned char *item_posf)
{
	unsigned char tindex, xmin;
	for(xmin=_TETR_blocks_x+3,tindex=0; tindex<8; tindex+=2)
	{
		if(item_posf[tindex] < xmin)	{xmin = item_posf[tindex];}	else{}
	}
	if(xmin!=0)
	{
		for(tindex=0; tindex<8; tindex+=2 )
		{
			if( envmx2f [item_posf[tindex]-1] [item_posf[tindex+1]] == 2)	{ return item_posf;}	else{}
		}
		for(tindex=0; tindex<8; tindex+=2 )
		{
			envmx2f [item_posf[tindex]-1] [item_posf[tindex+1]] = 1;
			envmx2f [item_posf[tindex]] [item_posf[tindex+1]] = 0;
			item_posf[tindex]--;//x érték csökkentése
		}
	}
	return item_posf;
}

void item_1_2(unsigned char **envmx2f, unsigned char *item_posf)
{
	unsigned char tindex=0;
	for(tindex=0; tindex<8; tindex+=2 )
	{
		envmx2f [item_posf[tindex]] [item_posf[tindex+1]] = 2;
	}
}

unsigned char *movdown(unsigned char **envmx2f, unsigned char *item_posf)
{
	if(item_posf[8]==0 || item_posf[1]==0)		{item_posf[8]=0; moving=0; item_1_2(envmx2f,item_posf); return item_posf;}//bugfix
	unsigned char tindex=0;
	for(tindex=0; tindex<8; tindex+=2 )
	{
		if( envmx2f [item_posf[tindex]] [item_posf[tindex+1]-1] == 2)//van-e alatta lerakott elem
		{
			item_posf[8]=0;
			item_1_2(envmx2f,item_posf);
			moving=0;
			return item_posf;
		} else{}
	}
	for(tindex=0; tindex<8; tindex+=2 )
	{
		envmx2f [item_posf[tindex]] [item_posf[tindex+1]-1] = 1;
		envmx2f [item_posf[tindex]] [item_posf[tindex+1]] = 0;
		item_posf[tindex+1]--;//y érték csökkentése
	}
	if(item_posf[1]==0) //item_posf[1] az elem első y koordinátája (legalsó része)
	{
		item_posf[8]=0;
		moving=0;
		item_1_2(envmx2f,item_posf);
	} else{}
	return item_posf;
}

void item_pos_reset(unsigned char *item_posf, unsigned char *itemstartposf)
{
	unsigned char i=0;
	for(i=0; i<9; i++)
	{
		item_posf[i]=itemstartposf[i];
	}
}

unsigned char check_spawn(unsigned char **envmx2f, unsigned char *item_posf)
{//ha van valami azon a helyen ahol az elemnek 1 es értéke lenne return 1(nem lehet berakni az elemet,game over), ha nincs return 0
	unsigned char tindex=0;
	for(tindex=0; tindex<8; tindex+=2 )
	{
		if( envmx2f [item_posf[tindex]] [item_posf[tindex+1]] !=0)	{return 1;}	else{}
	}
	return 0;
}

void add(unsigned char **envmx2f,unsigned char *item_posf)
{
	unsigned char tindex=0;
	for(tindex=0; tindex<8; tindex+=2 )
	{
		envmx2f [item_posf[tindex]] [item_posf[tindex+1]] = 1;
	}
}

void TETR_print(unsigned char **envmx2f)
{
	fill_rectangle_x1y1_x2y2(2,2,46,125,Pixel_off);//játéktér törlése
	fill_rectangle_x1y1_x2y2(48,121,63,127,Pixel_off);
	write_dec_num_int16_t_V(63,121,score,Pixel_on,size_5x8,ALIGN_RIGHT);
	uint8_t i=0,j=0;
	for(i=0; i<_TETR_blocks_y; i++)//játéktér írása aktuális elemekkel
	{
		for(j=0; j<_TETR_blocks_x; j++)
		{
			if(envmx2f[j][i])
			{
				draw_rectangle_xy_height_width((j*4)+2, (i*4)+2, 4, 4, Pixel_on);// 1 data pixel -> 4x4 rectangle in disp
			} else{}
		}
	}
	print_disp_mat();
}

#endif //STM32_bikecomputer_2

#endif	//_Tetrisgame_c
