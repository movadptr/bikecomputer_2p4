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
#define WRSR  	0x01	//Write Status Register
#define WRITE  	0x02	//Write to Memory Array
#define READ  	0x03	//Read from Memory Array
#define WRDI  	0x04	//Write Disable
#define RDSR  	0x05	//Read Status Register
#define WREN 	0x06	//Write Enable

//status register bits
#define WIP		0
#define WEL		1
#define BP0		2
#define BP1		3

#define MEMSIZE	128U	//memory size in bytes

uint8_t Read_M95010_W_EEPROM(uint8_t addr);
void Write_M95010_W_EEPROM(uint8_t addr, uint8_t data);
uint8_t Read_SREG_M95010_W_EEPROM(void);
void Write_SREG_M95010_W_EEPROM(uint8_t data);

#endif /* _M95010_W_EEPROM_H */


