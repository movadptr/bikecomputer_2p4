/*
 *  M95010_W_EEPROM.h
 *
 *  Created on: 2021. 12. 18.
 *      Author: Póti Szabolcs
 */

/*

  	/CS   ___+-----+___  VCC
  	SO    ___|°    |___  /HOLD
  	/WE   ___|     |___  SCK
 	VSS   ___|     |___  SI
 	  	     +-----+
*/
#ifndef _M95010_W_EEPROM_H
#define _M95010_W_EEPROM_H

#include "main.h"

//commands
#define WREN 	0b00000110	//Write Enable
#define WRDI  	0b00000100	//Write Disable
#define RDSR  	0b00000101	//Read Status Register
#define WRSR  	0b00000001	//Write Status Register
#define READ  	0b00000011	//Read from Memory Array
#define WRITE  	0b00000010	//Write to Memory Array

//status register bits
#define WIP		0
#define WEL		1
#define BP0		2
#define BP1		3

uint8_t Read_M95010_W_EEPROM(uint16_t addr);
void Write_M95010_W_EEPROM(uint16_t addr, uint8_t data);
uint8_t Read_SREG_M95010_W_EEPROM(void);
void Write_SREG_M95010_W_EEPROM(uint8_t data);

#endif /* _M95010_W_EEPROM_H */


