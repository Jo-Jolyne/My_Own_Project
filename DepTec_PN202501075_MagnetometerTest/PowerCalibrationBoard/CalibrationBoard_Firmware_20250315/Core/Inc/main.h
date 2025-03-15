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
#include "stm32f3xx_hal.h"

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
#define LED_RUN_Pin GPIO_PIN_13
#define LED_RUN_GPIO_Port GPIOC
#define MCU_CTRL_DMM_VOL_Pin GPIO_PIN_2
#define MCU_CTRL_DMM_VOL_GPIO_Port GPIOA
#define MCU_CTRL_DMM_CUR_Pin GPIO_PIN_3
#define MCU_CTRL_DMM_CUR_GPIO_Port GPIOA
#define MCU_RL_CTRL_RES1_Pin GPIO_PIN_4
#define MCU_RL_CTRL_RES1_GPIO_Port GPIOA
#define MCU_RL_CTRL_RES2_Pin GPIO_PIN_5
#define MCU_RL_CTRL_RES2_GPIO_Port GPIOA
#define MCU_RL_CTRL_RES3_Pin GPIO_PIN_6
#define MCU_RL_CTRL_RES3_GPIO_Port GPIOA
#define MCU_RL_CTRL_RES4_Pin GPIO_PIN_7
#define MCU_RL_CTRL_RES4_GPIO_Port GPIOA
#define W5500_SPI2_CS_Pin GPIO_PIN_12
#define W5500_SPI2_CS_GPIO_Port GPIOB
#define W5500_SPI2_SCK_Pin GPIO_PIN_13
#define W5500_SPI2_SCK_GPIO_Port GPIOB
#define W5500_SPI2_MISO_Pin GPIO_PIN_14
#define W5500_SPI2_MISO_GPIO_Port GPIOB
#define W5500_SPI2_MOSI_Pin GPIO_PIN_15
#define W5500_SPI2_MOSI_GPIO_Port GPIOB
#define W5500_SPI2_RST_Pin GPIO_PIN_6
#define W5500_SPI2_RST_GPIO_Port GPIOC
#define ADDRESS_KEY_Pin GPIO_PIN_7
#define ADDRESS_KEY_GPIO_Port GPIOC
#define W5500_SPI2_INT_Pin GPIO_PIN_8
#define W5500_SPI2_INT_GPIO_Port GPIOC
#define EEPROM_I2C1_SCL_Pin GPIO_PIN_6
#define EEPROM_I2C1_SCL_GPIO_Port GPIOB
#define EEPROM_I2C1_SDA_Pin GPIO_PIN_7
#define EEPROM_I2C1_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
