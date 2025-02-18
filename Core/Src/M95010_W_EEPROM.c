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
	uint8_t tx=RDSR;
	uint8_t rxt[2]={0};
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_TransmitReceive(&hspi1, &tx, rxt, 2, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	__enable_irq();
	return rxt[1];
}

void Write_SREG_M95010_W_EEPROM(uint8_t data)
{
	__disable_irq();
	uint8_t tx=WREN;
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, &tx, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	LL_mDelay(1);
	uint8_t txt[2]={WRSR, data};
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, txt, 2, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	LL_mDelay(1);
	tx=WRDI;
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, &tx, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	__enable_irq();
}

uint8_t Read_M95010_W_EEPROM(uint8_t addr)
{
	__disable_irq();
	while( (Read_SREG_M95010_W_EEPROM() & (1<<WIP)) != 0)//itt ciklik amíg nincs kész az előző írással
	{
		LL_mDelay(1);
	}

	uint8_t txt[2]={READ, addr};
	uint8_t rxt[3]={0};
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_TransmitReceive(&hspi1, txt, rxt, 3, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	__enable_irq();
	return rxt[2];
}

void Write_M95010_W_EEPROM(uint8_t addr, uint8_t data)
{
	__disable_irq();
	while( (Read_SREG_M95010_W_EEPROM() & (1<<WIP)) != 0)//itt ciklik amíg nincs kész az előző írással
	{
		LL_mDelay(1);
	}
	uint8_t tx=WREN;
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, &tx, 1, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	LL_mDelay(1);
	uint8_t txt[3]={WRITE, addr, data};
	LL_GPIO_ResetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	HAL_SPI_Transmit(&hspi1, txt, 3, 1000);
	LL_GPIO_SetOutputPin(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);
	LL_mDelay(1);

	__enable_irq();
}


#endif // _M95010_W_EEPROM_C
