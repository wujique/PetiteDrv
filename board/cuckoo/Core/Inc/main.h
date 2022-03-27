/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "stm32h7xx_hal.h"

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
#define DCMI_PWDN_Pin GPIO_PIN_3
#define DCMI_PWDN_GPIO_Port GPIOE
#define DCMI_RESET_Pin GPIO_PIN_13
#define DCMI_RESET_GPIO_Port GPIOC
#define VI2C_SDA_Pin GPIO_PIN_5
#define VI2C_SDA_GPIO_Port GPIOA
#define VI2C_SCL_Pin GPIO_PIN_4
#define VI2C_SCL_GPIO_Port GPIOC
#define LCD_RESET_Pin GPIO_PIN_8
#define LCD_RESET_GPIO_Port GPIOD
#define CTP_INT_CLK_Pin GPIO_PIN_9
#define CTP_INT_CLK_GPIO_Port GPIOD
#define CTP_RST_CS_Pin GPIO_PIN_14
#define CTP_RST_CS_GPIO_Port GPIOD
#define LCD_BL_Pin GPIO_PIN_15
#define LCD_BL_GPIO_Port GPIOD
#define IR_CUT_Pin GPIO_PIN_4
#define IR_CUT_GPIO_Port GPIOD
#define EXPOSURE_Pin GPIO_PIN_5
#define EXPOSURE_GPIO_Port GPIOD
#define strobe_led_Pin GPIO_PIN_6
#define strobe_led_GPIO_Port GPIOD
#define sd_card_det_Pin GPIO_PIN_5
#define sd_card_det_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
