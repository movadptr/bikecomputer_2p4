/*
 * numpicker.h
 *
 *  Created on: Apr 11, 2025
 *  Author: POS1BP
 */

#ifndef INC_NUMPICKER_H_
#define INC_NUMPICKER_H_

#include "main.h"

uint32_t assembleU32Num(uint8_t* numtf);
void disassembleU32Num(uint8_t* numtf, uint32_t val);
uint32_t numPickerUInt32_V(uint32_t Llimit, uint32_t Hlimit, uint32_t startval, uint8_t* buttons);

#endif /* INC_NUMPICKER_H_ */
