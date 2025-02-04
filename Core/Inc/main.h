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
}bkdata;

typedef struct
{
	volatile uint8_t cptnum;	//number of the captured edge
	volatile uint8_t ovfbtwcpt;	//number of timer overflows between 2 captures
	volatile uint16_t c1;		//first captured data
	volatile uint16_t c2;		//second captured data
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
#define	swvstr	"swBK2.4V1.0."
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
#define ADD_GAMES	(ADD_Conway_s_game_of_life || ADD_Minesweepergame || ADD_tetrisgame || ADD_trexgame)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
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
#define BTN_JOBB_Pin LL_GPIO_PIN_12
#define BTN_JOBB_GPIO_Port GPIOB
#define BTN_JOBB_EXTI_IRQn EXTI15_10_IRQn
#define BTN_ENTER_SYS_WKUP2_Pin LL_GPIO_PIN_13
#define BTN_ENTER_SYS_WKUP2_GPIO_Port GPIOB
#define BTN_ENTER_SYS_WKUP2_EXTI_IRQn EXTI15_10_IRQn
#define BTN_EXIT_Pin LL_GPIO_PIN_8
#define BTN_EXIT_GPIO_Port GPIOA
#define BTN_EXIT_EXTI_IRQn EXTI9_5_IRQn
#define BTN_BAL_Pin LL_GPIO_PIN_9
#define BTN_BAL_GPIO_Port GPIOA
#define BTN_BAL_EXTI_IRQn EXTI9_5_IRQn
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

#define menu_delaytime 150U
#define menu_delaytime_fast 110U

#define tyre_700x23C				2.09858F	//kerék kerülete méterben
#define tyre_id_700x23C				0x01U

#define tyre_700x25C				2.11115F	//kerék kerülete méterben
#define tyre_id_700x25C				0x02U

#define tyre_id_custom_perimeter	0x05U

#define timtick			30.5e-6F	// 24Mhz/előosztó

#define temperature_calib_val	0.0F//-1.0F


///////EEPROM addresses/////////////////////////////////
#define EE_PWM_duty_backlight		1U
#define EE_bitek					2U
#define EE_curr_tyre_id				3U
#define EE_totdist_0				4U//uint32_t
#define EE_totdist_1				5U//
#define EE_totdist_2				6U//
#define EE_totdist_3				7U//

#define EE_offsetXL					8U//int16_t
#define EE_offsetXH					9U//
#define EE_offsetYL					10U
#define EE_offsetYH					11U
#define EE_offsetZL					12U
#define EE_offsetZH					13U

#define EE_custom_tyre_perimeter_H	14U
#define EE_custom_tyre_perimeter_L	15U

#define EE_RTC_smcalL				16U//int16_t
#define EE_RTC_smcalH				17U//

#define EE_contrast					18U
////////////////////////////////////////////////////////

//bit masks for saved bits
#define backlight_EN				0x01U
#define LCD_inverted				0x02U

///////ADC stuff////////////////////////////////////////
#define batt_input_volt_div_ratio	1.32794f
#define max_batt					4.2f
#define	min_batt					3.69f
#define used_ADC_channels			4U
////////////////////////////////////////////////////////

///////bit masks for system_bits////////////////////////
#define moving_time_recording_EN_1		0x01U
#define moving_time_recording_EN_2		0x02U
#define flashlight_EN					0x04U
#define flashlight_blink				0x08U
////////////////////////////////////////////////////////

#define flashlight_toggle_trig_val		250U

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
