#ifndef __TASK_IEMS_V1_H
#define __TASK_IEMS_V1_H

#include "stm32f1xx_hal.h"
#include "OLED.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "sensor_SGP30.h"
#include "sensor_AHT20.h"
#include "sensor_BMP280.h"
#include "sensor_BH1750.h"
#include "sensor_PMS5003.h"
#include "Encoder.h"

#include <stdio.h>
#include <vector>

#ifdef __cplusplus
extern "C"{
#endif


namespace n_task_IEMS_V1{

    enum class page_t{
        PAGE_0 = 0,
        PAGE_1,
        PAGE_2,
        PAGE_3,
        PAGE_4,
        PAGE_5,
    };


    void task(void);

    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

}

#ifdef __cplusplus
}
#endif

#endif
