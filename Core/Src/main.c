/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
  *
  *  TIM1 - IMU
  *  TIM2 - speed, cadence
  *  TIM6 - nonblocking ms timer
  *  TIM15 - PWM backlight
  *  TIM16 - game
  *
  *
  *
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdlib.h>

#include "M95010_W_EEPROM.h"
#include "ST7565_64x128_LCD.h"
#include "disp_fgv.h"

#include "motion_di.h"
#include "app_mems.h"
#include "ism330dhcx.h"
#include "custom_motion_sensors.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
uint8_t tim_delay_ms_flag = 0;

volatile uint8_t btn=0;
volatile int8_t main_page=1;
#define main_page_db	4

LL_RTC_TimeTypeDef RTCtime={0};
LL_RTC_DateTypeDef RTCdate={0};

volatile uint8_t saved_bits=0;
volatile uint8_t system_bits=0;

volatile uint16_t adc_conv_results[used_ADC_channels] = {0};// 0-ch1-battery, 1-ch9-lightsensor, 2-ch17-vrefint //this is the destination of the DMA1

bkdata alldata={0};//főképernyő adatok
volatile float curr_tyre;//kerület m-ben
capturedata cpt1={0};
capturedata cpt2={0};

lap l1={0}, l2={0}, l3={0};
lap_tmp ltmp={0};

volatile uint8_t flashlight_blink_val = 0;

extern void *MotionCompObj[CUSTOM_MOTION_INSTANCES_NBR];
float Grotation[MDI_NUM_AXES];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

static void GPIO_Init(void);
static void RNG_Init(void);
static void MX_CRC_Init(void);
static void SPI1_Init(void);
static void ADC1_Init(void);
static void TIM1_Init(void);
static void TIM2_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM15_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */

static void init(void);
void ClockConfig(void);
static void DMA_Init(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	ClockConfig();
	GPIO_Init();
	RNG_Init();
	SPI1_Init();
	ADC1_Init();
	DMA_Init();
	TIM1_Init();
	TIM2_Init();
	MX_CRC_Init();
	MX_TIM6_Init();
	MX_TIM15_Init();
	MX_TIM16_Init();

	MX_MEMS_Init();

	init();

	btn=0;
	while(1)
	{
		delete_disp_mat();
		switch(main_page)//print all content
		{
			//főképernyő
			case 1: write_main_page_data();
				  break;
			//másodlagos képernyő: köridők, entergomb megnyomására kiszámolja a legutolsó megnyomás óta eltelt időt, megtett távolságot... és a legújabb kerül felülre, kiírja a körök számát
			case 2: write_secondary_page_data();
				  break;
			//menü
			case 3: write_text_V(9, 70, "Settings", Pixel_on, size_5x8);
				  break;
			//"kikapcsolás"
			case 4: write_text_V(9, 70, "PWR DOWN", Pixel_on, size_5x8);
				  break;
		}
		print_disp_mat();
		btn = 0;

		while(btn == 0x00)//amíg nem nyomok semmit itt ciklik
		{
			NVIC_DisableIRQ(SysTick_IRQn);
			HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
			NVIC_EnableIRQ(SysTick_IRQn);
		}

		if(btn == jobbgomb)
		{
			btn=0;
			main_page++;
			if(main_page > main_page_db)	{ main_page=1;} else{}
		}	else{}
		if(btn == balgomb )
		{
			btn=0;
			main_page--;
			if(main_page < 1)	{ main_page = main_page_db;} else{}
		}	else{}
		if(btn == exitgomb )
		{
			switch(main_page)
			{
				case 1: if((system_bits & moving_time_recording_EN_2) == 0) //főképrenyőnél exit gombot 1 másodpercig nyomvatartva reseteli az adatokat
						{
							for( uint8_t d = 0; ; d++)
							{
								if(d>=10)
								{
									alldata.dist=0;
									alldata.elapsed_time=0;
									alldata.maxspeed=0;
									alldata.moving_time=0;

									ltmp.elapsed_time=0;//azért van itt mert ha csak a főképernyő adatokat resetelném és hozzáadnék (legelőször) egy lapot(megnyomnám az entergombot a másodlagos képernyőn)
									ltmp.dist=0;		//akkor azokat az adatokat rakná bele amit a főképernyőnél már kitöröltem (persze más változókban vannak de ugyan azok az adatok lennének)
									ltmp.maxspeed=0;	//
								}else{}
								tim_delay_ms(100);
								if(btn != exitgomb)	{ break;} else{}
							}
						}	else{};
						break;

				case 2: for( uint8_t d = 0; ; d++)//reseteli a köröket ha 1 másodpercnél hosszabb ideig nyomom
						{
							if(d==10)
							{
								ltmp.laps=0;
								ltmp.elapsed_time=0;
								ltmp.dist=0;
								ltmp.maxspeed=0;
								l1.dist = 0;
								l1.max_speed = 0;
								l1.avg_speed = 0;
								l1.hours = 0;
								l1.mins = 0;
								l1.secs = 0;
								copy_lap(&l2, &l1);
								copy_lap(&l3, &l1);
								break;
							} else{}
							tim_delay_ms(100);
							if(btn != exitgomb)	{ break;} else{}
						}
						break;
			}

		}	else{}
		if(btn == entergomb )
		{
			switch(main_page)
			{
				case 1: //főképernyő
						if(system_bits & moving_time_recording_EN_1)	{ system_bits &= ~(moving_time_recording_EN_1 | moving_time_recording_EN_2);}// a főképernyőnél a középső gomb megnyomásával le lehet állí­tani, és el lehet indí­tani a mérést
						else
						{
							system_bits |= moving_time_recording_EN_1;
							if(alldata.speed > 0.5)
							{
								system_bits |= moving_time_recording_EN_2;
							}
							else
							{
								system_bits &= ~moving_time_recording_EN_2;
							}
						}
						break;
				case 2: //másodlagos képernyő// laps
						copy_lap(&l3, &l2);
						copy_lap(&l2, &l1);
						get_elapsed_time(ltmp.elapsed_time, &l1.hours, &l1.mins, &l1.secs);
						l1.dist = (ltmp.dist * curr_tyre)/1000;//km
						l1.max_speed = ltmp.maxspeed;
						l1.avg_speed = get_avg_speed(ltmp.dist, ltmp.elapsed_time);
						ltmp.elapsed_time=0;
						ltmp.dist=0;
						ltmp.maxspeed=0;
						ltmp.laps++;//számolja a megtett köröket
						break;
				case 3: //beállítások menü
						settings();
						break;
				case 4: for( uint8_t d = 0; ; d++)//csak akkor megy standby módba ha 1 másodpercnél tovább nyomom a gombot
						{
							if(d==10)
							{
								tim_delay_ms(150);//késleltetés hogy az utolsó kirajzolt csíkrészlet is látható legyen
								pwr_down();
							} else{}
							draw_line_x(1, 1+((d+1)*6), 60, Pixel_on);//futó csík
							tim_delay_ms(150);
							print_disp_mat();
							if(btn != entergomb)
							{
								draw_line_x(1, 1+((d+1)*6), 60, Pixel_off);//töröljük a csíkot
								break;
							} else{}
						}
						break;
			}
		  btn=0;
		} else{}
	}

	while(1)	{ __NOP();}
}


/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void TIM1_Init(void)
{
	//TIM1 interrupt Init
	NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);
	NVIC_SetPriority(TIM1_UP_TIM16_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
	NVIC_SetPriority(TIM1_CC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(TIM1_CC_IRQn);

	{
		uint32_t tmpcr1;

		tmpcr1 = LL_TIM_ReadReg(TIM1, CR1);
		MODIFY_REG(tmpcr1, (TIM_CR1_DIR | TIM_CR1_CMS), LL_TIM_COUNTERMODE_UP);//Select the Counter Mode
		MODIFY_REG(tmpcr1, TIM_CR1_CKD, LL_TIM_CLOCKDIVISION_DIV1);//Set the clock division
		LL_TIM_WriteReg(TIM1, CR1, tmpcr1);//Write to TIM1 CR1
		LL_TIM_SetAutoReload(TIM1, 20000);
		LL_TIM_SetPrescaler(TIM1, 20);
		LL_TIM_SetRepetitionCounter(TIM1, 0);
		LL_TIM_GenerateEvent_UPDATE(TIM1);//Generate an update event to reload the Prescaler and the repetition counter value (if applicable) immediately
	}

	LL_TIM_DisableARRPreload(TIM1);

	{
		uint32_t tmpccmr1;
		uint32_t tmpccer;
		uint32_t tmpcr2;

		CLEAR_BIT(TIM1->CCER, TIM_CCER_CC1E);//Disable the Channel 1: Reset the CC1E Bit
		tmpccer = LL_TIM_ReadReg(TIM1, CCER);//Get the TIM1 CCER register value
		tmpcr2 = LL_TIM_ReadReg(TIM1, CR2);//Get the TIM1 CR2 register value
		tmpccmr1 = LL_TIM_ReadReg(TIM1, CCMR1);//Get the TIM1 CCMR1 register value
		CLEAR_BIT(tmpccmr1, TIM_CCMR1_CC1S);//Reset Capture/Compare selection Bits
		MODIFY_REG(tmpccmr1, TIM_CCMR1_OC1M, LL_TIM_OCMODE_FROZEN);//Set the Output Compare Mode
		MODIFY_REG(tmpccer, TIM_CCER_CC1P, LL_TIM_OCPOLARITY_HIGH);//Set the Output Compare Polarity
		MODIFY_REG(tmpccer, TIM_CCER_CC1E, LL_TIM_OCSTATE_DISABLE);//Set the Output State
		MODIFY_REG(tmpccer, TIM_CCER_CC1NP, LL_TIM_OCPOLARITY_HIGH << 2U);//Set the complementary output Polarity
		MODIFY_REG(tmpccer, TIM_CCER_CC1NE, LL_TIM_OCSTATE_DISABLE << 2U);//Set the complementary output State
		MODIFY_REG(tmpcr2, TIM_CR2_OIS1, LL_TIM_OCIDLESTATE_LOW);//Set the Output Idle state
		MODIFY_REG(tmpcr2, TIM_CR2_OIS1N, LL_TIM_OCIDLESTATE_LOW << 1U);//Set the complementary output Idle state
		LL_TIM_WriteReg(TIM1, CR2, tmpcr2);//Write to TIM1 CR2
		LL_TIM_WriteReg(TIM1, CCMR1, tmpccmr1);//Write to TIM1 CCMR1
		LL_TIM_OC_SetCompareCH1(TIM1, 0);//Set the Capture Compare Register value
		LL_TIM_WriteReg(TIM1, CCER, tmpccer);//Write to TIM1 CCER
	}

	LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH1);
	LL_TIM_SetOCRefClearInputSource(TIM1, LL_TIM_OCREF_CLR_INT_NC);
	LL_TIM_DisableExternalClock(TIM1);
	LL_TIM_ConfigETR(TIM1, LL_TIM_ETR_POLARITY_NONINVERTED, LL_TIM_ETR_PRESCALER_DIV1, LL_TIM_ETR_FILTER_FDIV1);
	LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
	LL_TIM_SetTriggerOutput2(TIM1, LL_TIM_TRGO2_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM1);

	{
		uint32_t tmpbdtr = 0;

		//Set the Lock level, the Break enable Bit and the Polarity, the OSSR State, the OSSI State, the dead time value and the Automatic Output Enable Bit */
		// Set the BDTR bits
		MODIFY_REG(tmpbdtr, TIM_BDTR_DTG, 0);
		MODIFY_REG(tmpbdtr, TIM_BDTR_LOCK, LL_TIM_LOCKLEVEL_OFF);
		MODIFY_REG(tmpbdtr, TIM_BDTR_OSSI, LL_TIM_OSSI_DISABLE);
		MODIFY_REG(tmpbdtr, TIM_BDTR_OSSR, LL_TIM_OSSR_DISABLE);
		MODIFY_REG(tmpbdtr, TIM_BDTR_BKE, LL_TIM_BREAK_DISABLE);
		MODIFY_REG(tmpbdtr, TIM_BDTR_BKP, LL_TIM_BREAK_POLARITY_HIGH);
		MODIFY_REG(tmpbdtr, TIM_BDTR_AOE, LL_TIM_AUTOMATICOUTPUT_DISABLE);
		MODIFY_REG(tmpbdtr, TIM_BDTR_BKF, LL_TIM_BREAK_FILTER_FDIV1);

		// Set the BREAK2 input related BDTR bit-fields
		MODIFY_REG(tmpbdtr, TIM_BDTR_BK2F, LL_TIM_BREAK2_FILTER_FDIV1);
		MODIFY_REG(tmpbdtr, TIM_BDTR_BK2E, LL_TIM_BREAK2_DISABLE);
		MODIFY_REG(tmpbdtr, TIM_BDTR_BK2P, LL_TIM_BREAK2_POLARITY_HIGH);

		// Set TIM1_BDTR
		LL_TIM_WriteReg(TIM1, BDTR, tmpbdtr);
	}
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void ADC1_Init(void)
{
	Calibrate_ADC();

	NVIC_SetPriority(ADC1_2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(ADC1_2_IRQn);

	LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMA_REQUEST_0);
	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_HIGH);
	LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
	LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

	if (LL_ADC_IsEnabled(ADC1) == 0UL)//ADC instance must be disabled
	{
		MODIFY_REG(ADC1->CFGR, ADC_CFGR_RES | ADC_CFGR_ALIGN | ADC_CFGR_AUTDLY, LL_ADC_RESOLUTION_12B | LL_ADC_DATA_ALIGN_RIGHT | LL_ADC_LP_MODE_NONE);
	}

  	//ADC_REG_Init
	if (LL_ADC_IsEnabled(ADC1) == 0UL)// ADC instance must be disabled.
	{
		// Note: On this STM32 series, ADC trigger edge is set to value 0x0 by setting of trigger source to SW start.
		MODIFY_REG(ADC1->CFGR, ADC_CFGR_EXTSEL | ADC_CFGR_EXTEN | ADC_CFGR_DISCEN | ADC_CFGR_DISCNUM | ADC_CFGR_CONT | ADC_CFGR_DMAEN | ADC_CFGR_DMACFG | ADC_CFGR_OVRMOD,
				   LL_ADC_REG_TRIG_SOFTWARE | LL_ADC_REG_SEQ_DISCONT_DISABLE | LL_ADC_REG_CONV_SINGLE | LL_ADC_REG_DMA_TRANSFER_LIMITED | LL_ADC_REG_OVR_DATA_PRESERVED);
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS);
	}

	LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_VREFINT|LL_ADC_PATH_INTERNAL_TEMPSENSOR);

	if (__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(ADC12_COMMON) == 0UL)
	{
		MODIFY_REG(ADC12_COMMON->CCR, ADC_CCR_CKMODE | ADC_CCR_PRESC | ADC_CCR_DUAL | ADC_CCR_MDMA | ADC_CCR_DELAY, LL_ADC_CLOCK_ASYNC_DIV1 | LL_ADC_MULTI_INDEPENDENT);
	}

	// Disable ADC deep power down (enabled by default after reset state)
	LL_ADC_DisableDeepPowerDown(ADC1);
	LL_ADC_EnableInternalRegulator(ADC1);
	/* Delay for ADC internal voltage regulator stabilization. */
	/* Compute number of CPU cycles to wait for, from delay in us. */
	/* Note: Variable divided by 2 to compensate partially */
	/* CPU processing cycles (depends on compilation optimization). */
	/* Note: If system core clock frequency is below 200kHz, wait time */
	/* is only a few CPU processing cycles. */
	uint32_t wait_loop_index;
	wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
	while(wait_loop_index != 0)
	{
		wait_loop_index--;
	}

	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_6);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_6, LL_ADC_SAMPLINGTIME_247CYCLES_5);
	LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_6, LL_ADC_SINGLE_ENDED);

	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_16);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_16, LL_ADC_SAMPLINGTIME_247CYCLES_5);
	LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_16, LL_ADC_SINGLE_ENDED);

	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_VREFINT);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_247CYCLES_5);
	LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SINGLE_ENDED);

	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_TEMPSENSOR);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_247CYCLES_5);
	LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SINGLE_ENDED);

	LL_ADC_Enable(ADC1);
}

/**
  * Enable DMA controller clock
  */
static void DMA_Init(void)
{
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, used_ADC_channels);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t) adc_conv_results);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t) (&(ADC1->DR)) );

  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

  NVIC_SetPriority(DMA1_Channel1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void RNG_Init(void)
{
  LL_RNG_Enable(RNG);
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void TIM2_Init(void)
{
	// TIM2 interrupt Init
	NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(TIM2_IRQn);

	{
		uint32_t tmpcr1;

		tmpcr1 = LL_TIM_ReadReg(TIM2, CR1);
		MODIFY_REG(tmpcr1, (TIM_CR1_DIR | TIM_CR1_CMS), LL_TIM_COUNTERMODE_UP);// Select the Counter Mode
		MODIFY_REG(tmpcr1, TIM_CR1_CKD, LL_TIM_CLOCKDIVISION_DIV1);// Set the clock division
		LL_TIM_WriteReg(TIM2, CR1, tmpcr1);//Write to TIMx CR1
		LL_TIM_SetAutoReload(TIM2, timAR);
		LL_TIM_SetPrescaler(TIM2, 0);
		LL_TIM_GenerateEvent_UPDATE(TIM2);//Generate an update event to reload the Prescaler and the repetition counter value (if applicable) immediately
	}

	LL_TIM_DisableARRPreload(TIM2);

	{
		uint32_t tmpccmr2;
		uint32_t tmpccer;
		uint32_t tmpcr2;

		CLEAR_BIT(TIM2->CCER, TIM_CCER_CC3E);// Disable the Channel 3: Reset the CC3E Bit
		tmpccer =  LL_TIM_ReadReg(TIM2, CCER);// Get the TIMx CCER register value
		tmpcr2 = LL_TIM_ReadReg(TIM2, CR2);//Get the TIMx CR2 register value
		tmpccmr2 = LL_TIM_ReadReg(TIM2, CCMR2);//Get the TIMx CCMR2 register value
		CLEAR_BIT(tmpccmr2, TIM_CCMR2_CC3S);//Reset Capture/Compare selection Bits
		MODIFY_REG(tmpccmr2, TIM_CCMR2_OC3M, LL_TIM_OCMODE_FROZEN);//Select the Output Compare Mode
		MODIFY_REG(tmpccer, TIM_CCER_CC3P, LL_TIM_OCPOLARITY_HIGH << 8U);//Set the Output Compare Polarity
		MODIFY_REG(tmpccer, TIM_CCER_CC3E, LL_TIM_OCSTATE_DISABLE << 8U);//Set the Output State
		LL_TIM_WriteReg(TIM2, CR2, tmpcr2);//Write to TIMx CR2
		LL_TIM_WriteReg(TIM2, CCMR2, tmpccmr2);//Write to TIMx CCMR2
		LL_TIM_OC_SetCompareCH3(TIM2, 0);//Set the Capture Compare Register value
		LL_TIM_WriteReg(TIM2, CCER, tmpccer);//Write to TIMx CCER
	}


	LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH3);
	LL_TIM_SetOCRefClearInputSource(TIM2, LL_TIM_OCREF_CLR_INT_NC);
	LL_TIM_DisableExternalClock(TIM2);
	LL_TIM_ConfigETR(TIM2, LL_TIM_ETR_POLARITY_NONINVERTED, LL_TIM_ETR_PRESCALER_DIV1, LL_TIM_ETR_FILTER_FDIV1);
	LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM2);
	LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
	LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
	LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
	LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING);
	LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
	LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
	LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
	LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_FALLING);
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{
  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* TIM6 interrupt Init */
  NVIC_SetPriority(TIM6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),2, 0));
  NVIC_EnableIRQ(TIM6_IRQn);

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  TIM_InitStruct.Prescaler = 40000;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 65535;
  LL_TIM_Init(TIM6, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM6);
  LL_TIM_SetTriggerOutput(TIM6, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM6);
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */
}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{
  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
  LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  NVIC_SetPriority(TIM1_BRK_TIM15_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM1_BRK_TIM15_IRQn);

  //1562 presc and 256 arr gives ~100Hz
  TIM_InitStruct.Prescaler = 781;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 255;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM15, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM15);
  LL_TIM_OC_EnablePreload(TIM15, LL_TIM_CHANNEL_CH1);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 50;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM15, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM15, LL_TIM_CHANNEL_CH1);

  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
  TIM_OC_InitStruct.CompareValue = 0;
  LL_TIM_OC_Init(TIM15, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM15, LL_TIM_CHANNEL_CH2);

  LL_TIM_SetTriggerOutput(TIM15, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM15);
  TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
  TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
  TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
  TIM_BDTRInitStruct.DeadTime = 0;
  TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
  TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
  TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_ENABLE;
  LL_TIM_BDTR_Init(TIM15, &TIM_BDTRInitStruct);


  /**TIM15 GPIO Configuration
  PA2   ------> TIM15_CH1
  */
  GPIO_InitStruct.Pin = BACKLIGHT_PWM_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_14;
  LL_GPIO_Init(BACKLIGHT_PWM_GPIO_Port, &GPIO_InitStruct);

  LL_TIM_EnableCounter(TIM15);
  LL_TIM_CC_DisableChannel(TIM15, LL_TIM_CHANNEL_CH1);//not to light up backlight even if setting is off
  LL_TIM_CC_DisableChannel(TIM15, LL_TIM_CHANNEL_CH2);
}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
  LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);

  /* TIM16 interrupt Init */
  NVIC_SetPriority(TIM1_UP_TIM16_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  TIM_InitStruct.Prescaler = 400;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 10000;//100ms
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM16, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM16);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 0;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM16, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM16, LL_TIM_CHANNEL_CH1);
  TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
  TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
  TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
  TIM_BDTRInitStruct.DeadTime = 0;
  TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
  TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
  TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
  LL_TIM_BDTR_Init(TIM16, &TIM_BDTRInitStruct);
  /* USER CODE BEGIN TIM16_Init 2 */
  LL_TIM_EnableIT_CC1(TIM16);
  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void GPIO_Init(void)
{
	LL_GPIO_SetOutputPin(GPIOA, LCD_CS_Pin|LCD_RES_Pin);
	LL_GPIO_SetOutputPin(GPIOB, ACC_CS_Pin|EEPROM_CS_Pin);
	LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
	LL_GPIO_ResetOutputPin(GPIOB, FLASHLIGHT_Pin|D_LED_Pin);

	setPinToGenericOutput(LCD_CS_GPIO_Port, LCD_CS_Pin);
	setPinToGenericOutput(LCD_RES_GPIO_Port, LCD_RES_Pin);
	setPinToGenericOutput(LCD_DC_GPIO_Port, LCD_DC_Pin);
	setPinToGenericOutput(ACC_CS_GPIO_Port, ACC_CS_Pin);
	setPinToGenericOutput(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
	setPinToGenericOutput(D_LED_GPIO_Port, D_LED_Pin);
	setPinToGenericOutput(EEPROM_CS_GPIO_Port, EEPROM_CS_Pin);

	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE12);
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE13);
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE8);
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE9);

	LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_12);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_12);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_12);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_12);

	LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_13);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_13);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_13);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_13);

	LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_8);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_8);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_8);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_8);

	LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_9);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_9);
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_9);
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_9);

	LL_GPIO_SetPinPull(BTN_EXIT_GPIO_Port, BTN_EXIT_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinPull(BTN_BAL_SYS_WKUP2_GPIO_Port, BTN_BAL_SYS_WKUP2_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinPull(BTN_JOBB_GPIO_Port, BTN_JOBB_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinPull(BTN_ENTER_GPIO_Port, BTN_ENTER_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinMode(BTN_EXIT_GPIO_Port, BTN_EXIT_Pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinMode(BTN_BAL_SYS_WKUP2_GPIO_Port, BTN_BAL_SYS_WKUP2_Pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinMode(BTN_JOBB_GPIO_Port, BTN_JOBB_Pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinMode(BTN_ENTER_GPIO_Port, BTN_ENTER_Pin, LL_GPIO_MODE_INPUT);

	NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	//ADC GPIO
	LL_GPIO_SetPinPull(BATT_SENSE_GPIO_Port, BATT_SENSE_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinMode(BATT_SENSE_GPIO_Port, BATT_SENSE_Pin, LL_GPIO_MODE_ANALOG);

	LL_GPIO_SetPinPull(LIGHT_SENSE_GPIO_Port, LIGHT_SENSE_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinMode(LIGHT_SENSE_GPIO_Port, LIGHT_SENSE_Pin, LL_GPIO_MODE_ANALOG);

	//TIM2 GPIO
	LL_GPIO_SetPinSpeed(SPEED_SW_GPIO_Port, SPEED_SW_Pin, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(SPEED_SW_GPIO_Port, SPEED_SW_Pin, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(SPEED_SW_GPIO_Port, SPEED_SW_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetAFPin_8_15(SPEED_SW_GPIO_Port, SPEED_SW_Pin, LL_GPIO_AF_1);
	LL_GPIO_SetPinMode(SPEED_SW_GPIO_Port, SPEED_SW_Pin, LL_GPIO_MODE_ALTERNATE);

	LL_GPIO_SetPinSpeed(CADENCE_SW_GPIO_Port, CADENCE_SW_Pin, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(CADENCE_SW_GPIO_Port, CADENCE_SW_Pin, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(CADENCE_SW_GPIO_Port, CADENCE_SW_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetAFPin_0_7(CADENCE_SW_GPIO_Port, CADENCE_SW_Pin, LL_GPIO_AF_1);
	LL_GPIO_SetPinMode(CADENCE_SW_GPIO_Port, CADENCE_SW_Pin, LL_GPIO_MODE_ALTERNATE);
}

/* USER CODE BEGIN 4 */


//////////////functions////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setPinToGenericOutput(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
	LL_GPIO_SetPinSpeed(GPIOx, Pin, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(GPIOx, Pin, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOx, Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinMode(GPIOx, Pin, LL_GPIO_MODE_OUTPUT);
}

float getSlope(float angle)
{
	float slope = 0;
	slope = angle/45;//at 45° slope is 100%
	slope *= (-100.0);// *100 -> convert to %; *-1 -> because imu axis is backwards

	//not interested outside of the range +-45°
	if(slope>100.0) { slope = 100.0;}
	if(slope<(-100.0)) { slope = (-100.0);}

	return slope;
}

float calcSTM32temp(uint16_t rawtemp)
{
	float temp = 0;
	float CalTempDiff = (TEMPSENSOR_CAL2_TEMP-TEMPSENSOR_CAL1_TEMP);
	float CalValDiff = ((*TEMPSENSOR_CAL2_ADDR)-(*TEMPSENSOR_CAL1_ADDR));

	//Have to season the ADC reading with the Vref(Avcc) mismatch (3.3V/3.0V)
	//tempsensor was calibrated with TEMPSENSOR_CAL_VREFANALOG which is 3V
	temp = (CalTempDiff / CalValDiff) * (((float)rawtemp*(3.3/3.0)) - (float)(*TEMPSENSOR_CAL1_ADDR)) + (float)TEMPSENSOR_CAL1_TEMP;//°C

	return temp;
}

/*
 * @note Calibration should be performed before starting A/D conversion.
 * If it is performed after the ADC init it will mess up (sifts by one) the channel order for the DMA
 */
void Calibrate_ADC(void)
{
	LL_ADC_DisableDeepPowerDown(ADC1);
	LL_ADC_EnableInternalRegulator(ADC1);
	uint32_t wait_loop_index;
	wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
	while(wait_loop_index != 0)
	{
		wait_loop_index--;
	}
	LL_ADC_Enable(ADC1);//idk why, have to enable it before disabling, otherwise ADC calibration won't finish
	LL_mDelay(1);
	LL_ADC_Disable(ADC1);
	ADC1->CR &= (~ADC_CR_ADCALDIF);
	ADC1->CR |= ADC_CR_ADCAL;
	wait_loop_index = 0;
	while( (ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL )//Wait for calibration completion
	{
		__NOP();
		if(wait_loop_index==0x0000ffff)	{ Error_Handler(); break;}	else{}
		wait_loop_index++;
	}
}

void copy_lap(lap* l_dest, lap* l_source)
{
	l_dest->hours = l_source->hours;
	l_dest->mins = l_source->mins;
	l_dest->secs = l_source->secs;
	l_dest->dist = l_source->dist;
	l_dest->avg_speed = l_source->avg_speed;
	l_dest->max_speed = l_source->max_speed;
}

/*
 * note: standby mód, wakeup source: wakeup pin2 (pc13)
 */
void pwr_down(void)
{
	if(system_bits & moving_time_recording_EN_1)	{ system_bits &= ~(moving_time_recording_EN_1 | moving_time_recording_EN_2);}	else{}//mérés leállí­tása, ha nem lett volna leállí­tva

	//save total distance in EEPROM
	uint32_t tmpd = alldata.totdist;
	Write_M95010_W_EEPROM(EE_totdist_0, (uint8_t)( tmpd & 0x000000ff) );
	Write_M95010_W_EEPROM(EE_totdist_1, (uint8_t)((tmpd & 0x0000ff00)>>8) );
	Write_M95010_W_EEPROM(EE_totdist_2, (uint8_t)((tmpd & 0x00ff0000)>>16) );
	Write_M95010_W_EEPROM(EE_totdist_3, (uint8_t)((tmpd & 0xff000000)>>24) );

	LL_mDelay(200);//várunk hogy a gomb esetleges pergése, vagy rosszkori elengedése miatt megjövő interrupt ne rontsa el a sleepet

	LL_TIM_CC_DisableChannel(TIM1, LL_TIM_CHANNEL_CH1);
	LL_TIM_DisableCounter(TIM1);

	LL_TIM_CC_DisableChannel(TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_DisableChannel(TIM2, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_DisableChannel(TIM2, LL_TIM_CHANNEL_CH3);
	LL_TIM_DisableCounter(TIM2);

	LL_TIM_DisableCounter(TIM15);
	LL_TIM_DisableCounter(TIM16);

	LL_RTC_DisableWriteProtection(RTC);
	LL_RTC_ALMA_Disable(RTC);//ne keltse fel az rtc
	LL_RTC_EnableWriteProtection(RTC);

	NVIC_DisableIRQ(SysTick_IRQn);
	NVIC_DisableIRQ(TIM1_CC_IRQn);
	NVIC_DisableIRQ(TIM2_IRQn);
	NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);
	NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
	NVIC_DisableIRQ(TIM6_IRQn);
	NVIC_DisableIRQ(RTC_Alarm_IRQn);//not to trigger interrupt after sending to sleep the LCD

	EXTI->PR1 = 0x007DFFFF;//clear pending interrupts
	EXTI->PR2 = 0x00000078;//clear pending interrupts

	LL_GPIO_ResetOutputPin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);

	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B, ACC_CS_Pin);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_B, EEPROM_CS_Pin);

	//set pull on SPI lines in order to minimize current
	//+-------------+-------------------+---------------+
	//|				| eeprom consumes	| IMU consumes	|
	//|wihout this	| 100-400uA			| 700uA			|
	//|with this	| 0.3-0.4uA			| 4.3uA			|
	//+-------------+-------------------+---------------+
	//With LCD overall consumption is ~7.3 uA
	
	HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_A, LL_GPIO_PIN_5);//SCK
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, LL_GPIO_PIN_6);//MISO
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, LL_GPIO_PIN_7);//MOSI

	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, LCD_RES_Pin);
	HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_A, LCD_CS_Pin);

	HAL_PWREx_EnablePullUpPullDownConfig();
	LCD_sleep();
	ISM330DHCX_DeInit(MotionCompObj[CUSTOM_ISM330DHCX_0]);

	__disable_irq();
	EXTI->PR1 = 0x007DFFFF;//clear pending interrupts
	EXTI->PR2 = 0x00000078;//clear pending interrupts
	__enable_irq();

	HAL_PWREx_EnableBORPVD_ULP();
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);
	RCC->BDCR |= (RCC_BDCR_LSEON | RCC_BDCR_RTCEN);

	//clear pending interrupts again, right before going to sleep
	EXTI->PR1 = 0x007DFFFF;
	EXTI->PR2 = 0x00000078;

	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//clear wake up flag
	(void)PWR->SCR;

	MODIFY_REG(PWR->CR1, PWR_CR1_LPMS, PWR_CR1_LPMS_SHUTDOWN);//Set Shutdown mode
	SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));//Set SLEEPDEEP bit of Cortex System Control Register
#ifndef DEBUG
	DBGMCU->CR = 0; // Disable debug, trace and IWDG in low-power modes
#endif
	/* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM)
	__force_stores();
#endif
	while(1)
	{
		//TODO check if this mess is needed
		__DSB();
		__WFI();
		__WFI();
		__NOP();//check errata
		__NOP();
		__NOP();
	}
}

void write_secondary_page_data(void)
{
	write_character_V(22, 121, ':', Pixel_on, size_5x8);
	write_character_V(40, 121, ':', Pixel_on, size_5x8);
	write_dec_num_time_format_V(8, 121, l1.hours, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(26, 121, l1.mins, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(44, 121, l1.secs, Pixel_on, size_5x8, ALIGN_LEFT);
	write_text_V(0, 112,"d",Pixel_on,size_5x8);
	write_text_V(54, 112,"km",Pixel_on,size_5x8);
	write_dec_num_float_V(40, 112, l1.dist, 2, Pixel_on, size_5x8);
	write_text_V(2, 103,"vavg",Pixel_on,size_5x8);
	write_text_V(2, 94,"vmax",Pixel_on,size_5x8);
	write_dec_num_float_V(50, 103, l1.avg_speed, 1, Pixel_on, size_5x8);
	write_dec_num_float_V(50, 94, l1.max_speed, 1, Pixel_on, size_5x8);
	draw_line_x(0, 67, 91, Pixel_on);
	//
	write_character_V(22, 82, ':', Pixel_on, size_5x8);
	write_character_V(40, 82, ':', Pixel_on, size_5x8);
	write_dec_num_time_format_V(8, 82, l2.hours, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(26, 82, l2.mins, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(44, 82, l2.secs, Pixel_on, size_5x8, ALIGN_LEFT);
	write_text_V(0, 73,"d",Pixel_on,size_5x8);
	write_text_V(54, 73,"km",Pixel_on,size_5x8);
	write_dec_num_float_V(40, 73, l2.dist, 2, Pixel_on, size_5x8);
	write_text_V(2, 64,"vavg",Pixel_on,size_5x8);
	write_text_V(2, 55,"vmax",Pixel_on,size_5x8);
	write_dec_num_float_V(50, 64, l2.avg_speed, 1, Pixel_on, size_5x8);
	write_dec_num_float_V(50, 55, l2.max_speed, 1, Pixel_on, size_5x8);
	draw_line_x(0, 63, 52, Pixel_on);
	//
	write_character_V(22, 43, ':', Pixel_on, size_5x8);
	write_character_V(40, 43, ':', Pixel_on, size_5x8);
	write_dec_num_time_format_V(8, 43, l3.hours, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(26, 43, l3.mins, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(44, 43, l3.secs, Pixel_on, size_5x8, ALIGN_LEFT);
	write_text_V(0, 34,"d",Pixel_on,size_5x8);
	write_text_V(54, 34,"km",Pixel_on,size_5x8);
	write_dec_num_float_V(40, 34, l3.dist, 2, Pixel_on, size_5x8);
	write_text_V(2, 25,"vavg",Pixel_on,size_5x8);
	write_text_V(2, 16,"vmax",Pixel_on,size_5x8);
	write_dec_num_float_V(50, 25, l3.avg_speed, 1, Pixel_on, size_5x8);
	write_dec_num_float_V(50, 16, l3.max_speed, 1, Pixel_on, size_5x8);
	draw_line_x(0, 63, 13, Pixel_on);
	//
	write_text_V(2, 0, "laps:", Pixel_on, size_5x8);
	write_dec_num_uint8_t_V(60, 0, ltmp.laps, Pixel_on, size_5x8, ALIGN_RIGHT);
}

void write_main_page_data(void)
{
	write_character_V(22, 121, ':', Pixel_on, size_5x8);
	write_character_V(40, 121, ':', Pixel_on, size_5x8);
	draw_line_x(0, 63, 117, Pixel_on);
	draw_line_x(0, 63, 115, Pixel_on);
	write_text_V(54, 104,"km",Pixel_on,size_5x8);
	draw_line_x(54, 63, 101, Pixel_on);
	write_text_V(57, 93,"h",Pixel_on,size_5x8);
	write_text_V(2, 83,"max",Pixel_on,size_5x8);
	write_text_V(2, 73,"avg",Pixel_on,size_5x8);
	draw_line_x(0, 63, 70, Pixel_on);
	write_text_V(9, 61,"rpm",Pixel_on,size_5x8);
	draw_line_y(47, 69, 35,Pixel_on);
	setpixel(47, 66, Pixel_on);		  //ramp icon
	draw_line_x(46, 47, 65, Pixel_on);//
	draw_line_x(45, 47, 64, Pixel_on);//
	draw_line_x(44, 47, 63, Pixel_on);//
	draw_line_x(43, 47, 62, Pixel_on);//
	draw_line_x(42, 47, 61, Pixel_on);//
	write_text_V(54, 61,"%",Pixel_on,size_5x8);
	draw_line_x(0, 63, 46, Pixel_on);
	write_text_V(12, 38,"km",Pixel_on,size_5x8);
	draw_line_y(26, 45, 35, Pixel_on);
	setpixel(47, 43, Pixel_on);//° character
	setpixel(48, 44, Pixel_on);//
	setpixel(48, 42, Pixel_on);//
	setpixel(49, 43, Pixel_on);//
	write_character_V(50, 37, 'C', Pixel_on, size_5x8);
	draw_line_x(0, 63, 25, Pixel_on);
	draw_line_x(0, 63, 11, Pixel_on);
	write_character_V(0, 14, 'e', Pixel_on, size_5x8);
	write_character_V(0, 0, 'm', Pixel_on, size_5x8);

	get_rtc_data();
	write_dec_num_time_format_V(8, 121, RTCtime.Hours, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(26, 121, RTCtime.Minutes, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(44, 121, RTCtime.Seconds, Pixel_on, size_5x8, ALIGN_LEFT);

	draw_battery_band(alldata.batt);
	write_dec_num_float_V(35, 93, alldata.speed, 1, Pixel_on, size_10x16);
	write_dec_num_float_V(44, 83, alldata.maxspeed, 2, Pixel_on, size_5x8);
	write_dec_num_float_V(44, 73, get_avg_speed(alldata.dist, alldata.moving_time), 2, Pixel_on, size_5x8);
	write_dec_num_float_V(23, 50, alldata.cadence, 1, Pixel_on, size_5x8);
	write_dec_num_float_V(55, 50, alldata.grad, 1, Pixel_on, size_5x8);
	write_dec_num_float_V(19, 28, (alldata.dist*curr_tyre)/1000, 2, Pixel_on, size_5x8);//km a mértékegység
	write_dec_num_float_V(52, 28, alldata.acc_tempsensor, 1 , Pixel_on, size_5x8);
	write_elapsed_time();

	if(system_bits & moving_time_recording_EN_1)	{ write_character_V(58, 14, '>', Pixel_on, size_5x8);}
	else{ fill_rectangle_xy_height_width(58, 14, 7, 5, Pixel_off);}
}

void write_elapsed_time(void)
{
	uint8_t th,tm,ts;
	get_elapsed_time(alldata.elapsed_time, &th, &tm, &ts);
	write_character_V(22, 14, ':', Pixel_on, size_5x8);
	write_character_V(40, 14, ':', Pixel_on, size_5x8);
	write_dec_num_time_format_V(8, 14, th, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(26, 14, tm, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(44, 14, ts, Pixel_on, size_5x8, ALIGN_LEFT);

	get_elapsed_time(alldata.moving_time, &th, &tm, &ts);
	write_character_V(22, 0, ':', Pixel_on, size_5x8);
	write_character_V(40, 0, ':', Pixel_on, size_5x8);
	write_dec_num_time_format_V(8, 0, th, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(26, 0, tm, Pixel_on, size_5x8, ALIGN_LEFT);
	write_dec_num_time_format_V(44, 0, ts, Pixel_on, size_5x8, ALIGN_LEFT);
}

void get_elapsed_time(uint32_t et_sec, uint8_t* hf, uint8_t* mf, uint8_t* sf)
{
	*hf = ( (uint8_t)(et_sec/3600) );
	et_sec = (et_sec-((*hf)*3600));
	*mf = ( (uint8_t)(et_sec/60) );
	*sf = ( (uint8_t)(et_sec-((*mf)*60)));
}

float get_avg_speed(uint32_t distf, uint32_t secf)
{
	float vavg=(curr_tyre*distf);
	vavg /= secf;  // m/s
	vavg *= 3.6; // km/h
	if( (vavg<200) && (vavg>=0) )	{ return vavg;}
	else{ return 0;}
}

void write_rtc_data(void)
{
	LL_RTC_DisableWriteProtection(RTC);
	LL_RTC_EnableInitMode(RTC);
	tim_delay_ms(2);
	LL_RTC_TIME_Config(RTC, LL_RTC_TIME_FORMAT_AM_OR_24, __LL_RTC_CONVERT_BIN2BCD(RTCtime.Hours), __LL_RTC_CONVERT_BIN2BCD(RTCtime.Minutes), __LL_RTC_CONVERT_BIN2BCD(RTCtime.Seconds));//HAL_RTC_SetTime(&hrtc, &RTCtime, RTC_FORMAT_BIN);
	LL_RTC_DATE_Config(RTC, __LL_RTC_CONVERT_BIN2BCD(RTCdate.WeekDay), __LL_RTC_CONVERT_BIN2BCD(RTCdate.Day), __LL_RTC_CONVERT_BIN2BCD(RTCdate.Month), __LL_RTC_CONVERT_BIN2BCD(RTCdate.Year));//HAL_RTC_SetDate(&hrtc, &RTCdate, RTC_FORMAT_BIN);
	LL_RTC_DisableInitMode(RTC);
	tim_delay_ms(2);
	LL_RTC_EnableWriteProtection(RTC);
}

void get_rtc_data(void)
{
	RTCtime.Hours = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));
	RTCtime.Minutes = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
	RTCtime.Seconds = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
	RTCdate.Year = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC));
	RTCdate.Day = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
	RTCdate.Month = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
	RTCdate.WeekDay = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetWeekDay(RTC));
}

void _err_hnd_st_msg(char* str1, char*str2)
{
	delete_disp_mat();
	if(str1!=0)	{write_text_V(0, 64, str1, Pixel_on, size_5x8);}
	if(str2!=0)	{write_text_V(0, 54, str2, Pixel_on, size_5x8);}
	print_disp_mat();
	btn=0;
	while(!btn)	{ asm("nop");}
}

uint32_t get_dist_for_new_tyre(float old_tyre, float new_tyre, uint32_t current_dist_val)
{
	return  ((uint32_t) round((((float)current_dist_val*old_tyre)/new_tyre)) );
}

/*
 * @note: bits are write protected, before calling this function use LL_RTC_DisableWriteProtection(RTC)
 */
void SetSmoothCalib(int16_t calv)
{
	uint16_t atmp=0;
	while((RTC->ICSR & RTC_ICSR_RECALPF) == RTC_ICSR_RECALPF) //Wait until it's allow to modify calibartion register
	{
		if(atmp == 0xffff)	{ return;} else{}//timeout
		asm("nop");
		atmp++;
	}
	//clear prev cal period value; 32sec period val is 0 so don't have to set new, clr old calib val
	RTC->CALR &= (uint32_t)~((uint32_t)RTC_CALR_CALW8 | (uint32_t)RTC_CALR_CALW16 | (uint32_t)RTC_CALR_CALM_Msk | (uint32_t)RTC_CALR_CALP_Msk);
	while((RTC->ICSR & RTC_ICSR_RECALPF) == RTC_ICSR_RECALPF) //Wait until it's allow to modify calibartion register
	{
		if(atmp == 0xffff)	{ return;} else{}//timeout
		asm("nop");
		atmp++;
	}
	if(calv != 0)
	{
		if(calv < 0)
		{
			calv = abs(calv);
			RTC->CALR |= (((uint32_t)calv)&RTC_CALR_CALM_Msk);//mask out pulses  //bits 8:0 are used
		}
		else
		{	//ha pl +1-et akarok beállítani, CALP bittel hozzáadok 512-t aztán CALM bitekkel elveszek 511-et
			RTC->CALR |= (uint32_t) ( (uint32_t)RTC_CALR_CALP |  (512-(((uint32_t)calv)&RTC_CALR_CALM_Msk)) );//insert 512 pulse and decrease the amount of added pulses with CALM bits
		}
	}else{ return;}
}

/**
  * Care should be taken where you use this delay!!!!!!!!
  * The interrupt priority is 2, so should only use, where it can be guranteed that won't be called in higher prio interupts -
  * because it will freeze the application
  * Also don't use it in the init section where it's interrupt is not configured, or where the global interrupt EN is disabled
  */
void tim_delay_ms(uint16_t d)
{
	//prescaler: 40000;
	//period: 65535
	//1ms -> 1 clk;
	//max delay:   65535 ms

	tim_delay_ms_flag = 0;

	LL_TIM_SetAutoReload(TIM6, (uint32_t)d);
	TIM6->CNT = 1;
	LL_TIM_EnableCounter(TIM6);

	while(tim_delay_ms_flag == 0)//wait for the interrupt
	{
		__NOP();
	}
	tim_delay_ms_flag = 0;

	LL_TIM_DisableCounter(TIM6);
}

void calculate_batt_voltage(uint16_t baterry_ADC_data, uint16_t vrefint_ADC_data)
{
	// VCHANNELx = (3 V × VREFINT_CAL × ADC_DATAx) / (VREFINT_DATA × FULL_SCALE)

	float batt_volt = 0;

	batt_volt = (float)(*(VREFINT_CAL_ADDR) * (VREFINT_CAL_VREF/1000) * baterry_ADC_data);
	batt_volt /= (float)(vrefint_ADC_data * 4095);//
	alldata.batt = (batt_volt * batt_input_volt_div_ratio);
}

void draw_battery_band(float volt_val)
{
	if(volt_val > min_batt)
	{
		if(volt_val < max_batt)
		{
			uint8_t line = 0;
			line = ((pixels_x-1) * ( (volt_val - min_batt) / (max_batt - min_batt) ) );
			if(saved_bits & LCD_inverted)	{ draw_line_x(line, 63, 116, Pixel_on);}
			else{ { draw_line_x(0, line, 116, Pixel_on);}}
		}
		else
		{
			if(saved_bits & LCD_inverted)	{ draw_line_x(0, 63, 116, Pixel_off);}
			else{ { draw_line_x(0, 63, 116, Pixel_on);}}
		}
	}else{/*line width would be zero, so we don't have to do anything*/}
}

////init functions///////////////////////////////////////////////////////////////////////////////
static void init(void)
{
	HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

	__enable_irq();

	LL_mDelay(20);//wait

	{
		uint8_t tmp=0;
		//disable protection on all memory segment in EEPROM
		tmp = Read_SREG_M95010_W_EEPROM();
		if(tmp & (~((1<<BP0) | (1<<BP1))))	{ Write_SREG_M95010_W_EEPROM( tmp & (~((1<<BP0) | (1<<BP1))) );}	else{}
	}
	saved_bits = Read_M95010_W_EEPROM(EE_bitek);
	flashlight_blink_val = Read_M95010_W_EEPROM(EE_flashlight_blink);

	LCD_init(Read_M95010_W_EEPROM(EE_contrast));//15
	LCD_send_cmd(CMD_display_all_points_on);	//LCD test, meg amúgy is felvillan egy kicsit induláskor, és az nem néz ki túl jól
	LL_mDelay(500);								//
	LCD_send_cmd(CMD_display_all_points_off);	//

	uint8_t tmp_reg = 0;
	ism330dhcx_read_reg(&((ISM330DHCX_Object_t*)MotionCompObj[CUSTOM_ISM330DHCX_0])->Ctx, ISM330DHCX_TAP_CFG2, &tmp_reg, 1);
	tmp_reg |= 0x60;//Enable activity/inactivity (sleep) function. Sets accelerometer ODR to 12.5 Hz (low-power mode), gyro to power-down mode)
	ism330dhcx_write_reg(&((ISM330DHCX_Object_t*)MotionCompObj[CUSTOM_ISM330DHCX_0])->Ctx, ISM330DHCX_TAP_CFG2, &tmp_reg, 1);

	LL_TIM_EnableCounter(TIM1);
	LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableUpdateEvent(TIM1);
	LL_TIM_EnableIT_CC1(TIM1);

	LL_TIM_EnableCounter(TIM2);
	LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3);
	LL_TIM_EnableUpdateEvent(TIM2);
	LL_TIM_EnableIT_CC1(TIM2);
	LL_TIM_EnableIT_CC2(TIM2);
	LL_TIM_EnableIT_CC3(TIM2);

	LL_TIM_EnableUpdateEvent(TIM6);
	LL_TIM_EnableIT_UPDATE(TIM6);

	{
		uint8_t tmp=0;
		tmp = Read_M95010_W_EEPROM(EE_curr_tyre_id);
		switch(tmp)
		{
			case tyre_id_700x23C:	curr_tyre = tyre_700x23C;
									break;
			case tyre_id_700x25C: 	curr_tyre = tyre_700x25C;
									break;
			//case tyre_id_ :	break;
			//case tyre_id_ :	break;
			case tyre_id_custom_perimeter:
									curr_tyre = (float) (((uint16_t)( Read_M95010_W_EEPROM(EE_custom_tyre_perimeter_L) | (Read_M95010_W_EEPROM(EE_custom_tyre_perimeter_H)<<8) )) /(float)1000 );
									break;
			default:				curr_tyre=50;//error
									_err_hnd_st_msg("Tyre__", "__error");
									break;
		}
	}

	alldata.totdist = (uint32_t)(Read_M95010_W_EEPROM(EE_totdist_0) |
								(Read_M95010_W_EEPROM(EE_totdist_1)<<8) |
								(Read_M95010_W_EEPROM(EE_totdist_2)<<16) |
								(Read_M95010_W_EEPROM(EE_totdist_3)<<24) );

	__disable_irq();

	LL_RCC_EnableRTC();//Peripheral clock enable
	NVIC_SetPriority(RTC_Alarm_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
	NVIC_EnableIRQ(RTC_Alarm_IRQn);

	LL_RTC_DisableWriteProtection(RTC);

	if (LL_RTC_EnterInitMode(RTC) != ERROR)//Set Initialization mode
	{
		LL_RTC_SetHourFormat(RTC, LL_RTC_HOURFORMAT_24HOUR);
		if(PWR->SR1 & PWR_SR1_WUF2)	//check if wake up triggered by wakeUpPin2 or it was a power on reset
		{
			PWR->SCR &= ~PWR_SCR_CWUF2;//clear wakeup flag
		}
		else//if power on reset then do the init
		{
			LL_RTC_SetSynchPrescaler(RTC, 255);
			LL_RTC_SetAsynchPrescaler(RTC, 127);
		}
		LL_RTC_DisableInitMode(RTC);// Exit Initialization mode
		if(LL_RTC_IsShadowRegBypassEnabled(RTC) == 0U)//If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed
		{
		  LL_RTC_WaitForSynchro(RTC);
		}
	}

	LL_RTC_ALMA_Disable(RTC);
	LL_RTC_ALMA_SetMask(RTC, LL_RTC_ALMA_MASK_ALL);
	LL_RTC_ALMA_SetSubSecondMask(RTC, 0);
	LL_RTC_ALMA_SetTimeFormat(RTC, LL_RTC_ALMA_TIME_FORMAT_AM);
	EXTI->IMR1 |= LL_EXTI_LINE_18; //enable exti 18 int
	EXTI->RTSR1 |= LL_EXTI_LINE_18; //rising edge
	LL_RTC_EnableIT_ALRA(RTC);
	LL_RTC_ALMA_Enable(RTC);
	SetSmoothCalib( (int16_t)( (int16_t)Read_M95010_W_EEPROM(EE_RTC_smcalL) | ((int16_t)Read_M95010_W_EEPROM(EE_RTC_smcalH) << 8) ) );// best value on BK2.2 is 29 //kell ide mert tápfesz elvesztése esetén nem jegyzi meg az értéket
	LL_RTC_EnableWriteProtection(RTC);
	LL_RTC_CAL_LowPower_Enable(RTC);

	__enable_irq();
}


void ClockConfig(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
	while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
	{
	}
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	while (LL_PWR_IsActiveFlag_VOS() != 0)
	{
	}
	LL_RCC_HSI_Enable();

	/* Wait till HSI is ready */
	while(LL_RCC_HSI_IsReady() != 1)
	{

	}
	LL_RCC_HSI_SetCalibTrimming(64);
	LL_RCC_HSI48_Enable();

	/* Wait till HSI48 is ready */
	while(LL_RCC_HSI48_IsReady() != 1)
	{

	}
	LL_PWR_EnableBkUpAccess();

	LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
	LL_RCC_LSE_Enable();

	/* Wait till LSE is ready */
	while(LL_RCC_LSE_IsReady() != 1)
	{

	}

	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 10, LL_RCC_PLLR_DIV_4);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while(LL_RCC_PLL_IsReady() != 1)
	{

	}
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{

	}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_SetSystemCoreClock(40000000);

	/* Update the time base */
	if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
	{
		Error_Handler();
	}

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

	LL_RCC_SetRNGClockSource(LL_RCC_RNG_CLKSOURCE_HSI48);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);

	LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetClockSource(TIM15, LL_TIM_CLOCKSOURCE_INTERNAL);

	if(LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSE)
	{
		FlagStatus pwrclkchanged = RESET;
		// Update LSE configuration in Backup Domain control register
		// Requires to enable write access to Backup Domain if necessary
		if (LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_PWR) != 1U)
		{
			// Enables the PWR Clock and Enables access to the backup domain
			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
			pwrclkchanged = SET;
		}
		if (LL_PWR_IsEnabledBkUpAccess () != 1U)
		{
			// Enable write access to Backup domain
			LL_PWR_EnableBkUpAccess();
			while (LL_PWR_IsEnabledBkUpAccess () == 0U)
			{
				__NOP();
			}
		}
		LL_RCC_ForceBackupDomainReset();
		LL_RCC_ReleaseBackupDomainReset();
		LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
		LL_RCC_LSE_Enable();

		// Wait till LSE is ready
		while(LL_RCC_LSE_IsReady() != 1)
		{
			__NOP();
		}
		LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
		// Restore clock configuration if changed
		if (pwrclkchanged == SET)
		{
			LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
		}
	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
