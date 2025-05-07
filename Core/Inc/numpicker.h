/*
 * numpicker.h
 *
 *  Created on: Apr 11, 2025
 *  Author: Póti Szabolcs
 */

#ifndef INC_NUMPICKER_H_
#define INC_NUMPICKER_H_

#include "main.h"

uint8_t assembleHexNum(uint8_t* numtf);
void disassembleHexNum(uint8_t* numtf, uint8_t val);
uint32_t assembleU32Num(uint8_t* numtf);
void disassembleU32Num(uint8_t* numtf, uint32_t val);
uint32_t numPickerUInt32_V(uint32_t Llimit, uint32_t Hlimit, uint32_t startval, volatile uint8_t* buttons);
uint32_t numPickerUInt32_printInPlace_V(uint32_t Llimit, uint32_t Hlimit, uint32_t startval, volatile uint8_t* buttons, uint8_t xpos, uint8_t ypos);
uint8_t numPickerHex_printInPlace_V(uint8_t Llimit, uint8_t Hlimit, uint8_t startval, volatile uint8_t* buttons, uint8_t xpos, uint8_t ypos);

#endif /* INC_NUMPICKER_H_ */
