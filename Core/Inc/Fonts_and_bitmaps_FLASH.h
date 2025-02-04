/*
 * Fonts_and_bitmaps_FLASH.h.h
 *
 * Created: 2019. 06. 14. 19:54:40
 * Author : Póti Szabolcs
 */

#ifndef _Fonts_and_bitmaps_FLASH_h
#define _Fonts_and_bitmaps_FLASH_h

#include <stdint.h>


#ifdef _AVR_IO_H_
#include <avr/pgmspace.h>
#define constant	const PROGMEM
#else
#define constant	const
#endif

#endif //_Fonts_and_bitmaps_FLASH_h
