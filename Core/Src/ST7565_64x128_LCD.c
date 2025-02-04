/*
 * ST7565_64x128_LCD.c
 *
 * Created: 2019. 05. 04. 11:23:20
 * Author : Póti Szabolcs
 */

#ifndef _ST7565_64x128_LCD_c
#define _ST7565_64x128_LCD_c

#include <stdint.h>
#include "main.h"
#include "ST7565_64x128_LCD.h"
#include "M95010_W_EEPROM.h"

extern SPI_HandleTypeDef hspi1;
extern uint8_t disp_mat[pixels_y][pixels_x/8];
extern volatile uint8_t bitek;
extern uint8_t saved_bits;

void print_disp_mat(void)
{
	__disable_irq();
	uint8_t i=0, j=0;
	for(i=0; i < (pixels_x/8); i++)
	{
		go_to_col_page(4,i);
		LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
		LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
		for(j=0; j<pixels_y; j++)
		{
			HAL_SPI_Transmit(&hspi1, &disp_mat[j][i], 1, 1000);
		}
		LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
		LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
	}
	__enable_irq();
}

void delete_RAM(void)
{
	__disable_irq();
	uint8_t i=0, j=0;
	for(i=0; i < (pixels_x/8); i++)
	{
		go_to_col_page(4,i);
		LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
		LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
		for(j=0; j<pixels_y; j++)
		{
			HAL_SPI_Transmit(&hspi1, 0x00, 1, 1000);
		}
		LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
		LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
	}
	__enable_irq();
}

void go_to_col_page(uint8_t col, uint8_t page)
{
	if(page < (pixels_x/8))
	{
		LCD_send_cmd(CMD_set_page_addr + page);
	}	else{}
	if(col < pixels_y)
	{
		LCD_send_cmd(CMD_set_col_addr_upper4bits + (col&0xf0) );
		LCD_send_cmd(CMD_set_col_addr_lower4bits + (col&0x0f) );
	}	else{}
}

void LCD_sleep(void)
{	//When the display OFF command is executed when in the display all points ON mode, power saver mode is entered.
	__disable_irq();
	LCD_send_cmd(CMD_static_indicator_off);
	LCD_send_cmd(CMD_set_disp_off);
	LCD_send_cmd(CMD_display_all_points_on);
	if(saved_bits & backlight_EN)
	{
		LL_TIM_CC_DisableChannel(TIM15, LL_TIM_CHANNEL_CH1);
	}else{}
	LL_GPIO_ResetOutputPin(LCD_RES_GPIO_Port, LCD_RES_Pin);
	__enable_irq();
}

void LCD_init(uint8_t contrast)
{
	__disable_irq();
	LL_GPIO_ResetOutputPin(LCD_RES_GPIO_Port, LCD_RES_Pin);
	LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
	LL_mDelay(10);
	LL_GPIO_SetOutputPin(LCD_RES_GPIO_Port, LCD_RES_Pin);
	LL_mDelay(10);
	LCD_send_cmd(CMD_display_all_points_off);				//waking sequence from sleep mode
	LCD_send_cmd(CMD_static_indicator_on);					//
	LCD_send_cmd(CMD_static_indicator_regdata_on_const);	//
	LCD_send_cmd(CMD_set_disp_on);
	if(saved_bits & LCD_inverted)
	{
		LCD_send_cmd(CMD_show_reverse_image);
	}
	else
	{
		LCD_send_cmd(CMD_show_normal_image);
	}
	LCD_send_cmd(CMD_set_bias_1_9);
	LCD_send_cmd(CMD_ADC_select_reverse);//adc//oszlop sorrend
	LCD_send_cmd(CMD_com_output_scan_dir_normal);//common output sel
	LCD_send_cmd(CMD_power_control_set+0x01);//nem egyszerre kapcsoljuk be a táp különböző részeit
	#ifdef _AVR_IO_H_
	_delay_ms(2);
	#else
	LL_mDelay(2);
	#endif
	LCD_send_cmd(CMD_power_control_set+0x03);
	#ifdef _AVR_IO_H_
	_delay_ms(2);
	#else
	LL_mDelay(2);
	#endif
	LCD_send_cmd(CMD_power_control_set+0x07);//power control				//+3 bit val	//7
	LCD_send_cmd(CMD_voltage_regulator_int_resistor_ratio_set+7);//rb/ra	//+3 bit val	//7
	LCD_send_cmd(CMD_electronic_volume_mode_set);//contrast
	LCD_send_cmd(contrast & 0x3f);											//6 bit val		//3f
	LCD_send_cmd(CMD_set_booster_ratio);
	LCD_send_cmd(0x3);														//3 bit val		//0
	LCD_send_cmd(CMD_set_start_line+0);
	delete_RAM();

	LL_TIM_OC_SetCompareCH1(TIM15, Read_M95010_W_EEPROM(EE_PWM_duty_backlight) );
	if( saved_bits & backlight_EN )	{ LL_TIM_EnableCounter(TIM15); LL_TIM_CC_EnableChannel(TIM15, LL_TIM_CHANNEL_CH1);}	else{}
	__enable_irq();
}

void LCD_send_data(uint8_t data)
{
	LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
	LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
	HAL_SPI_Transmit(&hspi1, &data, 1, 1000);
	LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
	LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

void LCD_send_cmd(uint8_t cmd)
{
	LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
	HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
	LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
}

#endif //_ST7565_64x128_LCD_c
