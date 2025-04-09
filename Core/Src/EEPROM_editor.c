/*
 * EEPROM_editor.c
 *
 * Created: 2019. 05. 04. 11:23:20
 * Author : Póti Szabolcs
 */

#ifndef _EEPROM_EDITOR_c
#define _EEPROM_EDITOR_c

#include <stdint.h>
#include "main.h"
#include "M95010_W_EEPROM.h"
#include "disp_fgv.h"


#define DISPLAYED_MEM_SECTION_SIZE 20U
extern volatile uint8_t btn;

void EEPROM_editor(void);
void printCursor(uint8_t indx);
void printEditorContent(uint8_t* buff, uint8_t startindx);


void EEPROM_editor(void)
{
	uint8_t memBuf[MEMSIZE] = {0};

	//read EEPROM data into RAM16
	for(uint8_t indx=0; indx<MEMSIZE; indx++)
	{
		memBuf[indx] = Read_M95010_W_EEPROM(indx);
	}

	delete_disp_mat();
	uint8_t displayedMemSectionBeg = 0;
	uint8_t cursor_pos = 0;
	btn=0;

	while(1)
	{
		printEditorContent(memBuf, displayedMemSectionBeg);
		printCursor(cursor_pos);

		while(btn==0);
		tim_delay_ms(menu_delaytime);

		switch(btn)
		{
			//move right
			case jobbgomb:
							break;
			//move left
			case balgomb:
							break;
			//move up
			case entergomb:
							break;
			//move down
			case exitgomb:
							break;
			//modify
			case (entergomb+exitgomb):
										break;
			//exit editor and save changes
			case (balgomb+exitgomb):
										break;
		}


	}


	//write RAM data into EEPROM
	for(uint8_t indx=0; indx<MEMSIZE; indx++)
	{
		memBuf[indx] = Read_M95010_W_EEPROM(indx);
	}
}

/*
 * @param indx is 0-19, where 0 represents the left upper byte on the screen, 19 represents the right lower byte of the screen.
 */
void printCursor(uint8_t indx)
{
	if(indx % 2 == 0)//páros indexeknél bal oldalra rakjuk
	{
		draw_rectangle_xy_height_width(16, 100-((indx/2)*10), 10, 15, Pixel_on);
	}
	else//páratlan indexnél jobb oldalra
	{
		draw_rectangle_xy_height_width(33, 100-((indx/2)*10), 10, 15, Pixel_on);
	}
}

void printEditorContent(uint8_t* buff, uint8_t startindx)
{
	for(uint8_t i=0; i<DISPLAYED_MEM_SECTION_SIZE; )//print all 10 row
	{
		//print memory address
		write_hex_byte_V(0, 100-(i*10), i, Pixel_on, size_5x8, ALIGN_LEFT);

		//print 2 byte data
		write_hex_byte_V(16, 100-(i*10), buff[startindx+i], Pixel_on, size_5x8, ALIGN_LEFT);
		write_hex_byte_V(33, 100-(i*10), buff[startindx+i+1], Pixel_on, size_5x8, ALIGN_LEFT);

		//print ascii representation
		write_character_V(51, 100-(i*10), ((buff[startindx+i]>='!')&&(buff[startindx+i]<='z')) ? buff[startindx+i] : '.', Pixel_on, size_5x8);
		write_character_V(51, 100-(i*10), ((buff[startindx+i+1]>='!')&&(buff[startindx+i+1]<='z')) ? buff[startindx+i+1] : '.', Pixel_on, size_5x8);

		i+=2;
	}

	//draw separation lines
	draw_line_x(0, 127, 12, Pixel_on);
	draw_line_x(0, 127, 47, Pixel_on);
}

#endif //_EEPROM_EDITOR_c
