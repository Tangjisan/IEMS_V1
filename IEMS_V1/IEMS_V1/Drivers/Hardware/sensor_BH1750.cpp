#include "sensor_BH1750.h"
namespace n_BH1750 {

    // 引用外部的I2C句柄
    I2C_HandleTypeDef& _hi2c = hi2c2;

    const uint8_t _ADDRESS = 0x23   <<  1;          // SGP30 I2C address_ADDRESS
    const uint8_t _deinit_cmd[] = {0x00};           // Example initialization command
    const uint8_t _init_cmd[] = {0x01};           // Example initialization command
    const uint8_t _measure_auto_cmd[] = {0x10};        // Example read command
    const uint8_t _measure_once_cmd[] = {0x20};        // Example read command
    float _lux = 0;         // 存储lux测量值（初始化为0） 
    uint8_t _is_use_driver = 0;     // 硬件使用标志, 0-无新数据, 1-有新数据
    uint8_t _data_flag = 0;     // 数据就绪标志, 0-无新数据, 1-有新数据
    uint8_t _err_cnt = 0;       // 错误计数（未使用）
    
    

    /**
     * 等待I2C总线就绪
     * @return 就绪返回true，超时返回false
     */
    bool _wait_i2c_ready(void){

        uint32_t cnt = 100000;
        while(HAL_I2C_GetState(&_hi2c) != HAL_I2C_STATE_READY && cnt){
            --cnt;
            if(!cnt) return false;
        }
        
        return true;
    }

    namespace n_cmd{
        /**
         * 通过I2C发送命令
         * @return HAL状态（成功/失败）
         */
        inline HAL_StatusTypeDef _init(void){
            return HAL_I2C_Master_Transmit(&_hi2c, _ADDRESS, (uint8_t*)_init_cmd, sizeof(_init_cmd), HAL_MAX_DELAY);
        }
        inline HAL_StatusTypeDef _measure_once(void){
            return HAL_I2C_Master_Transmit(&_hi2c, _ADDRESS, (uint8_t*)_measure_once_cmd, sizeof(_measure_once_cmd), HAL_MAX_DELAY);
        }
        inline HAL_StatusTypeDef _measure_auto(void){
            return HAL_I2C_Master_Transmit(&_hi2c, _ADDRESS, (uint8_t*)_measure_auto_cmd, sizeof(_measure_auto_cmd), HAL_MAX_DELAY);
        }
        inline HAL_StatusTypeDef _receive(uint8_t* buffer_rx, uint16_t buffer_rx_sz){
            return HAL_I2C_Master_Receive(&_hi2c, _ADDRESS, (uint8_t*)buffer_rx, buffer_rx_sz, HAL_MAX_DELAY);
        }
    }
    


    HAL_StatusTypeDef init(void) {
        if(n_get::is_use_driver())  return HAL_BUSY;
        // 初始化状态变量
        HAL_StatusTypeDef err = HAL_OK;

        if(_wait_i2c_ready()){
            err = n_cmd::_init();
            if (err == HAL_OK) 
            HAL_Delay(100); // 等待传感器初始化
        }

        if(_wait_i2c_ready()){
            err = n_cmd::_measure_once();
            if(err == HAL_OK){
                HAL_Delay(100); // 初始先让传感器完成首次检测
            }
        }
        _is_use_driver = 1;
        return err;
    }


    /**
     * 解析数据转为lux值
     * @return 校验成功返回true，失败返回false
     */
    bool check_and_process(uint8_t* data, float& lux){
        uint16_t tmp = (data[0] << 8) | data[1];
        lux = tmp / 1.2f;
        return true;
    }


    bool ReadData(void) {
        if(n_get::is_use_driver() == false) return false;
        // Code to read TVOC and eCO2 data from SGP30 sensor
        uint8_t buffer[2] = {0};
        HAL_StatusTypeDef err;
        if(_wait_i2c_ready())
            n_cmd::_receive(buffer, sizeof(buffer));
        
        
        if(_wait_i2c_ready())
            n_cmd::_measure_once();
        

        // Verify CRC for each data pair
        if(check_and_process(buffer, _lux)){
            _data_flag = 1;
            return true; // Return true if read is successful
        }else{
            _data_flag = 0;
            return false;
        }
        
    }

    namespace n_get{
        float lux(void){
            return _lux;
        }

        bool is_data_ready(void){
            return _data_flag == 1;
        }
        bool is_use_driver(void){
            return _is_use_driver == 1;
        }
    } // namespace n_get
    

    

    namespace n_DMA{
        
        auto& _htim_catchdata = htim3;  // 引用外部的定时器句柄（用于触发数据采集，使用htim3）
        bool _is_use_dma = false;   // DMA使能标志（false：未使用；true：已使用）

        uint8_t* _buffer_rx = nullptr;  // 接收缓冲区指针
        uint8_t _buffer_rx_sz = 2;      // 接收缓冲区大小（固定为6字节，SGP30每次返回6字节数据）
        uint8_t _data_flag = 0;         // DMA数据就绪标志, 0-无新数据, 1-有新数据
        CbState_t _cb_state = CB_STATE_READY; // 状态机：0-空闲，1-发送完成等待，2-接收完成
        
        

        namespace n_cmd{
            /**
             * 通过DMA方式发送命令
             * @return HAL状态（成功/失败）
             */
            inline HAL_StatusTypeDef _measure(void){
                return HAL_I2C_Master_Transmit_DMA(&_hi2c, _ADDRESS, (uint8_t*)_measure_once_cmd, sizeof(_measure_once_cmd));
            }

            inline HAL_StatusTypeDef _receive(void){
                return HAL_I2C_Master_Receive_DMA(&_hi2c, _ADDRESS, (uint8_t*)_buffer_rx, _buffer_rx_sz);
            }
        }

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
            
            if(HAL_TIM_Base_GetState(&_htim_catchdata) != HAL_TIM_STATE_BUSY)
                deinit();
            
            return err;
        }

        HAL_StatusTypeDef deinit(void){
            // DMA反初始化代码
            _is_use_dma = false;
            delete[] _buffer_rx;
            _buffer_rx = nullptr;

            return HAL_OK;
        }

        bool check_and_process_incb(void){
            if(n_get::is_use_dma() == false) return false;
            // Function to check CRC and process data from DMA buffer
            return check_and_process(_buffer_rx, _lux);
        }

        namespace n_cb{
            /**
             * @brief  回调以启动DMA接收
             * @param  None
             * @retval void
             * @note   使用前请确保n_DMA::init()已被调用以启用DMA功能
             * @note   _cb_state : READY -> RECEIVE    && goto I2C_MasterRx
             */
            void receive(void){
                if(n_get::is_use_dma() == false) return;
                // Callback function to handle DMA completion
                if(_cb_state != CB_STATE_READY) return; // 忙碌中，直接返回
                _cb_state = CB_STATE_RECEIVE; // 接收完成
                n_cmd::_receive();
            }
            
            /**
             * @brief  I2C DMA接收完成回调处理函数
             * @param  None
             * @retval void
             * @note   使用前请确保n_DMA::init()已被调用以启用DMA功能
             * @note   _cb_state : RECEIVE -> READY    && goto I2C_MasterTx
             */
            void process_measure(void) {
                if(n_get::is_use_dma() == false) return;
                // I2C DMA reception completion callback
                if(_cb_state != CB_STATE_RECEIVE) return; // 非接收完成状态，直接返回
                _cb_state = CB_STATE_READY; // 重置状态为闲置
                if(check_and_process_incb()) _data_flag = 1; // 数据处理成功，设置标志位
                else    _data_flag = 0; // 数据处理失败，不设置标志位

                // 接收完成，开始下一次发送命令
                n_cmd::_measure();
            }
        } 
        
        namespace n_get{
            bool is_data_ready(void){
                return _data_flag == 1;
            }
            bool is_use_dma(void){
                return _is_use_dma;
            }
        }
    }
}
