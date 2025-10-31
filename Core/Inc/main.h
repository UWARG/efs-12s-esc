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
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DRV_INLC_Pin GPIO_PIN_7
#define DRV_INLC_GPIO_Port GPIOA
#define DRV_INLB_Pin GPIO_PIN_0
#define DRV_INLB_GPIO_Port GPIOB
#define DRV_INLA_Pin GPIO_PIN_1
#define DRV_INLA_GPIO_Port GPIOB
#define LED_1_Pin GPIO_PIN_2
#define LED_1_GPIO_Port GPIOB
#define LED_2_Pin GPIO_PIN_10
#define LED_2_GPIO_Port GPIOB
#define LED_3_Pin GPIO_PIN_11
#define LED_3_GPIO_Port GPIOB
#define DRV_INHC_Pin GPIO_PIN_8
#define DRV_INHC_GPIO_Port GPIOA
#define DRV_INHB_Pin GPIO_PIN_9
#define DRV_INHB_GPIO_Port GPIOA
#define DRV_INHA_Pin GPIO_PIN_10
#define DRV_INHA_GPIO_Port GPIOA
#define DRV_NSS_Pin GPIO_PIN_15
#define DRV_NSS_GPIO_Port GPIOA
#define DRV_EN_Pin GPIO_PIN_12
#define DRV_EN_GPIO_Port GPIOC
#define DRV_nFAULT_Pin GPIO_PIN_8
#define DRV_nFAULT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
