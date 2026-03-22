#ifndef __SENSOR_PMS5003_H_
#define __SENSOR_PMS5003_H_

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include <new>
#include <cstring>

namespace n_PMS5003{
    
    typedef struct {
        uint16_t pm1_0;   // PM1.0标准值（μg/m³）
        uint16_t pm2_5;   // PM2.5标准值
        uint16_t pm10;    // PM10标准值
    }data_t;

    typedef enum{
        SEND_STATE_FORCED,
        SEND_STATE_AUTO
    }send_state_t;
    typedef enum{
        RUN_STATE_SLEEP,
        RUN_STATE_NORMAL,
    }run_state_t;

    typedef struct {
        UART_HandleTypeDef* husart     ;
        GPIO_TypeDef*       set_port   ;
        uint16_t            set_pin    ;
        GPIO_TypeDef*       reset_port ;
        uint16_t            reset_pin  ;
        run_state_t         run_state  ;
        send_state_t        send_state ;
    }Config_t;

// extern uint8_t* _buffer_rx;
    void sleep(void);
    void wake(void);
    void reset(void);

    HAL_StatusTypeDef init(Config_t& config);
    HAL_StatusTypeDef deinit(void);

    
    namespace n_forced_mode{
        HAL_StatusTypeDef receive(void);
        HAL_StatusTypeDef use_timmer(TIM_HandleTypeDef* htim);
        namespace n_cb{
            HAL_StatusTypeDef request_receive(void);
            HAL_StatusTypeDef receive(void);
            HAL_StatusTypeDef parse_data(void);
        } // namespace n_cb
    } // namespace n_forced_mode

    namespace n_auto_mode{
        namespace n_cb{
            HAL_StatusTypeDef receive(void);
        } // namespace n_cb
    } // namespace n_auto_mode

    typedef enum {
            CB_STATE_WAITTING,      // 回调等待状态
            CB_STATE_READY,         // 回调就绪状态
            CB_STATE_RECEIVE,       // 接收回调状态
    }CbState_t;
    
    namespace n_get{
        bool is_data_ready(void);
        bool is_use_driver(void);
        uint16_t pm1_0(void);
        uint16_t pm2_5(void);
        uint16_t pm10(void);
    } // namespace n_get

}



#endif 
