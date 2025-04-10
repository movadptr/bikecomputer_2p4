/*
 * EEPROM_editor.c
 *
 * Created: 2025. 04. 09.
 * Author : Póti Szabolcs
 */

#ifndef _EEPROM_EDITOR_c
#define _EEPROM_EDITOR_c

#include <stdint.h>
#include "main.h"
#include "M95010_W_EEPROM.h"
#include "disp_fgv.h"
#include "EEPROM_editor.h"

extern volatile uint8_t btn;

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
	uint8_t memIndex = 0;
	btn=0;

	while(1)
	{
		printEditorContent(memBuf, displayedMemSectionBeg);
		printCursor(displayedMemSectionBeg, memIndex);
		print_disp_mat();

		while(btn==0);
		tim_delay_ms(menu_delaytime);

		switch(btn)
		{
			//move right
			case jobbgomb:	if(memIndex < (MEMSIZE-2))
							{
								memIndex++;
							}
							if((displayedMemSectionBeg+DISPLAYED_MEM_SECTION_SIZE) < memIndex)
							{
								displayedMemSectionBeg += 2;//mert egy sorba 2 byte fér el
							}

							break;
			//move left
			case balgomb:	if(memIndex > 0)
							{
								memIndex--;
							}
							if((displayedMemSectionBeg) > memIndex)
							{
								displayedMemSectionBeg -= 2;//mert egy sorba 2 byte fér el
							}
							break;
			//move up
			case entergomb:	if(memIndex > 2)
							{
								memIndex -= 2;
							}
							if((displayedMemSectionBeg) > memIndex)
							{
								displayedMemSectionBeg -= 2;//mert egy sorba 2 byte fér el
							}
							break;
			//move down
			case exitgomb:	if(memIndex < (MEMSIZE-3))
							{
								memIndex += 2;
							}
							if((displayedMemSectionBeg) < memIndex)
							{
								displayedMemSectionBeg += 2;//mert egy sorba 2 byte fér el
							}
							break;
			//modify
			case (entergomb|exitgomb):	modifyValue(memBuf, memIndex, displayedMemSectionBeg);
										break;

			default:	break;
		}

		if(btn == (balgomb|exitgomb))//exit EEPROM editor
		{
			break;
		}
	}

	//write RAM data into EEPROM
	for(uint8_t indx=0; indx<MEMSIZE; indx++)
	{
		Write_M95010_W_EEPROM(indx, memBuf[indx]);
	}
}

void modifyValue(uint8_t* buff, uint8_t indx, uint8_t startindx)
{
	while(1)
	{
		if( (btn == jobbgomb) && (buff[indx] < 0xff) )//értéket növel
		{
			buff[indx]++;
			printByte(buff, startindx, indx);
			printCursor(startindx, indx);
			print_disp_mat();
			tim_delay_ms(menu_delaytime);
		}	else{}
		if((btn == balgomb) && (buff[indx] > 0x00) )//értéket csökkent
		{
			buff[indx]--;
			printByte(buff, startindx, indx);
			printCursor(startindx, indx);
			print_disp_mat();
			tim_delay_ms(menu_delaytime);
		}	else{}
		if(btn == entergomb)	{ break;}	else{} //értéket elfogad
	}
}

void printByte(uint8_t* buff, uint8_t startindx, uint8_t indx)
{
	uint8_t pos = indx-startindx;
	if(pos % 2 == 0)//páros indexeknél bal oldalra rakjuk
	{
		write_hex_byte_V(16, 100-((pos/2)*10), buff[indx], Pixel_on, size_5x8, ALIGN_LEFT);
	}
	else//páratlan indexnél jobb oldalra
	{

		write_hex_byte_V(33, 100-((pos/2)*10), buff[indx], Pixel_on, size_5x8, ALIGN_LEFT);
	}
}

void printCursor(uint8_t startindx, uint8_t indx)
{
	uint8_t ix = indx-startindx;
	if(ix % 2 == 0)//páros indexeknél bal oldalra rakjuk
	{
		draw_rectangle_xy_height_width(16, 100-((ix/2)*10), 10, 15, Pixel_on);
	}
	else//páratlan indexnél jobb oldalra
	{
		draw_rectangle_xy_height_width(33, 100-((ix/2)*10), 10, 15, Pixel_on);
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
