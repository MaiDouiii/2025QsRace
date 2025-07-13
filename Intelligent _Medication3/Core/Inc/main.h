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
#include "stm32f4xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY3_Pin GPIO_PIN_4
#define KEY3_GPIO_Port GPIOE
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOF
#define LED0_Pin GPIO_PIN_10
#define LED0_GPIO_Port GPIOF
#define down_dir_Pin GPIO_PIN_12
#define down_dir_GPIO_Port GPIOB
#define down_en_Pin GPIO_PIN_13
#define down_en_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_15
#define LCD_BL_GPIO_Port GPIOB
#define BOTTOM_BACK_Pin GPIO_PIN_8
#define BOTTOM_BACK_GPIO_Port GPIOA
#define BOTTOM_FORWARD_Pin GPIO_PIN_12
#define BOTTOM_FORWARD_GPIO_Port GPIOA
#define TOP_UP_Pin GPIO_PIN_3
#define TOP_UP_GPIO_Port GPIOD
#define TOP_DOWN_Pin GPIO_PIN_7
#define TOP_DOWN_GPIO_Port GPIOD
#define Relay_Pin GPIO_PIN_15
#define Relay_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
