/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "bmp2_common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    uint8_t i;
    //int8_t rslt;
    uint32_t meas_time;
    struct bmp2_dev dev;
    struct bmp2_config conf;
    struct bmp2_status status;
    struct bmp2_data comp_data;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FSMC_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
    LCD_Init();
    POINT_COLOR = RED;
    LCD_ShowString(30,50,200,16,16,"STM32F103ZET6");    
    LCD_ShowString(30,70,200,16,16,"BMP280 TEST");    
    LCD_ShowString(30,90,200,16,16,"Luke");
    LCD_ShowString(30,110,200,16,16,"2021/11/01");
    POINT_COLOR = BLUE;//设置字体为蓝色
    LCD_ShowString(30,130,200,16,16,"Temperate: 00.00C");	//先在固定位置显示小数点
    LCD_ShowString(30,150,200,16,16,"Pressure:  0000hPa");	//先在固定位置显示小数点
    bmp2_interface_selection(&dev,&hi2c2);//选择接口总线
    bmp2_init(&dev);//初始化
    bmp2_get_config(&conf, &dev);//在写入之前始终读取当前设置，特别是在未修改所有配置的情况下
    
    //配置过采样模式、滤波器系数和输出数据速率。
    conf.filter = BMP2_FILTER_OFF;// 覆盖所需设置
    conf.os_mode = BMP2_OS_MODE_HIGH_RESOLUTION;//过采样模式设置为高分辨率，即os_pres=8x和os_temp=1x
    conf.odr = BMP2_ODR_250_MS;//设置输出数据速率
    bmp2_set_config(&conf, &dev);
    bmp2_set_power_mode(BMP2_POWERMODE_NORMAL, &conf, &dev);//设置normal电源模式
    bmp2_compute_meas_time(&meas_time, &conf, &dev);//以微秒为单位计算测量时间
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    i++;
    HAL_Delay(10);
    if(i==50)
    {
        bmp2_get_status(&status, &dev);//读取BMP280运行状态
        if (status.measuring == BMP2_MEAS_DONE)
        {
            dev.delay_us(meas_time, dev.intf_ptr);//两次测量间的延时
            bmp2_get_sensor_data(&comp_data, &dev);//读取补偿后的温度和压力
            //显示温度
            if(comp_data.temperature < 0)
            {
                LCD_ShowString(30+10*8,150,16,16,16,"-");//显示负号
            }
            else
            {
                LCD_ShowString(30+10*8,150,16,16,16," ");//无负号
            }
            LCD_ShowxNum(30+11*8,130,comp_data.temperature/100,2,16,0);//显示整数部分
            LCD_ShowxNum(30+14*8,130,comp_data.temperature%100,2,16, 0X80);//显示小数部分
            //显示大气压力
            LCD_ShowxNum(30+11*8,150,comp_data.pressure/100,4,16,0);//显示整数部分
        }
        
        LED0_TOGGLE();//提示系统正在运行
        i=0;
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
