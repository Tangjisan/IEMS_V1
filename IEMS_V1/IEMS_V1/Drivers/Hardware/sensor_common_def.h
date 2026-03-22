#ifndef __SENSOR_COMMON_DEF_H_
#define __SENSOR_COMMON_DEF_H_

#include "OLED.h"
#include "stm32f1xx_hal.h"

#define HAL_CHECK_STATUS(rslt, msg)      \
    do{                                  \
        HAL_StatusTypeDef err = rslt;    \
        if (err != HAL_OK) {             \
            OLED_ShowString(4, 1, msg);  \
            return err;                  \
        }                                \
    }while(0)                            


void SWO_Init(void);

#endif
