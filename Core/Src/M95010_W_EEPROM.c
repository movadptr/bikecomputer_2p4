/*
 *  M95010_W_EEPROM.c
 *
 *  Created on: 2021. 12. 18.
 *      Author: Póti Szabolcs
 */

#ifndef _M95010_W_EEPROM_C
#define _M95010_W_EEPROM_C

#include "M95010_W_EEPROM.h"

extern SPI_HandleTypeDef hspi1;

uint8_t Read_SREG_M95010_W_EEPROM(void)
{
	__disable_irq();
	uint8_t data=0;
	uint8_t tmp=RDSR;
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	HAL_SPI_Receive(&hspi1, &data, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	__enable_irq();
	return data;
}

void Write_SREG_M95010_W_EEPROM(uint8_t data)
{
	__disable_irq();
	uint8_t tmp=WREN;
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	tmp=WRSR;
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	tmp=WRDI;
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	__enable_irq();
}

uint8_t Read_M95010_W_EEPROM(uint16_t addr)
{
	__disable_irq();
	uint8_t data=0;
	uint8_t tmp=READ;
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	tmp=((addr & 0xff00)>>8);
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	tmp=(addr & 0x00ff);
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	HAL_SPI_Receive(&hspi1, &data, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	__enable_irq();
	return data;
}

void Write_M95010_W_EEPROM(uint16_t addr, uint8_t data)
{
	__disable_irq();
	while( Read_SREG_M95010_W_EEPROM() & (1<<WIP) )//itt ciklik amíg nincs kész az előző írással
	{ LL_mDelay(2);}
	uint8_t tmp=WREN;
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	tmp=WRITE;
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	tmp=((addr & 0xff00)>>8);
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	tmp=(addr & 0x00ff);
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	tmp=WRDI;
	HAL_SPI_Transmit(&hspi1, &tmp, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	__enable_irq();
}


#endif // _M95010_W_EEPROM_C
