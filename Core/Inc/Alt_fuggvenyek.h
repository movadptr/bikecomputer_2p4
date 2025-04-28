//
//	Alt_fuggvenyek.h
//
// 2019.01.06  20:31
// Author: Póti Szabolcs
//

#ifndef Alt_fuggvenyek_H
#define Alt_fuggvenyek_H
#include "stdint.h"

int32_t mypow16(int32_t exponent);

int32_t mypow10(int32_t exponent);

uint8_t reverse_byte(uint8_t byte);

#if defined (USE_FULL_LL_DRIVER)	//STM32 mikrokontrollerhez LL driverrel
uint32_t get_random_num(void);
#endif

void delaynop(uint8_t del);

/*************************************************************************************/
/* //https://en.wikipedia.org/wiki/Linear-feedback_shift_register 					 */
/* //https://www.eetimes.com/tutorial-linear-feedback-shift-registers-lfsrs-part-1/  */
/*************************************************************************************/
uint16_t LFSR_random(uint16_t prev_state);

/*************************************************************************************/
/* returns with the number of bytes of the string (does not counts terminating zero) */
/*************************************************************************************/
uint16_t stringsize(char *string);

uint8_t bcd_to_dec(uint8_t szam);

uint8_t dec_to_bcd(uint8_t szam);


#endif // ifndef Alt_fuggvenyek_H //
