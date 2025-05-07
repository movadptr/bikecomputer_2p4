/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_crs.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_rng.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_tim.h"
#include "stm32l4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct
{
	uint8_t hours;
	uint8_t mins;
	uint8_t secs;
	float avg_speed;//km/h
	float max_speed;//
	float dist;//km
}lap;

typedef struct
{
	uint32_t elapsed_time;//sec
	float maxspeed;//km/h
	uint32_t dist;//kerék körülfordulásainak száma a kör kezdetének elején
	uint8_t laps;
}lap_tmp;

typedef struct
{									//mértékegység
	volatile uint32_t dist;			//kerék körülfordulásainak száma
	volatile uint32_t totdist;		//
	volatile uint32_t elapsed_time; //sec
	volatile uint32_t moving_time;	//sec
	volatile float speed;			//km/h
	volatile float maxspeed;		//
	volatile float cadence; 		//rpm
	volatile float grad;
	volatile uint16_t rawbatt;		//nincs mért.egys, csak egy dimenzió nélküli szám az adc-ből
	volatile float batt;			//Volt
	volatile uint16_t rawlight;		//nincs mért.egys, csak egy dimenzió nélküli szám az adc-ből
	volatile uint16_t rawvrefint;	//nincs mért.egys, csak egy dimenzió nélküli szám az adc-ből
	volatile float acc_tempsensor;		//°C
	volatile uint16_t uc_tempsensor_rawtemp;//nincs mért.egys, csak egy dimenzió nélküli szám az adc-ből
}bkdata;

typedef struct
{
	volatile uint8_t cptnum;	//number of the captured edge
	volatile uint8_t ovfbtwcpt;	//number of timer overflows between 2 captures
	volatile uint32_t c1;		//first captured data
	volatile uint32_t c2;		//second captured data
	volatile uint8_t datastate;//tells if the data in the c1 and c2 are valid or not: if first bit is 1 then c1 is valid, if second bit is 1 then c2 is valid
}capturedata;
#define max_ovf_num	1//max overflows before resetting the capture variables, (this means stopping meas and setting velocity zero)
#define c1Valid		0x01
#define c2Valid		0x02
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define STM32_bikecomputer_2
#define	swvstr	"BK2.4V1.0."
#ifdef DEBUG
#define swtyp "D"
#else
#define swtyp "R"
#endif
#define SW_ver (swvstr swtyp)
#define build_date	__DATE__

#define ADD_Conway_s_game_of_life	1
#define ADD_Minesweepergame			1
#define ADD_tetrisgame				1
#define ADD_trexgame				0
#define ADD_GAMES	(ADD_Conway_s_game_of_life + ADD_Minesweepergame + ADD_tetrisgame + ADD_trexgame)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void setPinToGenericOutput(GPIO_TypeDef *GPIOx, uint32_t Pin);
void SetSmoothCalib(int16_t calv);
uint32_t get_dist_for_new_tyre(float old_tyre, float new_tyre, uint32_t current_dist_val);
void tim_delay_ms(uint16_t d);
void settings(void);
void get_rtc_data(void);
void write_main_page_data(void);
void write_rtc_data(void);
float get_avg_speed(uint32_t distf,uint32_t secf);
void get_elapsed_time(uint32_t et_sec, uint8_t* hf, uint8_t* mf, uint8_t* sf);
void write_elapsed_time(void);
void calculate_batt_voltage(uint16_t baterry_ADC_data, uint16_t vrefint_ADC_data);
void draw_battery_band(float volt_val);
void _err_hnd_st_msg(char* str1, char* str2);
void pwr_down(void);
void copy_lap(lap* l_dest, lap* l_source);
void Calibrate_ADC(void);
void write_secondary_page_data(void);
float calcSTM32temp(uint16_t rawtemp);
float getSlope(float angle);

void Conway_s_game_of_life();
int Minesweepergame();
int tetrisgame();
int trexgame();
void Tgame_button_isr();
void Tgame_main_isr();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_CS_Pin LL_GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOA
#define BATT_SENSE_Pin LL_GPIO_PIN_1
#define BATT_SENSE_GPIO_Port GPIOA
#define BACKLIGHT_PWM_Pin LL_GPIO_PIN_2
#define BACKLIGHT_PWM_GPIO_Port GPIOA
#define LCD_RES_Pin LL_GPIO_PIN_3
#define LCD_RES_GPIO_Port GPIOA
#define LCD_DC_Pin LL_GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOA
#define ACC_CS_Pin LL_GPIO_PIN_0
#define ACC_CS_GPIO_Port GPIOB
#define LIGHT_SENSE_Pin LL_GPIO_PIN_1
#define LIGHT_SENSE_GPIO_Port GPIOB
#define ACC_INT_Pin LL_GPIO_PIN_2
#define ACC_INT_GPIO_Port GPIOB
#define EEPROM_CS_Pin LL_GPIO_PIN_10
#define EEPROM_CS_GPIO_Port GPIOB
#define FLASHLIGHT_Pin LL_GPIO_PIN_11
#define FLASHLIGHT_GPIO_Port GPIOB
#define BTN_EXIT_Pin LL_GPIO_PIN_12
#define BTN_EXIT_GPIO_Port GPIOB
#define BTN_EXIT_EXTI_IRQn EXTI15_10_IRQn
#define BTN_BAL_SYS_WKUP2_Pin LL_GPIO_PIN_13
#define BTN_BAL_SYS_WKUP2_GPIO_Port GPIOB
#define BTN_BAL_SYS_WKUP2_EXTI_IRQn EXTI15_10_IRQn
#define BTN_JOBB_Pin LL_GPIO_PIN_8
#define BTN_JOBB_GPIO_Port GPIOA
#define BTN_JOBB_EXTI_IRQn EXTI9_5_IRQn
#define BTN_ENTER_Pin LL_GPIO_PIN_9
#define BTN_ENTER_GPIO_Port GPIOA
#define BTN_ENTER_EXTI_IRQn EXTI9_5_IRQn
#define SPEED_SW_Pin LL_GPIO_PIN_15
#define SPEED_SW_GPIO_Port GPIOA
#define CADENCE_SW_Pin LL_GPIO_PIN_3
#define CADENCE_SW_GPIO_Port GPIOB
#define D_LED_Pin LL_GPIO_PIN_8
#define D_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define balgomb		0x08U
#define exitgomb 	0x04U
#define entergomb	0x02U
#define jobbgomb	0x01U

#define menu_delaytime 200U
#define menu_delaytime_fast 120U

#define tyre_700x23C				2.09858F	//kerék kerülete méterben
#define tyre_id_700x23C				0x01U

#define tyre_700x25C				2.11115F	//kerék kerülete méterben
#define tyre_id_700x25C				0x02U

#define tyre_id_custom_perimeter	0x05U

#define timtick			25e-9F	// 24Mhz/előosztó
#define timAR			80000000UL//a timer counter hossza
#define temperature_calib_val	0.0F//-1.0F
#define TIM15_period		5e-3F//5ms timer 15 period

///////EEPROM addresses/////////////////////////////////
#define EE_PWM_duty_backlight		0x0001
#define EE_bitek					0x0002
#define EE_curr_tyre_id				0x0003
#define EE_totdist_0				0x0004//uint32_t
#define EE_totdist_1				0x0005//
#define EE_totdist_2				0x0006//
#define EE_totdist_3				0x0007//
/* OFFSETS ARE MANAGED BY ST DRIVER
#define EE_offsetXL					0x0008//int16_t
#define EE_offsetXH					0x0009//
#define EE_offsetYL					0x000A
#define EE_offsetYH					0x000B
#define EE_offsetZL					0x000C
#define EE_offsetZH					0x000D
*/
#define EE_custom_tyre_perimeter_H	0x000E
#define EE_custom_tyre_perimeter_L	0x000F

#define EE_RTC_smcalL				0x0011//int16_t
#define EE_RTC_smcalH				0x0012//

#define EE_contrast					0x0013
#define EE_flashlight_blink			0x0014
////////////////////////////////////////////////////////

//bit masks for saved bits
#define backlight_EN				0x01U
#define LCD_inverted				0x02U

///////ADC stuff////////////////////////////////////////
#define batt_input_volt_div_ratio	1.303586319f// (R2+R1)/R2
#define used_ADC_channels			4U
#define max_batt					4.2f
#define	min_batt					3.69f
////////////////////////////////////////////////////////

///////bit masks for system_bits////////////////////////
#define moving_time_recording_EN_1		0x01U
#define moving_time_recording_EN_2		0x02U
#define flashlight_EN					0x04U
#define flashlight_blink				0x08U
////////////////////////////////////////////////////////

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
