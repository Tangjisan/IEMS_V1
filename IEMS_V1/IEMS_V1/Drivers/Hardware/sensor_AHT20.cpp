#include "sensor_AHT20.h"
namespace n_AHT20 {
    I2C_HandleTypeDef& _hi2c = hi2c2;
    const uint8_t _ADDRESS = 0x38;

    const uint8_t _init_cmd[] = {0xBE, 0x08, 0x00}; // Example initialization command
    const uint8_t _measure_cmd[] = {0xAC, 0x33, 0x00}; // Example enable command
    const uint8_t _reset_cmd[] = {0xBA}; // Example reset command

    const uint8_t _W_ADDRESS = _ADDRESS << 1; // AHT20 I2C address
    const uint8_t _R_ADDRESS = _ADDRESS << 1 | 0x01; // AHT20 I2C address

    float _humidity = 0.0f;  // Initialize humidity to 0.0
    float _temperature = 0.0f;  // Initialize temperature to 0.0

    uint8_t _data_flag = 0; // 状态字节, 1表示数据有效，0表示数据无效

    bool _wait_i2c_ready(void){
            uint32_t cnt = 100000;
            while(HAL_I2C_GetState(&_hi2c) != HAL_I2C_STATE_READY && cnt){
                --cnt;
                if(!cnt) return false;
            }
        
        return true;
    }

    namespace n_cmd{
        // i2c命令
        inline HAL_StatusTypeDef _init(void){
            return HAL_I2C_Master_Transmit(&_hi2c, _W_ADDRESS, (uint8_t*)_init_cmd, sizeof(_init_cmd), HAL_MAX_DELAY);
        }
        inline HAL_StatusTypeDef _measure(void){
            return HAL_I2C_Master_Transmit(&_hi2c, _W_ADDRESS, (uint8_t*)_measure_cmd, sizeof(_measure_cmd), HAL_MAX_DELAY);
        }
        inline HAL_StatusTypeDef _reset(void){
            return HAL_I2C_Master_Transmit(&_hi2c, _W_ADDRESS, (uint8_t*)_reset_cmd, sizeof(_reset_cmd), HAL_MAX_DELAY);
        }
        inline HAL_StatusTypeDef _status(uint8_t& status){
            return HAL_I2C_Master_Receive(&_hi2c, _R_ADDRESS, (uint8_t*)&status, 1, HAL_MAX_DELAY);
        }
        inline HAL_StatusTypeDef _receive(uint8_t* buffer_rx, uint16_t buffer_rx_sz){
            return HAL_I2C_Master_Receive(&_hi2c, _R_ADDRESS, (uint8_t*)buffer_rx, buffer_rx_sz, HAL_MAX_DELAY);
        }
    } // namespace n_cmd
    
    

    HAL_StatusTypeDef init(void) {
        // Initialization code for AHT20 sensor

        // Use _hi2c for I2C communication
        HAL_Delay(40); // Wait for sensor to power up
        uint8_t status = 0;
        auto err = HAL_OK;
        if(_wait_i2c_ready())
            n_cmd::_status(status);
        HAL_Delay(10);
        if ((status & 1 << 3) != 0) {
            if(_wait_i2c_ready())
                n_cmd::_init();
        }
        if(_wait_i2c_ready())
            n_cmd::_measure();  // 初始化先请求一次检测数据
            
        return err;
    }


    bool check_and_process(uint8_t* buffer_rx, float &humidity, float &temperature){
        // Function to check and process data
        if(buffer_rx[0] & 1<<7){
            _data_flag = 0; // 数据无效
            return false;
        }
        _data_flag = 1; // 数据有效
        uint32_t Srh = (
            ((uint32_t)buffer_rx[1] << 12) |         // 湿度原始值高位
            ((uint32_t)buffer_rx[2] << 4) |          // 湿度原始值中位
            ((uint32_t)buffer_rx[3] >> 4));          // 湿度原始值
        uint32_t St = (
            (((uint32_t)buffer_rx[3] & 0x0F) << 16) |   // 温度原始值高位
            ((uint32_t)buffer_rx[4] << 8) |             // 温度原始值中位
            ((uint32_t)buffer_rx[5]));                  // 温度原始值
        // Use Srh and St as needed

        humidity = (Srh / 1048576.0f) * 100; // 转换为0.1%RH单位
        temperature = (St / 1048576.0f) * 200 - 50; // 转换为0.1摄氏度单位
        return true;
    }

    

    bool readData(void) {
        // Code to read TVOC and eCO2 data from SGP30 sensor
        HAL_StatusTypeDef err;

        uint8_t buffer_rx[6] = {0};
        if(_wait_i2c_ready())
            n_cmd::_receive(buffer_rx, sizeof(buffer_rx));

        _data_flag = check_and_process(buffer_rx, _humidity, _temperature); // 数据有效,开启下一次测量
        
        if(_wait_i2c_ready())
            n_cmd::_measure();  // 75ms 后测量完成,在主函数500ms间隔读取一次
        else
            return false;
        return true;
    }

    bool reset(void){
        // Code to reset AHT20 sensor
        HAL_StatusTypeDef err;
        n_cmd::_reset();
        if (err != HAL_OK) return false;
        HAL_Delay(20); // 等待20ms复位完成
        return true;
    }

    namespace n_get{
        float humidity(void){
            return _humidity;
        }
        float temperature(void){
            return _temperature;
        }

        bool is_data_ready(void){
            return _data_flag == 1;
        }

    } // namespace n_get

    
    namespace n_DMA{
        // auto& htim_wait10 = htim2;
        auto& _htim_catchdata = htim3;
        bool _is_use_dma = false;
        // auto onepulse_channel = TIM_CHANNEL_1;

        uint8_t* _buffer_rx = nullptr;
        uint16_t _buffer_rx_sz = 6;
        uint8_t _data_flag = 0;
        uint8_t _err_cnt = 0;
        CbState_t _cb_status = CB_STATE_READY;

        namespace n_cmd{
           inline HAL_StatusTypeDef _measure(void){
                return HAL_I2C_Master_Transmit_DMA(&_hi2c, _W_ADDRESS, (uint8_t*)_measure_cmd, sizeof(_measure_cmd));
            }
            inline HAL_StatusTypeDef _reset(void){
                return HAL_I2C_Master_Transmit_DMA(&_hi2c, _W_ADDRESS, (uint8_t*)_reset_cmd, sizeof(_reset_cmd));
            }
            inline HAL_StatusTypeDef _receive(void){
                return HAL_I2C_Master_Receive_DMA(&_hi2c, _R_ADDRESS, (uint8_t*)_buffer_rx, _buffer_rx_sz);
            } 
        } // namespace n_cmd
        

        /**
         * @brief  初始化DMA相关设置
         * @param  None
         * @retval HAL_TIM_Base_Start_IT结果
         */
        HAL_StatusTypeDef init(void){
            HAL_StatusTypeDef err = HAL_OK;
            // DMA初始化代码
            _is_use_dma = true;
            _buffer_rx = new (std::nothrow) uint8_t[_buffer_rx_sz]();

            if(_buffer_rx == nullptr){
                deinit();
                return HAL_ERROR;
            }
            
            if(HAL_TIM_Base_GetState(&_htim_catchdata) == HAL_TIM_STATE_READY)
                err = HAL_TIM_Base_Start_IT(&_htim_catchdata);
            
            if(HAL_TIM_Base_GetState(&_htim_catchdata) != HAL_TIM_STATE_BUSY){
                deinit();
                return HAL_ERROR;
            }
                
            
            return err;
        }

        HAL_StatusTypeDef deinit(void){
            // DMA反初始化代码
            _is_use_dma = false;
            delete[] _buffer_rx;
            _buffer_rx = nullptr;

            return HAL_OK;
        }

        /**
         * @brief  将DMA接收的数据进行校验并处理
         * @param  None
         * @retval bool 校验并处理结果，true表示成功，false表示失败
         * @note   使用前请确保n_DMA::init()已被调用以启用DMA功能
         */
        bool check_and_process(void){
            if(!_is_use_dma) return false;
            // Function to check CRC and process data from DMA buffer
            return n_AHT20::check_and_process(_buffer_rx, _humidity, _temperature);
            // Use tvoc and eco2 as needed
        }

        namespace n_cb{
            /**
             * @brief  回调以启动DMA接收
             * @param  None
             * @retval void
             * @note   使用前请确保n_DMA::init()已被调用以启用DMA功能
             * @note   _cb_status : READY -> RECEIVE    && goto I2C_MasterRx
             */
            void receive(void){
                if(!_is_use_dma) return;
                // Callback function to handle DMA completion
                if(_cb_status != CB_STATE_READY) return; // 忙碌中，直接返回

                n_cmd::_receive();   // 接收上一次的数据
                
                _cb_status = CB_STATE_RECEIVE; // 标记为发送完成等待状态
            }
            
            /**
             * @brief  I2C DMA接收完成回调处理函数
             * @param  None
             * @retval void
             * @note   使用前请确保n_DMA::init()已被调用以启用DMA功能
             * @note   _cb_status : RECEIVE -> READY    && goto I2C_MasterTx
             * @note   _cb_status : RECEIVE -> RESET    && goto I2C_MasterTx    // 错误五次后
             */
            void process_measure(void){
                if(!_is_use_dma) return;
                // I2C DMA reception completion callback
                if(_cb_status != CB_STATE_RECEIVE) return; // 非接收完成状态，直接返回
    
                if(check_and_process()){
                    _data_flag = 1; // 通知主循环处理数据;
                    _err_cnt = 0;
                    n_cmd::_measure();   // 发送下一次测量命令
                    _cb_status = CB_STATE_READY; // 重置状态为闲置
                }else{
                    _data_flag = 0;
                    _err_cnt++;
                    if(_err_cnt >= 5){
                        // 连续5次错误，重置传感器
                        n_cmd::_reset();
                        _err_cnt = 0;
                        _cb_status = CB_STATE_RESET;
                    }
                }    
                
                
            }

            /**
             * @brief  回调处理,表示RESET命令发送完成，返回READY状态
             * @param  None
             * @retval void
             * @note   使用前请确保n_DMA::init()已被调用以启用DMA功能
             * @note   _cb_status : RESET -> READY
             */
            void reset(void){
                if(!_is_use_dma) return;
                if(_cb_status != CB_STATE_RESET)   return;

                _cb_status = CB_STATE_READY;
            }
        }

        namespace n_get{
            bool is_data_ready(void){
                return _data_flag == 1;
            }
            bool is_use_dma(void){
                return _is_use_dma == true;
            }
        } // namespace n_get
        
       
    
        }

        

}




