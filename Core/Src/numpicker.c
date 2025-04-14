/*
 * numpicker.c
 *
 *  Created on: Apr 11, 2025
 *  Author: POS1BP
 */

#include "numpicker.h"
#include "ST7565_64x128_LCD.h"
#include "disp_fgv.h"
#include <stdlib.h>
#include "Alt_fuggvenyek.h"

//TODO make float variant
uint32_t numPickerUInt32_V(uint32_t Llimit, uint32_t Hlimit, uint32_t startval, volatile uint8_t* buttons)
{
	*buttons = 0;

	const uint8_t ypos = 100;
	const uint8_t digitSelWidth = 8;

	delete_disp_mat();

	uint8_t numt[10]={0};
	disassembleU32Num(numt, startval);

	uint8_t numOfDigits = 0;//amount of decimal digits
	uint8_t iDigits = 0;//index

	//find out how many digits we will need to display
	for(uint32_t tmp = Hlimit; tmp > 0; numOfDigits++)
	{
		tmp /= 10;
	}

	uint8_t* digitXPosT = calloc(numOfDigits, 1);//allocate var for digit x positions
	for(uint8_t tmpIDigits=0; tmpIDigits < numOfDigits; tmpIDigits++)
	{
		digitXPosT[tmpIDigits] = ((pixels_x-(pixels_x-(numOfDigits*digitSelWidth))/2))-((tmpIDigits+1)*digitSelWidth);//calculate and save digit x positions, centered horizontally
		write_dec_num_uint8_t_V(digitXPosT[tmpIDigits], ypos, numt[tmpIDigits], Pixel_on, size_5x8, ALIGN_LEFT);//at the seme time print the digits
	}

	draw_rectangle_xy_height_width(digitXPosT[0]-2, ypos-2, 11, 9, Pixel_on);//initial cursor pos
	print_disp_mat();

	while(1)
	{
		//select the digit
		if( (*buttons == balgomb) && (iDigits < (numOfDigits-1)) )//move to next digit, to the left
		{
			draw_rectangle_xy_height_width(digitXPosT[iDigits]-2, ypos-2, 11, 9, Pixel_off);//delete prev cursor
			iDigits++;
			draw_rectangle_xy_height_width(digitXPosT[iDigits]-2, ypos-2, 11, 9, Pixel_on);//print cursor
			print_disp_mat();
			tim_delay_ms(menu_delaytime);
		}	else{}
		if( (*buttons == jobbgomb) && (iDigits > 0) )//move to prev digit, to the right
		{
			draw_rectangle_xy_height_width(digitXPosT[iDigits]-2, ypos-2, 11, 9, Pixel_off);//delete prev cursor
			iDigits--;
			draw_rectangle_xy_height_width(digitXPosT[iDigits]-2, ypos-2, 11, 9, Pixel_on);//print cursor
			print_disp_mat();
			tim_delay_ms(menu_delaytime);
		}	else{}
		if(*buttons == entergomb)
		{
			*buttons = 0;
			//modify the selected digit
			while(1)
			{
				if((*buttons == jobbgomb)&& (assembleU32Num(numt)>=Llimit) && ((assembleU32Num(numt)+mypow10(iDigits))<=Hlimit))//move to next digit, to the rigt
				{
					numt[iDigits]++;
					if(numt[iDigits] > 9)	{numt[iDigits] = 9;} else{}//overflow
					fill_rectangle_xy_height_width(digitXPosT[iDigits], ypos, 7, 5, Pixel_off);//delete prev num
					write_dec_num_uint8_t_V(digitXPosT[iDigits], ypos, numt[iDigits], Pixel_on, size_5x8, ALIGN_LEFT);//print num
					print_disp_mat();
					tim_delay_ms(menu_delaytime);
				}	else{}
				if((*buttons == balgomb) && ((assembleU32Num(numt)-mypow10(iDigits))>=Llimit) && (assembleU32Num(numt)<=Hlimit))//move to prev digit, to the left
				{
					numt[iDigits]--;
					if(numt[iDigits] > 9)	{numt[iDigits] = 0;} else{}//underflow
					fill_rectangle_xy_height_width(digitXPosT[iDigits], ypos, 7, 5, Pixel_off);//delete prev num
					write_dec_num_uint8_t_V(digitXPosT[iDigits], ypos, numt[iDigits], Pixel_on, size_5x8, ALIGN_LEFT);//print num
					print_disp_mat();
					tim_delay_ms(menu_delaytime);
				}	else{}
				if(*buttons == entergomb)//accept the selced val
				{
					tim_delay_ms(menu_delaytime);
					*buttons = 0;
					break;
				}	else{}
			}

		}	else{}
		if(*buttons == exitgomb)//exit from num picker
		{
			*buttons = 0;
			break;
		}
		else{}
	}

	free(digitXPosT);

	//assemble the retun value
	return assembleU32Num(numt);
}

/*
 * numtf must be 10 byte size uint8_t block
 */
uint32_t assembleU32Num(uint8_t* numtf)
{
	return (uint32_t)( (uint32_t)numtf[0]+(numtf[1]*10)+(numtf[2]*100)+(numtf[3]*1000)+(numtf[4]*10000)+(numtf[5]*10000*10)+(numtf[6]*10000*100)+(numtf[7]*10000*1000)+(numtf[8]*100000*10000)+(numtf[9]*10000*10000*10) );
}

void disassembleU32Num(uint8_t* numtf, uint32_t val)
{
	//számjegylefejtés
	numtf[9]=((val/(10000*10000*10))%10);
	numtf[8]=((val/(10000*10000))%10);
	numtf[7]=((val/(10000*1000))%10);
	numtf[6]=((val/(10000*100))%10);
	numtf[5]=((val/(10000*10))%10);
	numtf[4]=((val/10000)%10);
	numtf[3]=((val/1000)%10);
	numtf[2]=((val/100)%10);
	numtf[1]=((val/10)%10);
	numtf[0]=((val/1)%10);
}

