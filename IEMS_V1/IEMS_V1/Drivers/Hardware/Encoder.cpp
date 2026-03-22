#include "Encoder.h"

namespace n_Encoder{
    namespace n_Config{
        volatile uint8_t _is_use_driver = 0;
        
        auto _gpio_A_pin  = _ENCODER_A_Pin;
        auto _gpio_A_port = _ENCODER_A_GPIO_Port;
        auto _gpio_A_exti = _ENCODER_A_EXTI_IRQn;

        auto _gpio_B_pin  = _ENCODER_B_Pin;
        auto _gpio_B_port = _ENCODER_B_GPIO_Port;
        auto _gpio_B_exti = _ENCODER_B_EXTI_IRQn;

        volatile int32_t _count = 0;

        volatile uint32_t _last_interrupt_time = 0;
        const uint16_t DEBOUNCE_TIME_MS = 5;
    } // namespace n_Config
    
    
    // 构造函数
    state_t init(void){
        if (n_Config::_gpio_A_port == nullptr || n_Config::_gpio_B_port == nullptr) {
            return state_t::ERROR;
        }
        if (n_Config::_gpio_A_exti == IRQn_Type::NonMaskableInt_IRQn || 
            n_Config::_gpio_B_exti == IRQn_Type::NonMaskableInt_IRQn) {
            return state_t::ERROR;
        }
        reset_count();
        n_Config::_is_use_driver = 1;
        return state_t::OK;
    }
    state_t deinit(void){
        if(is_use_driver() == 0)
            return state_t::ERROR;
        reset_count();
        n_Config::_is_use_driver = 0;
        return state_t::OK;
    }
    uint8_t is_use_driver(void){
        return n_Config::_is_use_driver;
    }

    void reset_count(void){
        n_Config::_count = 0;
    }

    int32_t get_count(void){
        if(is_use_driver() == 0)
            return 0;
        int32_t tmp = n_Config::_count;
        n_Config::_count = 0;
        return tmp;
    }

    namespace n_cb{
        void EXTI15_10(uint16_t GPIO_Pin){
            if(is_use_driver() == 0)    return;
            uint32_t current_time = HAL_GetTick();
            if((current_time - n_Config::_last_interrupt_time) < n_Config::DEBOUNCE_TIME_MS){
                return;
            }
            n_Config::_last_interrupt_time = current_time;

            if(GPIO_Pin == n_Config::_gpio_A_pin || GPIO_Pin == n_Config::_gpio_B_pin){
                uint8_t level_A = HAL_GPIO_ReadPin(n_Config::_gpio_A_port, n_Config::_gpio_A_pin);
                uint8_t level_B = HAL_GPIO_ReadPin(n_Config::_gpio_B_port, n_Config::_gpio_B_pin);

                if(GPIO_Pin == n_Config::_gpio_A_pin){
                    if(level_A == level_B){
                        n_Config::_count++;
                    }else{
                        n_Config::_count--;
                    }
                }else if(GPIO_Pin == n_Config::_gpio_B_pin){
                    if(level_A != level_B){
                        n_Config::_count++;
                    }else{
                        n_Config::_count--;
                    }
                }
            }
        }
    } // namespace n_cb
    

} // namespace n_Encoder
