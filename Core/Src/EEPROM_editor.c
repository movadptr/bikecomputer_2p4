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

	//read EEPROM data into RAM
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
		delete_disp_mat();
		printEditorContent(memBuf, displayedMemSectionBeg);
		printCursor(displayedMemSectionBeg, memIndex);
		print_disp_mat();

		while(btn==0);
		tim_delay_ms(menu_delaytime);

		switch(btn)
		{
			//move right
			case jobbgomb:	if(memIndex < (MEMSIZE-1))
							{
								memIndex++;
							}
							if(((displayedMemSectionBeg+DISPLAYED_MEM_SECTION_SIZE)-1) < memIndex)
							{
								displayedMemSectionBeg += 2;//because in one row there is 2 byte
							}

							break;
			//move left
			case balgomb:	if(memIndex > 0)
							{
								memIndex--;
							}
							if((displayedMemSectionBeg) > memIndex)
							{
								displayedMemSectionBeg -= 2;
							}
							break;
			//move up
			case exitgomb:	if(memIndex > 1)
							{
								memIndex -= 2;
							}
							if((displayedMemSectionBeg) > memIndex)
							{
								displayedMemSectionBeg -= 2;
							}
							break;
			//move down
			case entergomb:	if(memIndex < (MEMSIZE-2))
							{
								memIndex += 2;
							}
							if(((displayedMemSectionBeg+DISPLAYED_MEM_SECTION_SIZE)-1) < memIndex)
							{
								displayedMemSectionBeg += 2;
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
	tim_delay_ms(menu_delaytime);

	while(1)
	{
		if( (btn == jobbgomb) && (buff[indx] < 0xff) )//increase value
		{
			buff[indx]++;
			printByte(buff, startindx, indx);
			printCursor(startindx, indx);
			print_disp_mat();
			tim_delay_ms(menu_delaytime);
		}	else{}
		if((btn == balgomb) && (buff[indx] > 0x00) )//decrease value
		{
			buff[indx]--;
			printByte(buff, startindx, indx);
			printCursor(startindx, indx);
			print_disp_mat();
			tim_delay_ms(menu_delaytime);
		}	else{}
		if(btn == entergomb)	{ break;}	else{} //accept value
	}
}

void printByte(uint8_t* buff, uint8_t startindx, uint8_t indx)
{
	uint8_t pos = indx-startindx;
	if(pos % 2 == 0)//even indices go to the left side
	{
		//write byte
		fill_rectangle_xy_height_width(16, 102-((pos/2)*10), 7, 11, Pixel_off);
		write_hex_byte_V(16, 102-((pos/2)*10), buff[indx], Pixel_on, size_5x8, ALIGN_LEFT);
		//ascii representation
		fill_rectangle_xy_height_width(51, 102-((pos/2)*10), 8, 5, Pixel_off);
		write_character_V(51, 102-((pos/2)*10), ((buff[indx]>='!')&&(buff[indx]<='z')) ? buff[indx] : '.', Pixel_on, size_5x8);

	}
	else//odd indeces go to thre right side
	{
		//write byte
		fill_rectangle_xy_height_width(33, 102-((pos/2)*10), 7, 11, Pixel_off);
		write_hex_byte_V(33, 102-((pos/2)*10), buff[indx], Pixel_on, size_5x8, ALIGN_LEFT);
		//ascii representation
		fill_rectangle_xy_height_width(58, 102-((pos/2)*10), 8, 5, Pixel_off);
		write_character_V(58, 102-((pos/2)*10), ((buff[indx]>='!')&&(buff[indx]<='z')) ? buff[indx] : '.', Pixel_on, size_5x8);
	}
}

void printCursor(uint8_t startindx, uint8_t indx)
{
	uint8_t ix = indx-startindx;
	if(ix % 2 == 0)//even indices go to the left side
	{
		draw_rectangle_xy_height_width(14, 100-((ix/2)*10), 11, 15, Pixel_on);
	}
	else//odd indeces go to thre right side
	{
		draw_rectangle_xy_height_width(31, 100-((ix/2)*10), 11, 15, Pixel_on);
	}
}

void printEditorContent(uint8_t* buff, uint8_t startindx)
{
	for(uint8_t i=0, row=0; i<(DISPLAYED_MEM_SECTION_SIZE-1); )
	{
		//print memory address
		write_hex_byte_V(0, 101-(row*10), startindx+i, Pixel_on, size_5x8, ALIGN_LEFT);

		//print 2 byte data
		write_hex_byte_V(16, 102-(row*10), buff[startindx+i], Pixel_on, size_5x8, ALIGN_LEFT);
		write_hex_byte_V(33, 102-(row*10), buff[startindx+i+1], Pixel_on, size_5x8, ALIGN_LEFT);

		//print ascii representation
		write_character_V(51, 102-(row*10), ((buff[startindx+i]>='!')&&(buff[startindx+i]<='z')) ? buff[startindx+i] : '.', Pixel_on, size_5x8);
		write_character_V(58, 102-(row*10), ((buff[startindx+i+1]>='!')&&(buff[startindx+i+1]<='z')) ? buff[startindx+i+1] : '.', Pixel_on, size_5x8);
		row++;
		i+=2;
	}

	//draw separation lines
	draw_line_y(0, 127, 12, Pixel_on);
	draw_line_y(0, 127, 47, Pixel_on);
}

#endif //_EEPROM_EDITOR_c
