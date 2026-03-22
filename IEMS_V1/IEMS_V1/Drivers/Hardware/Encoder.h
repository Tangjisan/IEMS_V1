#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "stm32f1xx_hal.h"
#include "gpio.h"

namespace n_Encoder{
    enum class state_t{
        OK,
        ERROR,
    };

    state_t init(void);
    state_t deinit(void);
    uint8_t is_use_driver(void);
    void reset_count(void);
    int32_t get_count(void);
    
    namespace n_cb{
        void EXTI15_10(uint16_t GPIO_Pin);
    } // namespace n_cb
} // namespace n_Encoder

#endif // __ENCODER_H_