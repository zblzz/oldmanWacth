/**
  ******************************************************************************
  * @file    nfc04a1_conf.h
  * @author  SRA Application Team
  * @brief   This file contains definitions for the NFC4 components bus interfaces
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NFC04A1_CONF_H__
#define __NFC04A1_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include "custom_bus.h"
#include "custom_errno.h"

#include "stm32l4xx_hal_exti.h"

#define NFC04A1_I2C_Init         BSP_I2C3_Init
#define NFC04A1_I2C_DeInit       BSP_I2C3_DeInit
#define NFC04A1_I2C_ReadReg16    BSP_I2C3_ReadReg16
#define NFC04A1_I2C_WriteReg16   BSP_I2C3_WriteReg16
#define NFC04A1_I2C_Recv         BSP_I2C3_Recv
#define NFC04A1_I2C_IsReady      BSP_I2C3_IsReady

#define NFC04A1_GetTick          HAL_GetTick

#define NFC04A1_LPD_PIN_PORT GPIOC
#define NFC04A1_LPD_PIN GPIO_PIN_4
#define NFC04A1_GPO_PIN_PORT GPIOC
#define NFC04A1_GPO_PIN GPIO_PIN_5
#define NFC04A1_NFCTAG_GPO_EXTI_LINE EXTI_LINE_5
#define NFC04A1_GPO_EXTI EXTI9_5_IRQn
extern EXTI_HandleTypeDef GPO_EXTI;
#define H_EXTI_5 GPO_EXTI
#define NFC04A1_LED1_PIN_PORT GPIOC
#define NFC04A1_LED1_PIN GPIO_PIN_15
#define NFC04A1_LED2_PIN_PORT GPIOC
#define NFC04A1_LED2_PIN GPIO_PIN_14
#define NFC04A1_LED3_PIN_PORT GPIOC
#define NFC04A1_LED3_PIN GPIO_PIN_13

#define NFC04A1_NFCTAG_INSTANCE         (0)

#define NFC04A1_NFCTAG_GPO_PRIORITY     (0)

#ifdef __cplusplus
}
#endif

#endif /* __NFC04A1_CONF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

