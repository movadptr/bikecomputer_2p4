/**
  *******************************************************************************
  * @file    demo_serial.h
  * @author  MEMS Software Solutions Team
  * @brief   Header for demo_serial.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DEMO_SERIAL_H
#define DEMO_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "bsp_ip_conf.h"
#include "motion_di_manager.h"

/* Enable sensor masks */
#define PRESSURE_SENSOR       0x00000001U
#define TEMPERATURE_SENSOR    0x00000002U
#define HUMIDITY_SENSOR       0x00000004U
#define UV_SENSOR             0x00000008U /* for future use */
#define ACCELEROMETER_SENSOR  0x00000010U
#define GYROSCOPE_SENSOR      0x00000020U
#define MAGNETIC_SENSOR       0x00000040U
#define GAS_SENSOR            0x00000080U

/* Exported variables --------------------------------------------------------*/

extern volatile uint32_t SensorsEnabled;

#ifdef __cplusplus
}
#endif

#endif /* DEMO_SERIAL_H */
