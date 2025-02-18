/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ST7565_64x128_LCD.h"
#include "disp_fgv.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

extern volatile uint8_t btn;

extern bkdata alldata;//főképernyő adatok
extern lap_tmp ltmp;

extern volatile int8_t main_page;

extern volatile uint8_t system_bits;


extern uint8_t tim_delay_ms_flag;

extern volatile float curr_tyre;//kerület m-ben
extern capturedata cpt1;
extern capturedata cpt2;

extern volatile uint16_t adc_conv_results[used_ADC_channels];// 0-ch1-battery, 1-ch9-lightsensor, 2-ch17-vrefint //this is the destination of the DMA1

extern volatile int32_t avgtemp;
volatile int32_t avgtemp_samplenum = 0;
extern volatile uint8_t temp_presc;

extern volatile uint8_t Tgame_status;

volatile float prev_v = 0, curr_v = 0;//previous and current velocity in m/s

volatile uint8_t flashlight_toggle_cnt = 0;
extern volatile uint8_t flashlight_blink_freq;


void (*GameMainIsrPntr)() = NULL;
void (*GameBtnIsrPntr)() = NULL;
volatile uint16_t game_main_isr_presc = 0;
volatile uint16_t game_main_isr_presc_cmp_val = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
	if(LL_DMA_IsActiveFlag_TC1(DMA1))
	{
		//LL_DMA_ClearFlag_TC1(DMA1);
		//LL_DMA_ClearFlag_HT1(DMA1);
		LL_DMA_ClearFlag_GI1(DMA1);

		alldata.rawbatt = adc_conv_results[0];
		alldata.rawlight = adc_conv_results[1];
		alldata.rawvrefint = adc_conv_results[2];
		alldata.rawtempsensor = adc_conv_results[3];

		calculate_batt_voltage(adc_conv_results[0], adc_conv_results[2]);

		LL_GPIO_TogglePin(D_LED_GPIO_Port, D_LED_Pin);
	}

	if(LL_ADC_IsActiveFlag_EOS(ADC1))
	{
		LL_ADC_ClearFlag_EOS(ADC1);
	}

	if(LL_DMA_IsActiveFlag_TE1(DMA1))
	{
		LL_DMA_ClearFlag_TE1(DMA1);
	}
  /* USER CODE END DMA1_Channel1_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles ADC1 and ADC2 interrupts.
  */
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */
	if(LL_ADC_IsActiveFlag_EOS(ADC1) != 0)
	{
		LL_ADC_ClearFlag_EOS(ADC1);
	}

	if(LL_ADC_IsActiveFlag_OVR(ADC1) != 0)
	{
		LL_ADC_ClearFlag_OVR(ADC1);
	}
  /* USER CODE END ADC1_2_IRQn 0 */
  /* USER CODE BEGIN ADC1_2_IRQn 1 */

  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_8) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8);
    /* USER CODE BEGIN LL_EXTI_LINE_8 */
    if(LL_GPIO_IsInputPinSet(BTN_JOBB_GPIO_Port, BTN_JOBB_Pin))		{ btn |= jobbgomb;}
    else{ btn &= ~jobbgomb;}
    /* USER CODE END LL_EXTI_LINE_8 */
  }
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_9) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_9);
    /* USER CODE BEGIN LL_EXTI_LINE_9 */
    if(LL_GPIO_IsInputPinSet(BTN_ENTER_GPIO_Port, BTN_ENTER_Pin))		{ btn |= entergomb;}
    else{ btn &= ~entergomb;}
    /* USER CODE END LL_EXTI_LINE_9 */
  }
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
#ifndef DEBUG
  if(btn)
  {
	  if(GameBtnIsrPntr != NULL)
	  {
		  GameBtnIsrPntr();
	  }
  } else{}
#endif
  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM1 break interrupt and TIM15 global interrupt.
  */
void TIM1_BRK_TIM15_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 0 */
	if(LL_TIM_IsActiveFlag_CC2(TIM15))
	{
		LL_TIM_ClearFlag_CC2(TIM15);
		flashlight_toggle_cnt++;
		uint8_t tmp = ceil(1/(float)(101.0f-flashlight_blink_freq))*100;//shit TODO fix
		if(tmp==flashlight_toggle_cnt)
		{
			LL_GPIO_TogglePin(FLASHLIGHT_GPIO_Port, FLASHLIGHT_Pin);
			flashlight_toggle_cnt=0;
		}
		else
		{

		}
	}
  /* USER CODE END TIM1_BRK_TIM15_IRQn 0 */
  /* USER CODE BEGIN TIM1_BRK_TIM15_IRQn 1 */

  /* USER CODE END TIM1_BRK_TIM15_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
  */
void TIM1_UP_TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 0 */
	if(LL_TIM_IsActiveFlag_CC1(TIM16))
	{
		LL_TIM_ClearFlag_CC1(TIM16);

		game_main_isr_presc++;
		if(game_main_isr_presc >game_main_isr_presc_cmp_val)
		{
			game_main_isr_presc = 0;
			if(GameMainIsrPntr != NULL)
			{
				GameMainIsrPntr();
			}
		}
	}
  /* USER CODE END TIM1_UP_TIM16_IRQn 0 */
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

	//speed interrupt variables////////////////////////////////////////////////
	volatile uint32_t ti1 = 0; //number of osc periods between 2 captures
	volatile float captureinterval;//time between 2 captures [s]
	volatile float v1 = 0; //tmp velocity variable
	///////////////////////////////////////////////////////////////////////////
	//cadence interrupt variables//////////////////////////////////////////////
	volatile float t2;
	volatile uint32_t ti2=0;
	///////////////////////////////////////////////////////////////////////////


	//speed interrupt/////////////////////////////////////////////////////////////////////
	//measures every period beetween interrupts
	if(TIM2->SR & LL_TIM_SR_CC1IF)
	{
		TIM2->SR &= ~LL_TIM_SR_CC1IF;//clear isr flag
		TIM2->SR &= ~LL_TIM_SR_CC1OF;////clear overcaptureflag

		#ifdef DEBUG
		LL_GPIO_TogglePin(D_LED_GPIO_Port, D_LED_Pin);
		#endif

		if(system_bits & moving_time_recording_EN_1)
		{
			alldata.dist++;
			alldata.totdist++;
			ltmp.dist++;
		}

		switch(cpt1.cptnum)
		{
			case 0:	cpt1.c1 = LL_TIM_IC_GetCaptureCH1(TIM2);
					cpt1.cptnum = 1;
					cpt1.datastate |= c1Valid;

					if( (cpt1.datastate & (c1Valid|c2Valid)) == (c1Valid|c2Valid) )
					{
						if(cpt1.ovfbtwcpt == 0)
						{
							ti1=( cpt1.c1 - cpt1.c2 );
						}
						else
						{
							ti1=( cpt1.c1 + (timAR-cpt1.c2) );//32 bit timer //only 1 overflow occured
							if(cpt1.ovfbtwcpt > 1)
							{
								cpt1.ovfbtwcpt--;//one whole timer period was already added before this if
								ti1=( ti1+(cpt1.ovfbtwcpt*timAR) );
							}else{}
						}
					}else{}
					break;

			case 1:	cpt1.c2 = LL_TIM_IC_GetCaptureCH1(TIM2);
					cpt1.cptnum = 0;
					cpt1.datastate |= c2Valid;

					if( (cpt1.datastate & (c1Valid|c2Valid)) == (c1Valid|c2Valid) )
					{
						if(cpt1.ovfbtwcpt == 0)
						{
							ti1=( cpt1.c2 - cpt1.c1 );
						}
						else
						{
							ti1=( cpt1.c2 + (timAR-cpt1.c1) );//32 bit timer //only 1 overflow occured
							if(cpt1.ovfbtwcpt > 1)
							{
								cpt1.ovfbtwcpt--;//one whole timer period was already added before this if
								ti1=( ti1+(cpt1.ovfbtwcpt*timAR) );
							}else{}
						}
					}else{}
					break;

			default:	break;
		}

		if( (cpt1.datastate & (c1Valid|c2Valid)) == (c1Valid|c2Valid) )
		{
			cpt1.ovfbtwcpt=0;
			captureinterval = ( (float)(ti1*(float)timtick) );
			v1=(float)((curr_tyre/captureinterval));//  v=s/t	// m/s
			v1=(float)(v1*3.6);//m/s -> km/h
			if(v1<(alldata.speed+50))//ha egy fordulaton belül túlságosan megnő a sebesség akkor ott pergett a reed cső és a kondi nem szűrte ki(ha v1 < előző+50)
			{
				alldata.speed=v1;
			}
			if(alldata.maxspeed < alldata.speed)	{ alldata.maxspeed=alldata.speed;}	else{}
			if(ltmp.maxspeed < alldata.speed)	{ ltmp.maxspeed=alldata.speed;}	else{}
		}else{}

	}else{}

	//cadence interrupt/////////////////////////////////////////////////////////////////////
	//only measures every second period between interrupts
	if(TIM2->SR & LL_TIM_SR_CC2IF)
	{
		TIM2->SR &= ~LL_TIM_SR_CC2IF;//clear isr flag
		TIM2->SR &= ~LL_TIM_SR_CC2OF;////clear overcaptureflag
		if(cpt2.cptnum==1)
		{
			cpt2.c2 = LL_TIM_IC_GetCaptureCH2(TIM2);
			cpt2.cptnum=0;

			if(cpt2.ovfbtwcpt == 0)
			{
				ti2=( cpt2.c2 - cpt2.c1 );
			}
			else
			{
				ti2=( cpt2.c2 + (timAR-cpt2.c1) );
				if(cpt2.ovfbtwcpt > 1)
				{
					cpt2.ovfbtwcpt--;
					ti2=( ti2+(cpt2.ovfbtwcpt*timAR) );
				}else{}
			}
			t2=( (float)(ti2*(float)timtick) );
			alldata.cadence=(float)(60/t2);// ((1/t)*60)
		}
		else
		{
			cpt2.ovfbtwcpt=0;
			cpt2.c1 = LL_TIM_IC_GetCaptureCH2(TIM2);
			cpt2.cptnum=1;
		}
	}else{}
  /* USER CODE END TIM2_IRQn 0 */
  /* USER CODE BEGIN TIM2_IRQn 1 */

	//timer overflow///////////////////////////////////////////////////////////////////////
	if(TIM2->SR &= LL_TIM_SR_CC3IF)
	{
		TIM2->SR &= ~LL_TIM_SR_CC3IF;//clear isr flag
		cpt1.ovfbtwcpt++;
		cpt2.ovfbtwcpt++;

		if(cpt1.ovfbtwcpt > max_ovf_num)
		{
			cpt1.cptnum = 0;
			alldata.speed = 0;
			cpt1.datastate = 0;
		}else{}

		if(cpt2.ovfbtwcpt > max_ovf_num)
		{
			cpt2.cptnum = 0;
			alldata.cadence = 0;
			cpt2.datastate = 0;
		}else{}
	}else{}

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_12) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_12);
    /* USER CODE BEGIN LL_EXTI_LINE_12 */
    if(LL_GPIO_IsInputPinSet(BTN_EXIT_GPIO_Port, BTN_EXIT_Pin))		{ btn |= exitgomb;}
    else{ btn &= ~exitgomb;}
    /* USER CODE END LL_EXTI_LINE_12 */
  }
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
    /* USER CODE BEGIN LL_EXTI_LINE_13 */
    if(LL_GPIO_IsInputPinSet(BTN_BAL_SYS_WKUP2_GPIO_Port, BTN_BAL_SYS_WKUP2_Pin))		{ btn |= balgomb;}
    else{ btn &= ~balgomb;}
    /* USER CODE END LL_EXTI_LINE_13 */
  }
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
#ifndef DEBUG
  if(btn)
  {
	  if(GameBtnIsrPntr != NULL)
	  {
		  GameBtnIsrPntr();
	  }
  } else{}
#endif
  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles RTC alarm interrupt through EXTI line 18.
  */
void RTC_Alarm_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_Alarm_IRQn 0 */
	if(LL_RTC_IsActiveFlag_ALRA(RTC))
	{
		LL_RTC_ClearFlag_ALRA(RTC);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_18);

		if((alldata.speed > 0.5) && (system_bits & moving_time_recording_EN_1))	{ system_bits |= moving_time_recording_EN_2;}	else{ system_bits &= ~moving_time_recording_EN_2;}
		if(system_bits & moving_time_recording_EN_1)
		{
			alldata.elapsed_time++;
			ltmp.elapsed_time++;
			if(system_bits & moving_time_recording_EN_2)
			{
				alldata.moving_time++;
			}else{}
		}

		if(main_page==1)
		{
			delete_disp_mat();
			write_main_page_data();
			print_disp_mat();
		}else{}

		if(LL_ADC_IsActiveFlag_ADRDY(ADC1))
		{
			LL_ADC_REG_StartConversion(ADC1);
		}

	}else{}
  /* USER CODE END RTC_Alarm_IRQn 0 */
  /* USER CODE BEGIN RTC_Alarm_IRQn 1 */

  /* USER CODE END RTC_Alarm_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */
	if(LL_TIM_IsActiveFlag_UPDATE(TIM6))
	{
		LL_TIM_ClearFlag_UPDATE(TIM6);
		tim_delay_ms_flag = 1;
	}else{}
  /* USER CODE END TIM6_IRQn 0 */
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
