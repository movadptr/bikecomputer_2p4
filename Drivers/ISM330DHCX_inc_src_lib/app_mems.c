/**
  ******************************************************************************
  * File Name          : app_mems.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-MEMS1.11.1.0 instances.
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

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_mems.h"
#include "main.h"
#include <stdio.h>

#include "stm32l4xx_hal.h"
#include "demo_serial.h"
#include "bsp_ip_conf.h"
#include "fw_version.h"
#include "motion_di_manager.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DWT_LAR_KEY  0xC5ACCE55 /* DWT register unlock key */
#define ALGO_FREQ  100U /* Algorithm frequency 100Hz */
#define ACC_ODR  ((float)ALGO_FREQ)
#define ACC_FS  2 /* FS = <-2g, 2g> */
#define ALGO_PERIOD  (1000000U / ALGO_FREQ) /* Algorithm period [us] */
#define FROM_MG_TO_G  0.001f
#define FROM_G_TO_MG  1000.0f
#define FROM_MDPS_TO_DPS  0.001f
#define FROM_DPS_TO_MDPS  1000.0f
#define FROM_MGAUSS_TO_UT50  (0.1f/50.0f)
#define FROM_UT50_TO_MGAUSS  500.0f

/* Public variables ----------------------------------------------------------*/

volatile uint32_t SensorsEnabled = (ACCELEROMETER_SENSOR | GYROSCOPE_SENSOR);
char LibVersion[35];
int32_t LibVersionLen;

MDI_cal_type_t AccCalMode = MDI_CAL_NONE;
MDI_cal_type_t GyrCalMode = MDI_CAL_NONE;

/* Extern variables ----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static MOTION_SENSOR_Axes_t AccValue;
static MOTION_SENSOR_Axes_t GyrValue;
static float TempValue;
static int64_t Timestamp = 0;

extern float Grotation[MDI_NUM_AXES];
extern bkdata alldata;//főképernyő adatok

/* Private function prototypes -----------------------------------------------*/
static void MX_DynamicInclinometer_Init(void);
static void MX_DynamicInclinometer_Process(void);
static void DI_Data_Handler(void);
static void Init_Sensors(void);
static void Accelero_Sensor_Handler(void);
static void Gyro_Sensor_Handler(void);
static void Temperature_Sensor_Handler(void);

#ifdef BSP_IP_MEMS_INT1_PIN_NUM
static void MEMS_INT1_Force_Low(void);
static void MEMS_INT1_Init(void);
#endif

void MX_MEMS_Init(void)
{
  MX_DynamicInclinometer_Init();
}

/*
 * LM background task
 */
void MX_MEMS_Process(void)
{
  MX_DynamicInclinometer_Process();
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialize the application
  * @retval None
  */
static void MX_DynamicInclinometer_Init(void)
{
#ifdef BSP_IP_MEMS_INT1_PIN_NUM
  /* Force MEMS INT1 pin of the sensor low during startup in order to disable I3C and enable I2C. This function needs
   * to be called only if user wants to disable I3C / enable I2C and didn't put the pull-down resistor to MEMS INT1 pin
   * on his HW setup. This is also the case of usage X-NUCLEO-IKS4A1 or X-NUCLEO-IKS01A3 expansion board together with
   * sensor in DIL24 adapter board where the LDO with internal pull-up is used.
   */
  MEMS_INT1_Force_Low();
#endif

  Init_Sensors();//Initialize (disabled) sensors

#ifdef BSP_IP_MEMS_INT1_PIN_NUM
  MEMS_INT1_Init();// Initialize MEMS INT1 pin back to it's default state after I3C disable / I2C enable
#endif

  MotionDI_manager_init((int)ALGO_FREQ);//DynamicInclinometer API initialization function
  // OPTIONAL
  // Get library version
  MotionDI_manager_get_version(LibVersion, &LibVersionLen);
}

/**
  * @brief  Initialize all sensors
  * @param  None
  * @retval None
  */
static void Init_Sensors(void)
{
  BSP_SENSOR_ACC_Init();
  BSP_SENSOR_GYR_Init();
  //BSP_SENSOR_TEMP_Init();

  BSP_SENSOR_ACC_SetOutputDataRate(ACC_ODR);
  BSP_SENSOR_ACC_SetFullScale(ACC_FS);
}

/**
  * @brief  Process of the application
  * @retval None
  */
static void MX_DynamicInclinometer_Process(void)
{
	Accelero_Sensor_Handler();
	Gyro_Sensor_Handler();
	//Temperature_Sensor_Handler(&msg_dat);

	DI_Data_Handler();
}

/**
  * @brief  Dynamic Inclinometer data handler
  * @param  Msg the Dynamic Inclinometer data part of the stream
  * @param  Cmd the Dynamic Inclinometer command to GUI
  * @retval None
  */
static void DI_Data_Handler(void)
{
  MDI_input_t      data_in;
  MDI_output_t     data_out;

  if ((SensorsEnabled & ACCELEROMETER_SENSOR) == ACCELEROMETER_SENSOR)
  {
    if ((SensorsEnabled & GYROSCOPE_SENSOR) == GYROSCOPE_SENSOR)
    {
      // Convert acceleration from [mg] to [g]
      data_in.Acc[0] = (float)AccValue.x * FROM_MG_TO_G;
      data_in.Acc[1] = (float)AccValue.y * FROM_MG_TO_G;
      data_in.Acc[2] = (float)AccValue.z * FROM_MG_TO_G;

      // Convert angular velocity from [mdps] to [dps]
      data_in.Gyro[0] = (float)GyrValue.x * FROM_MDPS_TO_DPS;
      data_in.Gyro[1] = (float)GyrValue.y * FROM_MDPS_TO_DPS;
      data_in.Gyro[2] = (float)GyrValue.z * FROM_MDPS_TO_DPS;

      data_in.Timestamp = Timestamp;
      Timestamp += ALGO_PERIOD;

      MotionDI_manager_run(&data_in, &data_out);//Run Dynamic Inclinometer algorithm

      Grotation[0] = data_out.rotation[0];
      Grotation[1] = data_out.rotation[1];
      Grotation[2] = data_out.rotation[2];
      alldata.grad = getSlope(Grotation[1]);
    }
  }
}

/**
  * @brief  Handles the ACC axes data getting/sending
  * @param  Msg the ACC part of the stream
  * @retval None
  */
static void Accelero_Sensor_Handler(void)
{
  if ((SensorsEnabled & ACCELEROMETER_SENSOR) == ACCELEROMETER_SENSOR)
  {
    BSP_SENSOR_ACC_GetAxes(&AccValue);
  }
}

/**
  * @brief  Handles the GYR axes data getting/sending
  * @param  Msg the GYR part of the stream
  * @retval None
  */
static void Gyro_Sensor_Handler(void)
{
  if ((SensorsEnabled & GYROSCOPE_SENSOR) == GYROSCOPE_SENSOR)
  {
    BSP_SENSOR_GYR_GetAxes(&GyrValue);
  }
}

/**
  * @brief  Handles the TEMP axes data getting/sending
  * @param  Msg the TEMP part of the stream
  * @retval None
  */
static void Temperature_Sensor_Handler(void)
{
  if ((SensorsEnabled & TEMPERATURE_SENSOR) == TEMPERATURE_SENSOR)
  {
    BSP_SENSOR_TEMP_GetValue(&TempValue);
  }
}

#ifdef BSP_IP_MEMS_INT1_PIN_NUM
/**
  * @brief  Force MEMS INT1 pin low
  * @param  None
  * @retval None
  */
static void MEMS_INT1_Force_Low(void)
{
	setPinToGenericOutput(BSP_IP_MEMS_INT1_GPIOX, BSP_IP_MEMS_INT1_PIN_NUM);

	LL_GPIO_ResetOutputPin(BSP_IP_MEMS_INT1_GPIOX, BSP_IP_MEMS_INT1_PIN_NUM);
}

/**
  * @brief  Configure MEMS INT1 pin to the default state
  * @param  None
  * @retval None
  */
static void MEMS_INT1_Init(void)
{
	LL_GPIO_SetPinPull(BSP_IP_MEMS_INT1_GPIOX, BSP_IP_MEMS_INT1_PIN_NUM, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinMode(BSP_IP_MEMS_INT1_GPIOX, BSP_IP_MEMS_INT1_PIN_NUM, LL_GPIO_MODE_INPUT);
}
#endif

#ifdef __cplusplus
}
#endif
