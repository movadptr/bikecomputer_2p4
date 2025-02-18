/*
 * Minesweepergame.c
 *
 * Created: 2020. 05. 30. 11:57:40
 * Author : Póti Szabolcs
 */
// 5*5 pixeles cellák, alapból mindegyiken fekete keret, kiválasztva a keret eltűnik,
// körülötte lévő aknák számát a benne lévő fekete pixelek adják meg
// Ami mellett nincs akna az teljesen fekete
// Aknának jelölt cella: +
// Felfedetlen: üres
// Felrobbant: x
// Aknának jelölés  exitgomb+entergomb
// Felfedés 	balgomb+jobbgomb
// Felfedett cellán balgomb+jobbgomb a körülötte lévő összes cellát felfedi//automatikus felfedés
// A cellákban alapból a körülötte lévő aknák száma van, ha fel van fedve akkor +10 (void_id)
// Felső 8 pixel magas sávban: maradék bombák száma, eltelt másodpercek
// játék vége után gombnyomásra lép ki

//original minesweeper settings:	level	size	mines
//									easy	9x9		10
//									medium	16x16	40
//									hard	30x16	99
#ifndef _Minesweepergame_c
#define _Minesweepergame_c

#include "main.h"
#include "ST7565_64x128_LCD.h"
#include "disp_fgv.h"
#include "Fonts_and_bitmaps_FLASH.h"
#include "Alt_fuggvenyek.h"

#ifdef STM32_bikecomputer_2

#define _MS_blocks_x		15
#define _MS_blocks_y		30
#define numofmines			91U
#define _blocksize			5U

#define flag_				100U//ezt az értéket adja hozzá ha zászlóval kijelöli a cellát, így könnyű lesz megkülönböztetni
#define detonated_mine_id	19U
#define mine_id				9U
#define eight_id			18U
#define seven_id			17U
#define six_id				16U
#define five_id				15U
#define four_id				14U
#define three_id			13U
#define two_id				12U
#define one_id				11U
#define void_id				10U

int Minesweepergame(void);
void MS_init(uint8_t** MS_mat);
void MS_print(uint8_t** MS_mat);
uint8_t ismember_(uint16_t* ptf, uint16_t num);//return 1 ha ptf tömbben van olyan elem mint a num; egyébként return 0
#define end_num		((_MS_blocks_y*_MS_blocks_x)+1)//ez jelzi a tömb végét amiben az ismember_() keres
void MS_game_over(uint8_t** MS_mat);
uint8_t reveale_cell_env(uint8_t MSyf, uint8_t MSxf, uint8_t** MS_mat);
void reveale_void_field(uint8_t MSyf, uint8_t MSxf, uint8_t** MS_mat);
uint8_t win(int8_t mines_left, uint8_t** MS_mat);

extern volatile uint8_t btn;


//minesweeper game start logo bitmap
constant uint8_t bmp_mine[122]={30,30,0x00,0x00,0x00,0x00,0x06,0x0F,0x0B,0x06,0x06,0x08,0x08,0x12,0x10,0x70,0x90,0xF0,0x70,0x10,0x12,0x08,
									0x0D,0x07,0x06,0x0B,0x0F,0x06,0x00,0x00,0x00,0x00,0x03,0x05,0x05,0x1F,0x60,0x81,0x80,0x10,0x00,0x04,
									0x02,0x00,0x13,0x07,0xAC,0x0C,0x27,0x93,0x0D,0xB2,0x4B,0x94,0xEB,0xFF,0xFF,0x7F,0x1F,0x05,0x05,0x03,
									0x00,0x80,0x80,0xE0,0x39,0x1E,0x4F,0x9F,0x37,0x0F,0xAB,0x17,0x6F,0x93,0xEF,0xE7,0xD7,0xBF,0xCF,0x57,
									0x3F,0xDF,0x7F,0xFF,0xFE,0xF9,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x80,0x80,0xC0,
									0xC0,0xE0,0xE0,0xF8,0xFC,0xE4,0xF8,0xE0,0xE0,0xC0,0xC0,0x80,0x80,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00};


int Minesweepergame(void)
{
	uint8_t MS__mat[_MS_blocks_y][_MS_blocks_x]={0};//játéktér
	uint8_t* MS_mat[_MS_blocks_y];
	uint8_t MSx=0, MSy=0;//MS_mat címzésére
	delete_disp_mat();
	print_bmp_V(16, 79, bmp_mine, Pixel_on, Merge);
	write_text_V(2, 40, "Minesweeper", Pixel_on, size_5x8);
	print_disp_mat();
	delete_disp_mat();
	LL_mDelay(1000);
	while(MSy<_MS_blocks_y)//**
	{
		MS_mat[MSy]= MS__mat[MSy];
		MSy++;
	}
	MS_init(MS_mat);
	int8_t mines_left=numofmines;
	write_dec_num_int16_t_V(60, 121, mines_left, Pixel_on, size_5x8, ALIGN_RIGHT);
	MSy=0;//kezdő pozíció
	MSx=0;//
	// todo másodpercek számolását megkezdeni
	MS_print(MS_mat);
	while(1)//főcilus
	{
		draw_rectangle_xy_height_width(MSx*(_blocksize-1), MSy*(_blocksize-1), _blocksize, _blocksize, Pixel_off);//draw cursor
		print_disp_mat();
		while(!btn)	{ asm("nop");}//amíg nem nyomok semmit itt ciklik
		LL_mDelay(80);//dupla gombnyomáshoz
		draw_rectangle_xy_height_width(MSx*(_blocksize-1),MSy*(_blocksize-1), _blocksize, _blocksize, Pixel_on);//erase cursor
		if( (btn == balgomb) && (MSx > 0) )						{ MSx--;}	else{}//balra
		if( (btn == jobbgomb) && (MSx < (_MS_blocks_x-1)) )		{ MSx++;}	else{}//jobbra
		if( (btn == exitgomb) && (MSy > 0) )					{ MSy--;}	else{}//le
		if( (btn == entergomb) && (MSy < (_MS_blocks_y-1)) )	{ MSy++;}	else{}//fel
		if( btn == (entergomb | exitgomb) )
		{
			if( MS_mat[MSy][MSx] >= flag_ )
			{
				MS_mat[MSy][MSx]-=flag_;//zászlót felszed
				mines_left++;
			}
			else
			{
				if( MS_mat[MSy][MSx] <= mine_id )//csak akkor rakható le ha a mező nincs felfedve
				{
					MS_mat[MSy][MSx]+=flag_;//zászlót lerak
					mines_left--;
				}else{}
			}
			fill_rectangle_x1y1_x2y2(40, 121, pixels_x-1, pixels_y-1, Pixel_off);
			write_dec_num_int16_t_V(60, 121, mines_left, Pixel_on, size_5x8, ALIGN_RIGHT);
			MS_print(MS_mat);
		}else{}
		if( btn == (balgomb | jobbgomb) )//felfedés
		{
			if(  MS_mat[MSy][MSx]<=mine_id )//egy cella felfedése //így automatikusan benne van hogy zászlót nem fed fel
			{
				if(MS_mat[MSy][MSx] == 0)	{ reveale_void_field(MSy, MSx, MS_mat);}
				else
				{
					MS_mat[MSy][MSx]+=void_id;
					if( MS_mat[MSy][MSx] == detonated_mine_id )	{ MS_game_over(MS_mat); return 0;} else{}
				}
			}
			else
			{
				if( (MS_mat[MSy][MSx] >= void_id) && (MS_mat[MSy][MSx] <= eight_id) )//automatikus felfedés "nem üres cellára kattintva"
				{
					if( reveale_cell_env(MSy, MSx, MS_mat) )	{ MS_game_over(MS_mat); return 0;}	else{}
				}else{}
			}
			MS_print(MS_mat);
		}else{}
		if( win(mines_left, MS_mat) == 1 )	{ break;}	else{}
		btn=0;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////

uint8_t win(int8_t mines_leftf, uint8_t** MS_mat)
{
	if(mines_leftf == 0)//ha van felfedetlen cella akkor még nem nyert, akkor se ha van rajta flag
	{
		uint8_t ix=0, iy=0;
		for(iy=0; iy<_MS_blocks_y; iy++)
		{
			for(ix=0; ix<_MS_blocks_x; ix++)
			{
				if( (MS_mat[iy][ix] < void_id)  || ((MS_mat[iy][ix] > eight_id) && (MS_mat[iy][ix] != (mine_id+flag_))) )
				{
					return 0;
				}	else{}
			}
		}
		//todo másodpercek számolását leállítani
		write_text_V(0, 121, "You won!", Pixel_on, size_5x8);
		print_disp_mat();
		btn=0;
		while(!btn)	{ asm("nop");}//gombnyomásra fejeződik be a játék
		return 1;
	}
	else{return 0;}
}
/*uint8_t win(int8_t mines_leftf, uint8_t** MS_mat)
{
	uint16_t db=0;
	if(mines_leftf == 0)//ha van felfedetlen cella akkor még nem nyert, akkor se ha van rajta flag
	{
		uint8_t ix=0, iy=0;
		for(iy=0; iy<_MS_blocks_y; iy++)
		{
			for(ix=0; ix<_MS_blocks_x; ix++)
			{
				if( (MS_mat[iy][ix] >= void_id)  || (MS_mat[iy][ix] == (mine_id+flag_)) )
				{
					db++;
				}	else{}
			}
		}
	}else{}
	if( db == (_MS_blocks_y*_MS_blocks_x) )//nyert
	{
		//todo másodpercek számolását leállítani
		write_text(0, 121, "You won!", Pixel_on, size_5x8);
		print_disp_mat();
		btn=0;
		while(!btn)	{ asm("nop");}//gombnyomásra fejeződik be a játék
		return 1;
	}
	else{ return 0;}
}*/

void reveale_void_field(uint8_t MSyf, uint8_t MSxf, uint8_t** MS_mat)
{
	MS_mat[MSyf][MSxf]+=void_id;//kezdő void cella felfedése
	int8_t yj, xj;
	for(yj=-1; yj<2; yj++)
	{
		for(xj=-1; xj<2; xj++)
		{
			if( (((MSyf+yj) >= 0) && ((MSyf+yj) < _MS_blocks_y)) && (((MSxf+xj) >= 0) && ((MSxf+xj) < _MS_blocks_x))  )//túlcímzés elkerülése
			{
				if(MS_mat[MSyf+yj][MSxf+xj] == 0)
				{
					reveale_void_field(MSyf+yj, MSxf+xj, MS_mat);
				}
				else//ha nem void cella, akkor csak valamilyen szám lehet, ha nincs felfedve, fel kell fedni, mert a void terület szélén van
				{
					if(MS_mat[MSyf+yj][MSxf+xj] < void_id)	{MS_mat[MSyf+yj][MSxf+xj] += void_id;}	else{}
				}
			}else{}
		}
	}
}
/*
void reveale_void_field(uint8_t MSyf, uint8_t MSxf, uint8_t** MS_mat)
{
	MS_mat[MSyf][MSxf]+=void_id;//kezdő void cella felfedése
	uint8_t coord[16]={0}, k=0, db=0;;
	int8_t yj, xj;
	for(yj=-1; yj<2; yj++)
	{
		for(xj=-1; xj<2; xj++)
		{
			if( (((MSyf+yj) >= 0) && ((MSyf+yj) < _MS_blocks_y)) && (((MSxf+xj) >= 0) && ((MSxf+xj) < _MS_blocks_x))  )//túlcímzés elkerülése
			{
				if(MS_mat[MSyf+yj][MSxf+xj] == 0)//void cellák koordinátáinak meghatározása
				{
					coord[k]=MSyf+yj;
					coord[k+1]=MSxf+xj;
					k+=2;
					db++;
				}
				else//ha nem void cella, akkor csak valamilyen szám lehet, ha nincs felfedve, fel kell fedni, mert a void terület szélén van
				{
					if(MS_mat[MSyf+yj][MSxf+xj] < void_id)	{MS_mat[MSyf+yj][MSxf+xj] += void_id;}	else{}
				}
			}else{}
		}
	}
	for(k=0; db>0; k+=2,db--)
	{
		if(MS_mat[coord[k]] [coord[k+1]] == 0) { reveale_void_field(coord[k], coord[k+1], MS_mat);}	else{}//rekurzió
	}
}*/

uint8_t reveale_cell_env(uint8_t MSyf, uint8_t MSxf, uint8_t** MS_mat)
{
	int8_t yj, xj, dm=0;
	for(yj=-1; yj<2; yj++)
	{
		for(xj=-1; xj<2; xj++)
		{
			if( (((MSyf+yj) >= 0) && ((MSyf+yj) < _MS_blocks_y)) && (((MSxf+xj) >= 0) && ((MSxf+xj) < _MS_blocks_x))  )//túlcímzés elkerülése
			{
				if( (MS_mat[MSyf+yj][MSxf+xj] == mine_id) || (MS_mat[MSyf+yj][MSxf+xj] < void_id) )	//felfedett cellákat és flag-eket ne rontsa el
				{
					if(MS_mat[MSyf+yj][MSxf+xj] == 0)	{ reveale_void_field(MSyf+yj, MSxf+xj, MS_mat);}
					else{ MS_mat[MSyf+yj][MSxf+xj] += void_id;}
				}	else{}
				if( MS_mat[MSyf+yj][MSxf+xj] == detonated_mine_id )	{ dm++;}	else{}//felrobbant aknák száma
			}else{}
		}
	}
	return dm;
}

void MS_game_over(uint8_t** MS_mat)
{
	//todo másodpercek számolását leállítani
	uint8_t ix=0, iy=0;
	for(iy=0; iy<_MS_blocks_y; iy++)//megmaradt aknák felfedése
	{
		for(ix=0; ix<_MS_blocks_x; ix++)
		{
			if(MS_mat[iy][ix] == mine_id)
			{
				MS_mat[iy][ix] += void_id;
			}	else{}
		}
	}
	MS_print(MS_mat);//még utoljára frissítjük a képet, hogy a felrobbant akna is látszódjon
	btn=0;
	while(!btn)	{ asm("nop");}//gombnyomásra fejeződik be a játék
}

uint8_t ismember_(uint16_t* ptf, uint16_t num)
{
	uint8_t k=0;
	while( ptf[k] != end_num )//
	{
		if( ptf[k] == num)	{ return 1;}	else{ k++;}
	}
	return 0;
}

void MS_print(uint8_t** MS_mat)
{
	uint8_t ix=0, iy=0;
	fill_rectangle_x1y1_x2y2(0, 0, _MS_blocks_x*(_blocksize-1), _MS_blocks_y*(_blocksize-1), Pixel_off);//játéktér törlése
	for(ix=0; ix<=(_MS_blocks_x*(_blocksize-1)); ix+=(_blocksize-1))//print grid
	{
		draw_line_y(0, _MS_blocks_y*(_blocksize-1), ix, Pixel_on);
	}
	for(ix=0; ix<=(_MS_blocks_y*(_blocksize-1)); ix+=(_blocksize-1))
	{
		draw_line_x(0, _MS_blocks_x*(_blocksize-1), ix, Pixel_on);
	}
	for(iy=0; iy<_MS_blocks_y; iy++)//print content
	{
		for(ix=0; ix<_MS_blocks_x; ix++)
		{
			if(MS_mat[iy][ix] >= flag_)
			{
				setpixel( (ix*(_blocksize-1))+2 ,(iy*(_blocksize-1))+2 , Pixel_on);
				setpixel( (ix*(_blocksize-1))+1 ,(iy*(_blocksize-1))+2 , Pixel_on);
				setpixel( (ix*(_blocksize-1))+2 ,(iy*(_blocksize-1))+3 , Pixel_on);
				setpixel( (ix*(_blocksize-1))+3 ,(iy*(_blocksize-1))+2 , Pixel_on);
				setpixel( (ix*(_blocksize-1))+2 ,(iy*(_blocksize-1))+1 , Pixel_on);
			}	else{}
			switch(MS_mat[iy][ix])
			{
				case detonated_mine_id: setpixel( (ix*(_blocksize-1))+2 , (iy*(_blocksize-1))+2 , Pixel_on);
										setpixel( (ix*(_blocksize-1))+1 , (iy*(_blocksize-1))+1 , Pixel_on);
										setpixel( (ix*(_blocksize-1))+1 , (iy*(_blocksize-1))+3 , Pixel_on);
										setpixel( (ix*(_blocksize-1))+3 , (iy*(_blocksize-1))+3 , Pixel_on);
										setpixel( (ix*(_blocksize-1))+3 , (iy*(_blocksize-1))+1 , Pixel_on);
										break;
				case void_id:	fill_rectangle_xy_height_width( (ix*(_blocksize-1))+1, (iy*(_blocksize-1))+1, _blocksize-2, _blocksize-2, Pixel_on);
								break;

				case eight_id:	setpixel( (ix*(_blocksize-1))+2 , (iy*(_blocksize-1))+1 , Pixel_on);
				//fall through
				case seven_id:	setpixel( (ix*(_blocksize-1))+3 , (iy*(_blocksize-1))+1 , Pixel_on);
				//fall through
				case six_id:	setpixel( (ix*(_blocksize-1))+3 , (iy*(_blocksize-1))+2 , Pixel_on);
				//fall through
				case five_id:	setpixel( (ix*(_blocksize-1))+3 , (iy*(_blocksize-1))+3 , Pixel_on);
				//fall through
				case four_id:	setpixel( (ix*(_blocksize-1))+2 , (iy*(_blocksize-1))+3 , Pixel_on);
				//fall through
				case three_id:	setpixel( (ix*(_blocksize-1))+1 , (iy*(_blocksize-1))+3 , Pixel_on);
				//fall through
				case two_id:	setpixel( (ix*(_blocksize-1))+1 , (iy*(_blocksize-1))+2 , Pixel_on);
				//fall through
				case one_id:	//setpixel( (ix*(_blocksize-1))+1 , (_MS_blocks_y*(_blocksize-1))-(_blocksize-1)-(iy*(_blocksize-1))+1 , Pixel_on);
								setpixel( (ix*(_blocksize-1))+1 , (iy*(_blocksize-1))+1 , Pixel_on);
			}
		}
	}
	print_disp_mat();
}

void MS_init(uint8_t** MS_mat)
{
	uint8_t tmp=0;//futó változó
	uint16_t tmp2=0;
	uint16_t mt[numofmines+1];//random számok az aknákhoz; +1 -> lezáró elem
	for(tmp=0; tmp<numofmines; tmp++)//tömb feltöltése end_num értékkel, hogy gyorsabb legyen a keresés, és hogy lehessen akna a nullás pozícióban
	{
		mt[tmp]=end_num;
	}
	LL_RNG_Enable(RNG);
	for(tmp=0; tmp<numofmines; )//generate mines
	{
		tmp2 = get_random_num();
		tmp2 = tmp2 % (_MS_blocks_y*_MS_blocks_x);//32 bites nyers random számból [0,_MS_blocks_y*_MS_blocks_x[ intervallumban lévő szám generálása
		if( (ismember_(mt, tmp2) == 0) && (tmp2!=0) )		{ mt[tmp] = tmp2; tmp++;}	else{}//ne generáljon többször ugyanabba a cellába, a kezdő pozícióba (nulladik cella) ne generáljon aknát
	}
	LL_RNG_Disable(RNG);
	for(tmp=0; tmp<(numofmines); tmp++)//aknák elpakolása a cellákba
	{
		MS_mat[mt[tmp]/_MS_blocks_x] [( mt[tmp] % _MS_blocks_x)] = mine_id;
	}
	uint8_t  yi=0 ,xi=0;//indexelés
	int8_t xj, yj;//futó változó, cella körüli cellák vizsgálatára
	for(yi=0; yi<_MS_blocks_y; yi++)//generate numbers
	{
		for(xi=0; xi<_MS_blocks_x; xi++)
		{
			if(MS_mat[yi][xi] == 0)
			{
				for(tmp=0,yj=-1; yj<2; yj++)
				{
					for(xj=-1; xj<2; xj++)
					{
						if( (((yi+yj) >= 0) && ((yi+yj) < _MS_blocks_y)) && (((xi+xj) >= 0) && ((xi+xj) < _MS_blocks_x)) )//túlcímzés elkerülése
						{
							if(MS_mat[yi+yj][xi+xj] == mine_id)	{ tmp++;}	else{}//cella körüli aknák száma
						}else{}
					}
				}
				MS_mat[yi][xi]=tmp;//aknák számának beírása a cellába
			}else{}
		}
	}
}

#if ( numofmines >= ((_MS_blocks_x*_MS_blocks_y)/2) )
	#line 33
	#error Number of mines is too large
#endif

#endif//#ifdef STM32_bikecomputer_2


#ifdef _Terminal

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define disp_mode   d_nogrid
#define d_grid      1
#define d_nogrid    0

unsigned int _MS_blocks_x;
unsigned int _MS_blocks_y;
unsigned int numofmines;
unsigned int end_num;

#define flag_				100//ezt az értéket adja hozzá ha zászlóval kijelöli a cellát, így könnyű lesz megkülönböztetni
#define detonated_mine_id	19
#define mine_id				9
#define eight_id			18
#define seven_id			17
#define six_id				16
#define five_id				15
#define four_id				14
#define three_id			13
#define two_id				12
#define one_id				11
#define void_id				10

int Minesweepergame(void);
void MS_init(unsigned int** MS_mat);
void MS_print(unsigned int** MS_mat, unsigned int MSyf, unsigned int MSxf);
unsigned int ismember_(unsigned int* ptf, unsigned int num);
void MS_game_over(unsigned int** MS_mat, unsigned int MSyf, unsigned int MSxf);
unsigned int reveale_cell_env(unsigned int MSyf, unsigned int MSxf, unsigned int** MS_mat);
void reveale_void_field(unsigned int MSyf, unsigned int MSxf, unsigned int** MS_mat);
unsigned int win(int mines_left, unsigned int** MS_mat);

int Minesweepergame(void)
{
    char btn=0;
	unsigned int MSx=0, MSy=0;//MS_mat címzésére
    int mines_left=0;
    unsigned int** MS_mat;

    while(1)//menü ciklus
    {
        system("cls");

        printf("original minesweeper settings:\nlevel	size	mines\neasy	9x9     10\nmedium	16x16	40\nhard	30x16	99\n\n");
        printf("\n\ncontrols:\nmoveing:\twasd\nuse\tflag:\tq\nclick:\t\te");

        printf("\n\ngive the width of the minefield  ");
        scanf("%i", &_MS_blocks_x);
        printf("give the height of the minefield  ");
        scanf("%i", &_MS_blocks_y);
        printf("give the number of mines  ");
        scanf("%i", &numofmines);

        end_num	= ((_MS_blocks_y*_MS_blocks_x)+1);//ez jelzi a tömb végét amiben az ismember_() keres

        MS_mat = calloc(_MS_blocks_y, sizeof(unsigned int *));
        for(MSy = 0; MSy < _MS_blocks_y; MSy++)
        {
            MS_mat[MSy] = (unsigned int *)calloc(_MS_blocks_x, sizeof(unsigned int));
        }

        mines_left=numofmines;
        MSy=0;//kezdő pozíció
	    MSx=0;//
        MS_init(MS_mat);

        while(1)//főcilus
        {
            MS_print(MS_mat,MSy,MSx);
            printf("\n mines left: %i\n", mines_left);
            scanf("%c", &btn);

            if( (btn == 'a') && (MSx > 0) )						{ MSx--;}	else{}//balra
            if( (btn == 'd') && (MSx < (_MS_blocks_x-1)) )		{ MSx++;}	else{}//jobbra
            if( (btn == 'w') && (MSy > 0) )					{ MSy--;}	else{}//le
            if( (btn == 's') && (MSy < (_MS_blocks_y-1)) )	{ MSy++;}	else{}//fel
            if( btn == 'q' )
            {
                if( MS_mat[MSy][MSx] >= flag_ )
                {
                    MS_mat[MSy][MSx]-=flag_;//zászlót felszed
                    mines_left++;
                }
                else
                {
                    if( MS_mat[MSy][MSx] <= mine_id )//csak akkor rakható le ha a mező nincs felfedve
                    {
                        MS_mat[MSy][MSx]+=flag_;//zászlót lerak
                        mines_left--;
                    }else{}
                }
                MS_print(MS_mat,MSy,MSx);
            }else{}
            if( btn == ('e') )//felfedés
            {
                if(  MS_mat[MSy][MSx]<=mine_id )//egy cella felfedése //így automatikusan benne van hogy zászlót nem fed fel
                {
                    if(MS_mat[MSy][MSx] == 0)	{ reveale_void_field(MSy, MSx, MS_mat);}
                    else
                    {
                        MS_mat[MSy][MSx]+=void_id;
                        if( MS_mat[MSy][MSx] == detonated_mine_id )	{ MS_game_over(MS_mat,MSy,MSx); break;} else{}
                    }
                }
                else
                {
                    if( (MS_mat[MSy][MSx] >= void_id) && (MS_mat[MSy][MSx] <= eight_id) )//automatikus felfedés "nem üres cellára kattintva"
                    {
                        if( reveale_cell_env(MSy, MSx, MS_mat) )	{ MS_game_over(MS_mat,MSy,MSx); break;}	else{}
                    }else{}
                }
                MS_print(MS_mat,MSy,MSx);
            }else{}
            if( win(mines_left, MS_mat) == 1 )	{ break;}	else{}
            btn=0;
        }

        free(MS_mat);

        printf("\n\nDo you want to play again? (y,n)\n");
        scanf("%c", &btn);
        while(1)
        {
            scanf("%c", &btn);
            if( (btn=='y') || (btn=='n') )  { break;}
        }
        if(btn == 'n')    { return 0;}
        else { continue;}
    }
	return 0;
}
/////////////////////////////////////////////////////////////////

unsigned int win(int mines_leftf, unsigned int** MS_mat)
{
	if(mines_leftf == 0)//ha van felfedetlen cella akkor még nem nyert, akkor se ha van rajta flag
	{
		unsigned int ix=0, iy=0;
		for(iy=0; iy<_MS_blocks_y; iy++)
		{
			for(ix=0; ix<_MS_blocks_x; ix++)
			{
				if( (MS_mat[iy][ix] < void_id)  || ((MS_mat[iy][ix] > eight_id) && (MS_mat[iy][ix] != (mine_id+flag_))) )
				{
					return 0;
				}	else{}
			}
		}
		printf("YOU WON!");
		return 1;
	}
	else{return 0;}
}

void reveale_void_field(unsigned int MSyf, unsigned int MSxf, unsigned int** MS_mat)
{
	MS_mat[MSyf][MSxf]+=void_id;//kezdő void cella felfedése
	int yj, xj;
	for(yj=-1; yj<2; yj++)
	{
		for(xj=-1; xj<2; xj++)
		{
			if( (((MSyf+yj) >= 0) && ((MSyf+yj) < _MS_blocks_y)) && (((MSxf+xj) >= 0) && ((MSxf+xj) < _MS_blocks_x))  )//túlcímzés elkerülése
			{
				if(MS_mat[MSyf+yj][MSxf+xj] == 0)
				{
					reveale_void_field(MSyf+yj, MSxf+xj, MS_mat);
				}
				else//ha nem void cella, akkor csak valamilyen szám lehet, ha nincs felfedve, fel kell fedni, mert a void terület szélén van
				{
					if(MS_mat[MSyf+yj][MSxf+xj] < void_id)	{MS_mat[MSyf+yj][MSxf+xj] += void_id;}	else{}
				}
			}else{}
		}
	}
}

unsigned int reveale_cell_env(unsigned int MSyf, unsigned int MSxf, unsigned int** MS_mat)
{
	int yj, xj, dm=0;
	for(yj=-1; yj<2; yj++)
	{
		for(xj=-1; xj<2; xj++)
		{
			if( (((MSyf+yj) >= 0) && ((MSyf+yj) < _MS_blocks_y)) && (((MSxf+xj) >= 0) && ((MSxf+xj) < _MS_blocks_x))  )//túlcímzés elkerülése
			{
				if( (MS_mat[MSyf+yj][MSxf+xj] == mine_id) || (MS_mat[MSyf+yj][MSxf+xj] < void_id) )	//felfedett cellákat és flag-eket ne rontsa el
				{
					if(MS_mat[MSyf+yj][MSxf+xj] == 0)	{ reveale_void_field(MSyf+yj, MSxf+xj, MS_mat);}
					else{ MS_mat[MSyf+yj][MSxf+xj] += void_id;}
				}	else{}
				if( MS_mat[MSyf+yj][MSxf+xj] == detonated_mine_id )	{ dm++;}	else{}//felrobbant aknák száma
			}else{}
		}
	}
	return dm;
}

void MS_game_over(unsigned int** MS_mat, unsigned int MSyf, unsigned int MSxf)
{
	//todo másodpercek számolását leállítani
	unsigned int ix=0, iy=0;
	for(iy=0; iy<_MS_blocks_y; iy++)//megmaradt aknák felfedése
	{
		for(ix=0; ix<_MS_blocks_x; ix++)
		{
			if(MS_mat[iy][ix] == mine_id)
			{
				MS_mat[iy][ix] += void_id;
			}	else{}
		}
	}
	MS_print(MS_mat,MSyf,MSxf);//még utoljára frissítjük a képet, hogy a felrobbant akna is látszódjon
	printf("GAME OVER");
}

unsigned int ismember_(unsigned int* ptf, unsigned int num)//return 1 ha ptf tömbben van olyan elem mint a num; egyébként return 0
{
	unsigned int k=0;
	while( ptf[k] != end_num )//
	{
		if( ptf[k] == num)	{ return 1;}	else{ k++;}
	}
	return 0;
}

void MS_print(unsigned int** MS_mat, unsigned int MSyf, unsigned int MSxf)
{
	unsigned int ix=0, iy=0;
	int xk=0;
	system("cls");
    printf(" ");
    #if disp_mode ==  d_grid
    for(xk=0; xk<_MS_blocks_x; xk++)
    {
        if( (MSyf==0) && (MSxf==xk) )   { printf("  ");}    else{ printf("- ");}
    }
    #endif

    #if disp_mode ==  d_nogrid
    for(xk=0; xk<_MS_blocks_x; xk++)
    {
        printf("- ");
    }
    #endif

    printf("\n");
	for(iy=0; iy<_MS_blocks_y; iy++)
	{
        #if disp_mode == d_grid
        if((MSxf==0) && (MSyf==iy))   {printf(" ");}  else{ printf("|");}
		#endif

        #if disp_mode == d_nogrid
        printf("|");
		#endif

        for(ix=0; ix<_MS_blocks_x; ix++)
		{
            if(MS_mat[iy][ix] >= flag_)             {printf(",");}
            if(MS_mat[iy][ix] == detonated_mine_id)   {printf("X");}
            if(MS_mat[iy][ix] == void_id)             {printf("%c",(char)219);}
            if( (MS_mat[iy][ix] <= eight_id) && (MS_mat[iy][ix] >= one_id) )  { printf("%i",MS_mat[iy][ix]-10);}
            if( (MS_mat[iy][ix] <= 9) && (MS_mat[iy][ix] >= 0) )  { printf(" ");}

            #if disp_mode ==  d_grid
            if(  ((MSyf==iy) && (ix==MSxf)) || ((MSyf==iy) && (ix+1==MSxf))  )   { printf(" ");}  else{ printf("|");}
            #endif

            #if disp_mode ==  d_nogrid  //kurzor oldala
            if(iy==MSyf)
            {
                if( (((ix==MSxf) || (ix==MSxf-1)) && (ix<_MS_blocks_x-1))   )
                { printf("|");}
                else if(ix!=_MS_blocks_x-1)
                { printf(" ");}
            }
            if((iy!=MSyf) && (ix<_MS_blocks_x-1))    {printf(" ");}
            #endif
		}

        #if disp_mode ==  d_nogrid
        printf("|");//jobb keret
        if((iy ==_MS_blocks_y-1) && (ix ==_MS_blocks_x))   //alsó keret
        {
            printf("\n");
            printf(" ");
            for(xk=0; xk<_MS_blocks_x; xk++)
            {
                printf("- ");
            }
        }
        printf("\n");
        if( ((iy==MSyf)&&(iy!=_MS_blocks_y-1)) || (iy == MSyf-1) )//kurzor alja és teteje
        {
            for(xk=0; xk<_MS_blocks_x; xk++)
            {
                if(xk==MSxf)    {printf(" -");} else{ printf("  ");}
            }
        }
        #endif

        #if disp_mode ==  d_grid
        printf("\n");
        printf(" ");
        for(xk=0; xk<_MS_blocks_x; xk++)
        {
            if(  ((MSyf==iy) && (MSxf==xk)) || ((MSyf-1==iy) && (MSxf==xk))  )   { printf("  ");}    else{ printf("- ");}
        }
        #endif

        printf("\n");
	}
}

void MS_init(unsigned int** MS_mat)
{
	unsigned int tmp=0;//futó változó
	unsigned int tmp2=0;
	unsigned int mt[numofmines+1];//random számok az aknákhoz; +1 -> lezáró elem
	for(tmp=0; tmp<numofmines; tmp++)//tömb feltöltése end_num értékkel, hogy gyorsabb legyen a keresés, és hogy lehessen akna a nullás pozícióban
	{
		mt[tmp]=end_num;
	}

    srand(clock());//init pseudo random number generator
	for(tmp=0; tmp<numofmines; )//generate mines
	{
		tmp2 = rand();
		tmp2 = tmp2 % (_MS_blocks_y*_MS_blocks_x);//nyers random számból [0,_MS_blocks_y*_MS_blocks_x[ intervallumban lévő szám generálása
		if( (ismember_(mt, tmp2) == 0) && (tmp2!=0) )		{ mt[tmp] = tmp2; tmp++;}	else{}//ne generáljon többször ugyanabba a cellába, a kezdő pozícióba (nulladik cella) ne generáljon aknát
	}

	for(tmp=0; tmp<(numofmines); tmp++)//aknák elpakolása a cellákba
	{
		MS_mat[mt[tmp]/_MS_blocks_x] [( mt[tmp] % _MS_blocks_x)] = mine_id;
	}
	unsigned int  yi=0 ,xi=0;//indexelés
	int xj, yj;//futó változó, cella körüli cellák vizsgálatára
	for(yi=0; yi<_MS_blocks_y; yi++)//generate numbers
	{
		for(xi=0; xi<_MS_blocks_x; xi++)
		{
			if(MS_mat[yi][xi] == 0)
			{
				for(tmp=0,yj=-1; yj<2; yj++)
				{
					for(xj=-1; xj<2; xj++)
					{
						if( (((yi+yj) >= 0) && ((yi+yj) < _MS_blocks_y)) && (((xi+xj) >= 0) && ((xi+xj) < _MS_blocks_x)) )//túlcímzés elkerülése
						{
							if(MS_mat[yi+yj][xi+xj] == mine_id)	{ tmp++;}	else{}//cella körüli aknák száma
						}else{}
					}
				}
				MS_mat[yi][xi]=tmp;//aknák számának beírása a cellába
			}else{}
		}
	}
}

#endif//_Terminal

#endif//#ifndef _Minesweepergame_c
