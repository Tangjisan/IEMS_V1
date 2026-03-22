#include "Task_IEMS_V1.h"

#define p5switch HAL_GPIO_TogglePin(LED_P5_GPIO_Port, LED_P5_Pin)   // 切换页面时LED闪烁提示

namespace n_task_IEMS_V1{

    int16_t encoder_count = 0;
    page_t current_page = page_t::PAGE_0;
    page_t last_page = current_page;

    auto& _huart = huart2;
    auto& _htim_catchdata = htim3;

    void limit_count(int16_t& count){
        constexpr int16_t res = 10;

        constexpr int16_t PAGE_MIN = static_cast<int16_t>(page_t::PAGE_0);
        constexpr int16_t PAGE_MAX = static_cast<int16_t>(page_t::PAGE_5);
        constexpr int16_t PAGE_COUNT = PAGE_MAX - PAGE_MIN + 1; // 页面总数

        constexpr int16_t count_limit_count = PAGE_COUNT * res;
        constexpr int16_t count_limit_min = PAGE_MIN * res;

        count = (count + count_limit_count - count_limit_min) % count_limit_count + count_limit_min;
    }

    void loop_page_change(page_t& page, int16_t& count){
        constexpr int16_t res = 10;
        int16_t page_tmp = count / res;
        page = static_cast<page_t>(page_tmp);
    }

    namespace n_print_AHT20{
        // AHT20 sensor data
        void print_title(void){
            OLED_ShowString(1, 8, "AHT20");
            OLED_ShowString(2, 1, "HUMI:");
            OLED_ShowString(3, 1, "TEMP:");
            OLED_ShowString(4, 1, "                ");
        }
        void print_data(void){
            if(n_AHT20::n_get::is_data_ready()){
                float humi = n_AHT20::n_get::humidity();
                float temp = n_AHT20::n_get::temperature();
                OLED_ShowNum(2, 6, humi * 100, 10);
                OLED_ShowNum(3, 6, temp * 100, 10);
            }else{
                OLED_ShowString(2, 6, "err");
                OLED_ShowString(3, 6, "err");
            }
        }
    }

    namespace n_print_BH1750{
        // BH1750 sensor data
        void print_title(void){
            OLED_ShowString(1, 8, "BH1750");
            OLED_ShowString(2, 1, "LUX:");
            OLED_ShowString(3, 1, "                ");
            OLED_ShowString(4, 1, "                ");
        }
        void print_data(void){
            if(n_BH1750::n_get::is_data_ready()){
                float lux = n_BH1750::n_get::lux();
                OLED_ShowNum(2, 6, lux * 100, 10);
            }else{
                OLED_ShowString(2, 6, "err");
            }
        }
    }

    namespace n_print_BMP280{
        // BMP280 sensor data
        void print_title(void){
            OLED_ShowString(1, 8, "BMP280");
            OLED_ShowString(2, 1, "PRES:");
            OLED_ShowString(3, 1, "TEMP:");
            OLED_ShowString(4, 1, "                ");
        }
        void print_data(void){
            if(n_AHT20::n_get::is_data_ready()){
                float pres = n_BMP280::n_get::pressure();
                float temp = n_BMP280::n_get::temperature();
                OLED_ShowNum(2, 6, pres * 100, 10);
                OLED_ShowNum(3, 6, temp * 100, 10);
            }else{
                OLED_ShowString(2, 6, "err");
                OLED_ShowString(3, 6, "err");
            }
        }
    }

    namespace n_print_SGP30{
        // SGP30 sensor data
        void print_title(void){
            OLED_ShowString(1, 8, "SGP30");
            OLED_ShowString(2, 1, "eCO2:");
            OLED_ShowString(3, 1, "TVOC:");
            OLED_ShowString(4, 1, "                ");
        }
        void print_data(void){
            if(n_SGP30::n_get::is_data_ready()){
                float eco2 = n_SGP30::n_get::eco2();
                float tvoc = n_SGP30::n_get::tvoc();
                OLED_ShowNum(2, 6, eco2 * 100, 10);
                OLED_ShowNum(3, 6, tvoc * 100, 10);
            }else{
                OLED_ShowString(2, 7, "err");
                OLED_ShowString(3, 7, "err");
            }
        }
    }

    namespace n_print_PMS5003{
        // PMS5003 sensor data
        void print_title(void){
            OLED_ShowString(1, 8, "PMS5003");
            OLED_ShowString(2, 1, "PM1.0:");
            OLED_ShowString(3, 1, "PM2.5:");
            OLED_ShowString(4, 1, "PM10 :");
        }
        void print_data(void){
            if(n_PMS5003::n_get::is_data_ready()){
                float PM1_0 = n_PMS5003::n_get::pm1_0();
                float PM2_5 = n_PMS5003::n_get::pm2_5();
                float PM10  = n_PMS5003::n_get::pm10();
                OLED_ShowNum(2, 6, PM1_0 * 100, 5);
                OLED_ShowNum(3, 6, PM2_5 * 100, 5);
                OLED_ShowNum(4, 6, PM10  * 100, 5);
            }else{
                OLED_ShowString(2, 6, "err");
                OLED_ShowString(3, 6, "err");
                OLED_ShowString(4, 6, "err");
            }
        }
    }
    

    void print_title(page_t page){
        OLED_Clear();
        switch (page){
            case page_t::PAGE_0:
                OLED_ShowString(1, 1, "Page 0");
                OLED_ShowString(3, 5, "IEMS V1");
                break;
            case page_t::PAGE_1:
                OLED_ShowString(1, 1, "Page 1");
                n_print_AHT20::print_title();
                break;
            case page_t::PAGE_2:
                OLED_ShowString(1, 1, "Page 2");
                n_print_BH1750::print_title();
                break;
            case page_t::PAGE_3:
                OLED_ShowString(1, 1, "Page 3");
                n_print_BMP280::print_title();
                break;
            case page_t::PAGE_4:
                OLED_ShowString(1, 1, "Page 4");
                n_print_SGP30::print_title();
                break;
            case page_t::PAGE_5:
                OLED_ShowString(1, 1, "Page 5");
                n_print_PMS5003::print_title();
                break;
            default:
                break;
        }
    }

    void print_data(page_t page){
        switch (page){
            case page_t::PAGE_0:
                break;
            case page_t::PAGE_1:
                n_print_AHT20::print_data();
                break;
            case page_t::PAGE_2:
                n_print_BH1750::print_data();
                break;
            case page_t::PAGE_3:
                n_print_BMP280::print_data();
                break;
            case page_t::PAGE_4:
                n_print_SGP30::print_data();
                break;
            case page_t::PAGE_5:
                n_print_PMS5003::print_data();
                break;
            default:
                break;
        }
    }
 
    void task_init(void){
        OLED_Init();
        n_Encoder::init();
        n_AHT20::init();
        n_BH1750::init();
        n_BMP280::init();
        n_SGP30::init();
        
        n_PMS5003::Config_t PMS5003_config = {
            .husart     = &_huart,
            .set_port   = _PMS5003_SET_GPIO_Port,
            .set_pin    = _PMS5003_SET_Pin,
            .reset_port = _PMS5003_RESET_GPIO_Port,
            .reset_pin  = _PMS5003_RESET_Pin,
            .run_state  = n_PMS5003::RUN_STATE_NORMAL,
            .send_state = n_PMS5003::SEND_STATE_FORCED,
        };
        n_PMS5003::init(PMS5003_config);
        n_PMS5003::n_forced_mode::use_timmer(&_htim_catchdata);

    }

    void task_loop(void){
        n_AHT20::readData();
        n_BH1750::ReadData();
        n_BMP280::readdata();
        n_SGP30::readData();

        
        print_data(current_page);
    }

    void task(void){
        // Task implementation
        
        task_init();
        print_title(current_page);
        

        while (1) {
            // 更改页面
            encoder_count += n_Encoder::get_count();
            limit_count(encoder_count);
            loop_page_change(current_page, encoder_count);
            
            // 页面切换时清屏
            if(current_page != last_page){
                last_page = current_page;
                print_title(current_page);

            }

            // Task loop
            task_loop();

            HAL_Delay(50); // 延时1秒
        }
    }

    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
        n_Encoder::n_cb::EXTI15_10(GPIO_Pin);
    }

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
        if (htim == &_htim_catchdata) {
            n_PMS5003::n_forced_mode::n_cb::request_receive();
        }
    }

    void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
        if (huart == &_huart) {
            n_PMS5003::n_forced_mode::n_cb::receive();
        }
    }
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
        if (huart == &_huart) {
            // n_PMS5003::n_auto_mode::n_cb::receive();
            n_PMS5003::n_forced_mode::n_cb::parse_data();
        }
    }

}

