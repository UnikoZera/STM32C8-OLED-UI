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
#include "stm32f1xx_hal.h"

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
extern uint16_t count; // 旋钮数据
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SysWork_Light_Pin GPIO_PIN_13
#define SysWork_Light_GPIO_Port GPIOC
#define Flash_CS_Pin GPIO_PIN_7
#define Flash_CS_GPIO_Port GPIOA
#define Switch_Input_Pin GPIO_PIN_11
#define Switch_Input_GPIO_Port GPIOB
#define Encoder_Input_Pin GPIO_PIN_12
#define Encoder_Input_GPIO_Port GPIOB
#define Flash_SCK_Pin GPIO_PIN_13
#define Flash_SCK_GPIO_Port GPIOB
#define Flash_MISO_Pin GPIO_PIN_14
#define Flash_MISO_GPIO_Port GPIOB
#define Flash_MOSI_Pin GPIO_PIN_15
#define Flash_MOSI_GPIO_Port GPIOB
#define Encoder_CH1_Pin GPIO_PIN_4
#define Encoder_CH1_GPIO_Port GPIOB
#define Encoder_CH2_Pin GPIO_PIN_5
#define Encoder_CH2_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
