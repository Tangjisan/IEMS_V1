/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for
  *                      the gpio
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define LED0_RESET()    HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET)
#define LED1_RESET()    HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET)
#define LED0_SET()      HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_RESET)
#define LED1_SET()      HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET)
#define LED0_TOGGLE()   HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin)
#define LED1_TOGGLE()   HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin)

#define BEEP_SET()      HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_SET)
#define BEEP_RESET()    HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET)
#define BEEP_TOGGLE()   HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_Pin)

#define KEY0    HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)
#define KEY1    HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)
#define WK_UP   HAL_GPIO_ReadPin(WK_UP_GPIO_Port,WK_UP_Pin)

#define KEY0_PRES 	1	//KEY0按下
#define KEY1_PRES   2	//KEY1按下
#define WKUP_PRES   3	//KEY_UP按下(即WK_UP/KEY_UP)
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
uint8_t KEY_Scan(uint8_t mode);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
