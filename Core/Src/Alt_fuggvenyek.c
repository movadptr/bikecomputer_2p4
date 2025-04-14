//
//	Alt_fuggvenyek.c
//
// 2019.01.06  20:31
// Author: Póti Szabolcs
//

#ifndef Alt_fuggvenyek_C
#define Alt_fuggvenyek_C

#include <stdint.h>
#include "main.h"

int32_t mypow10(int32_t exponent)
{
	uint32_t num = 10;

	switch(exponent)
	{
		case 0: num = 1;
				break;

		case 1: //num = 10;
				break;

		default:	exponent--;
					while(exponent)
					{
						num *= 10;
						exponent--;
					}
					break;
	}
	return num;
}

uint8_t reverse_byte(uint8_t byte)
{
	byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
	byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
	byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
	return byte;
}

#if defined (USE_FULL_LL_DRIVER)	//STM32 mikrokontrollerhez LL driverrel
uint32_t get_random_num(void)
{
	if((LL_RNG_IsActiveFlag_CEIS(RNG)==0) && (LL_RNG_IsActiveFlag_CEIS(RNG)==0))//ha nincs hiba
	{
		while( ! LL_RNG_IsActiveFlag_DRDY(RNG) )
		{
			__NOP();
		}
	}
	return LL_RNG_ReadRandData32(RNG);
}
#endif

void delaynop(uint8_t del)
{
	while(del)
	{
		asm("nop");
		del--;
	}
}

uint16_t LFSR_random(uint16_t prev_state)
{
    return  (((prev_state<<1)&0xfffe) | (((((prev_state>>15)&0x01)^((prev_state>>13)&0x01))^((prev_state>>12)&0x01))^((prev_state>>10)&0x01)));
}

uint16_t stringsize(char *string)
{
	uint16_t i=0;
	
	while(string[i])
	{
		i++;
	}
	return i;
}

uint8_t dec_to_bcd(uint8_t szam)
{
	return	((szam/10)<<4)|(szam%10);
}

uint8_t bcd_to_dec(uint8_t szam)
{
	return (10*((szam&0xf0)>>4))+(szam&0x0f);
}

#endif // ifndef Alt_fuggvenyek_C //
