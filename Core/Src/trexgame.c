/*
 * trexgame.c
 *
 *  Created on: 2021. márc. 26.
 *      Author: Póti Szabolcs
 */


#ifndef _trexgame_c
#define _trexgame_c

#include "main.h"
#ifdef STM32_bikecomputer_2

#include "ST7565_64x128_LCD.h"
#include "disp_fgv.h"
#include "Fonts_and_bitmaps_FLASH.h"

extern constant uint8_t bmp_bigcactus1[50];
extern constant uint8_t bmp_smallcactus1[19];
extern constant uint8_t bmp_smallcactus2[36];
extern constant uint8_t bmp_smallcactus3[19];
extern constant uint8_t bmp_misccactus_group[98];

extern constant uint8_t bmp_trex_normal_head[18];
extern constant uint8_t bmp_trex_died_head[18];
extern constant uint8_t bmp_trex_normpos_body[22];
extern constant uint8_t bmp_trex_normpos_2legdown[6];
extern constant uint8_t bmp_trex_normpos_llegdown[6];
extern constant uint8_t bmp_trex_normpos_rlegdown[6];

extern constant uint8_t bmp_trex_duck_bodyhead[46];
extern constant uint8_t bmp_trex_duck_llegdown[6];
extern constant uint8_t bmp_trex_duck_rlegdown[6];

extern constant uint8_t bmp_pterodactyl_wingdown[47];
extern constant uint8_t bmp_pterodactyl_wingup[41];

extern uint8_t btn;

int trexgame(void);

int trexgame(void)
{
	btn=0;
	delete_disp_mat();
	draw_line_y(0, 127, 3, Pixel_on);
	//print_bmp_V(6, 127, bmp_trex_duck_bodyhead, Pixel_on, Merge);
	print_bmp_V(18, 116, bmp_trex_normal_head, Pixel_on, Merge);
	print_bmp_V(8, 125, bmp_trex_normpos_body, Pixel_on, Merge);


	print_bmp_V(4, 50, bmp_misccactus_group, Pixel_on, Merge);

	//TODO isr function pointer setup
	//print_disp_mat();
	while(!btn)
	{
		/*print_bmp_V(4, 120, bmp_trex_duck_llegdown, Pixel_on, Overwrite);
		print_disp_mat();
		LL_mDelay(200);
		print_bmp_V(4, 120, bmp_trex_duck_rlegdown, Pixel_on, Overwrite);
		print_disp_mat();
		LL_mDelay(200);*/
		print_bmp_V(4, 120, bmp_trex_normpos_llegdown, Pixel_on, Overwrite);
		setpixel(7,113,Pixel_off);
		print_disp_mat();
		LL_mDelay(200);
		print_bmp_V(4, 120, bmp_trex_normpos_rlegdown, Pixel_on, Overwrite);
		print_disp_mat();
		LL_mDelay(200);
	}
	return 0;
}

#endif//STM32_bikecomputer_2







#endif//_trexgame_c
