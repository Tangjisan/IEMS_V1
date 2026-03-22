#include "sensor_BMP280.h"
#include "sensor_common_def.h"

namespace n_BMP280 {

    // 引用外部的I2C句柄
    I2C_HandleTypeDef& _hi2c = hi2c2;

    uint8_t _is_use_driver = 0;     // 硬件使用标志, 0-无新数据, 1-有新数据
    uint8_t _data_flag = 0;     // 数据就绪标志, 0-无新数据, 1-有新数据
    uint8_t _err_cnt = 0;       // 错误计数（未使用）
    power_mode_t _power_mode_flag = POWER_MODE_FORCED;   // 运行模式标志，0-强制模式，1-正常模式

    uint8_t _reset_cmd[1] = {BMP2_SOFT_RESET_CMD};
    uint8_t _force_cmd[1] = {BMP2_POWERMODE_FORCED};
    
    struct bmp2_dev _dev = {0};     // 定义BMP280设备结构体
    struct bmp2_config _config = {0};   // 定义设备配置结构体
    struct bmp2_data _data = {0};   // 用于储存获取的数据
    

    /**
     * 等待I2C总线就绪
     * @return 就绪返回true，超时返回false
     */
    bool _wait_i2c_ready(void){

        uint32_t cnt = 100;
        while(HAL_I2C_GetState(&_hi2c) != HAL_I2C_STATE_READY && cnt){
            --cnt;
            HAL_Delay(1);
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
            if(bmp2_interface_selection(&_dev, (void*)&_hi2c) != BMP2_OK)  return HAL_ERROR;
            if(bmp2_init(&_dev) != BMP2_OK)     return HAL_ERROR;
            return HAL_OK;
        }
        inline HAL_StatusTypeDef _measure(void){
            if(bmp2_set_power_mode(BMP2_POWERMODE_FORCED, &_config, &_dev) != BMP2_OK)  return HAL_ERROR;
            return HAL_OK;
        }

        inline HAL_StatusTypeDef _receive_data(void){
            if(bmp2_get_sensor_data(&_data, &_dev) != BMP2_OK)  return HAL_ERROR;
            return HAL_OK;
        }
        inline HAL_StatusTypeDef _get_power_mode(uint8_t* mode){
            if(bmp2_get_power_mode(mode, &_dev) != BMP2_OK) return HAL_ERROR;
            return HAL_OK; 
        }
    }
    
    HAL_StatusTypeDef init(void){
        if(n_get::is_use_driver())  return HAL_BUSY;
        // 初始化状态变量
        HAL_StatusTypeDef err = HAL_OK;
        int8_t bmp2err = BMP2_OK;

        _wait_i2c_ready();
        n_cmd::_init();

        // 配置测量参数
        _wait_i2c_ready();
        bmp2err = bmp2_get_config(&_config, &_dev);  // 获取当前配置
        err = (bmp2err == BMP2_OK ? HAL_OK : HAL_ERROR);

        // 设置温度和压力采样率（例如：超高频采样）
            _config.filter = BMP2_FILTER_COEFF_16;  // 滤波系数：16（平滑输出）
            _config.odr = BMP2_ODR_500_MS;   // 输出数据率：1000ms（1Hz更新）
            _config.os_mode = BMP2_OS_MODE_STANDARD_RESOLUTION;
            _wait_i2c_ready();
        if(bmp2_set_config(&_config, &_dev) != BMP2_OK)  // 应用配置
            return HAL_ERROR;

        _wait_i2c_ready();
        // 设置为睡眠模式（按需设置强制模式）
        bmp2err = bmp2_set_power_mode(BMP2_POWERMODE_SLEEP, &_config, &_dev);
        err = (bmp2err == BMP2_OK ? HAL_OK : HAL_ERROR);
        _power_mode_flag = POWER_MODE_FORCED;

        _is_use_driver = 1;
        return err;
    }

    HAL_StatusTypeDef set_normal_mode(void){
        if(n_get::is_use_driver() == false)     return HAL_ERROR;
        if(bmp2_set_power_mode(BMP2_POWERMODE_NORMAL, &_config, &_dev) != BMP2_OK)  return HAL_ERROR;
        _power_mode_flag = POWER_MODE_NORMAL;
        return HAL_OK;
    }

    HAL_StatusTypeDef set_sleep_mode(void){
        if(n_get::is_use_driver() == false)     return HAL_ERROR;
        if(bmp2_set_power_mode(BMP2_POWERMODE_SLEEP, &_config, &_dev) != BMP2_OK)  return HAL_ERROR;
        _power_mode_flag = POWER_MODE_FORCED;
        return HAL_OK;
    }

    HAL_StatusTypeDef measure(void){
        if(n_get::is_use_driver() == false) return HAL_ERROR;

        if(n_cmd::_measure() != HAL_OK) return HAL_ERROR;
        return HAL_OK;
    } 


    HAL_StatusTypeDef readdata(void) {
        if(n_get::is_use_driver() == false) return HAL_ERROR;
        
        if(n_cmd::_receive_data() == HAL_OK)
            _data_flag = 1;
        else
            _data_flag = 0;
        
        return HAL_OK;
    }

    HAL_StatusTypeDef getpowermode(uint8_t* mode){
        if(n_get::is_use_driver() == false) return HAL_ERROR;
        
        n_cmd::_get_power_mode(mode);
        return HAL_OK;
    }

    namespace n_get{
        double pressure(void){
            return _data.pressure;
        }
        
        double temperature(void){
            return _data.temperature;
        }

        bool is_data_ready(void){
            return _data_flag == 1;
        }
        bool is_use_driver(void){
            return _is_use_driver == 1;
        }
        uint8_t get_power_mode(void){
            return _power_mode_flag;
        }
    } // namespace n_get
    


    
    namespace n_DMA{
        
        auto& _htim_catchdata = htim3;  // 引用外部的定时器句柄（用于触发数据采集，使用htim3）
        bool _is_use_dma = false;   // DMA使能标志（false：未使用；true：已使用）

        uint8_t _data_flag = 0;         // DMA数据就绪标志, 0-无新数据, 1-有新数据
        CbState_t _cb_state = CB_STATE_READY; // 状态机：0-空闲，1-发送完成等待，2-接收完成

        uint8_t _ctrl_meas = 0;
        uint8_t _status = 0;
        uint8_t _data_buff[6] = {0};
        bmp2_uncomp_data _uncomp_data = {0};
        bmp2_data _comp_data = {0};

        static void set_os_mode(uint8_t *reg_data, const struct bmp2_config *conf);
        static int8_t parse_sensor_data(const uint8_t *reg_data, struct bmp2_uncomp_data *uncomp_data);
        static int8_t st_check_boundaries(int32_t utemperature, int32_t upressure);

        void _add_osrs_cmd(void){
            set_os_mode(_force_cmd, &_config);
        }
        namespace n_cmd{
            /**
             * 通过DMA方式发送命令
             * @return HAL状态（成功/失败）
             */
            // inline HAL_StatusTypeDef _init(void){
            //     if(bmp2_interface_selection_DMA(&_dev, (void*)&_hi2c) != BMP2_OK)  return HAL_ERROR;
            //     return HAL_OK;
            // }
            inline HAL_StatusTypeDef _measure(void){
                _add_osrs_cmd();
                auto&& rslt = HAL_I2C_Mem_Write_DMA(&_hi2c, BMP280_I2C_ADDR, BMP2_REG_CTRL_MEAS, 1, _force_cmd, 1);
                _force_cmd[0] = BMP2_POWERMODE_FORCED;
                HAL_CHECK_STATUS(rslt, "n_BMP280::n_DMA::n_cmd::_measure err");
                _ctrl_meas = 0;
                // if(_dev.write(BMP2_REG_CTRL_MEAS, &_ctrl_meas, 1, (void*)&_hi2c) != BMP2_OK)  return HAL_ERROR;
                return HAL_OK;
            }
            inline HAL_StatusTypeDef _reset(void){
                auto&& rslt = HAL_I2C_Mem_Write_DMA(&_hi2c, BMP280_I2C_ADDR, BMP2_REG_SOFT_RESET, 1, _reset_cmd, 1);
                HAL_CHECK_STATUS(rslt, "n_BMP280::n_DMA::n_cmd::_reset err");
                return HAL_OK; 
            }
            inline HAL_StatusTypeDef _receive_data(void){
                auto&& rslt = HAL_I2C_Mem_Read_DMA(&_hi2c, BMP280_I2C_ADDR, BMP2_REG_PRES_MSB, 1, _data_buff, 6);
                HAL_CHECK_STATUS(rslt, "n_BMP280::n_DMA::n_cmd::_receive_data err");
                // if(_dev.read(BMP2_REG_PRES_MSB, _data, 6, (void*)&_hi2c) != BMP2_OK)  return HAL_ERROR;
                return HAL_OK;
            }
            inline HAL_StatusTypeDef _receive_status(void){
                auto&& rslt = HAL_I2C_Mem_Read_DMA(&_hi2c, BMP280_I2C_ADDR, BMP2_REG_STATUS, 1, &_status, 1);
                HAL_CHECK_STATUS(rslt, "n_BMP280::n_DMA::n_cmd::_receive_status err");
                // if(_dev.read(BMP2_REG_STATUS, &_status, 1, (void*)&_hi2c) != BMP2_OK)  return HAL_ERROR;
                return HAL_OK;
            }
        }

        HAL_StatusTypeDef init(void){
            HAL_StatusTypeDef err = HAL_OK;

            // DMA初始化代码
            _is_use_dma = true;
            
            if(HAL_TIM_Base_GetState(&_htim_catchdata) == HAL_TIM_STATE_READY)
                err = HAL_TIM_Base_Start_IT(&_htim_catchdata);
            
            if(HAL_TIM_Base_GetState(&_htim_catchdata) != HAL_TIM_STATE_BUSY)
                deinit();
            
            return err;
        }

        HAL_StatusTypeDef deinit(void){
            // DMA反初始化代码
            _is_use_dma = false;

            return HAL_OK;
        }
        

        

        HAL_StatusTypeDef handle_data(void){
            HAL_StatusTypeDef rslt;
            
            if(parse_sensor_data(_data_buff, &_uncomp_data) != BMP2_OK) return HAL_ERROR;
            if(bmp2_compensate_data(&_uncomp_data, &_comp_data, &_dev) != BMP2_OK)  return HAL_ERROR;
            return HAL_OK;
        }

        namespace n_cb{
            /**
             * @brief  回调以启动DMA接收
             * @param  None
             * @retval void
             * @note   使用前请确保n_DMA::init()已被调用以启用DMA功能
             * @note   _cb_state : READY -> RECEIVE    && goto I2C_MemRx
             * @note   normal模式不需其余调用 / force模式需调用measure回调
             */
            void receive(void){
                if(n_get::is_use_dma() == false) return;

                HAL_StatusTypeDef err;
                // Callback function to handle DMA completion

                if(n_get::get_power_mode() == POWER_MODE_FORCED){
                    if(_cb_state != CB_STATE_READY) return; // 忙碌中，直接返回

                    _cb_state = CB_STATE_RECEIVE; // 接收完成
                    

                }
                else if(n_get::get_power_mode() == POWER_MODE_NORMAL){
                    _data_flag = 1;
                }

                if(handle_data() != HAL_OK)         _data_flag = 0;
                if(n_cmd::_receive_data() != HAL_OK)    _data_flag = 0;
                
            }
            
            /**
             * @brief  I2C DMA接收完成回调处理函数
             * @param  None
             * @retval void
             * @note   使用前请确保n_DMA::init()已被调用以启用DMA功能
             * @note   _cb_state : RECEIVE -> READY    && goto I2C_MemTx
             */
            void measure(void) {
                if(_is_use_dma == false)                            return;
                // I2C DMA reception completion callback
                if(_cb_state != CB_STATE_RECEIVE)                   return; // 非接收完成状态，直接返回
                if(n_get::get_power_mode() != POWER_MODE_FORCED)    return;


                _cb_state = CB_STATE_READY; // 接收完成

                

                if(n_cmd::_measure() != HAL_OK)     _data_flag = 0;
                else                                _data_flag = 1;
                
            }
            
        } 
        
        namespace n_get{
            bool is_data_ready(void){
                return _data_flag == 1;
            }
            bool is_use_dma(void){
                return _is_use_dma;
            }
            bool is_driver_waitting(void){
                return (_status & 1 << 3) ? true : false;
            }
            power_mode_t get_power_mode(void){
                return _power_mode_flag;
            }
            double pressure(void){
                return _comp_data.pressure;
                
            }
            double temperature(void){
                return _comp_data.temperature;
            }
        }

        /*!
        *  @brief 本接口用于根据过采样模式设置温度和压力的过采样率
        */
        static void set_os_mode(uint8_t *reg_data, const struct bmp2_config *conf)
        {
            switch (conf->os_mode)
            {
                case BMP2_OS_MODE_ULTRA_LOW_POWER:
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_TEMP, BMP2_OS_1X);//温度过采样率
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_PRES, BMP2_OS_1X);//压力过采样率
                    break;
                case BMP2_OS_MODE_LOW_POWER:
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_TEMP, BMP2_OS_1X);//温度过采样率
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_PRES, BMP2_OS_2X);//压力过采样率
                    break;
                case BMP2_OS_MODE_STANDARD_RESOLUTION:
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_TEMP, BMP2_OS_1X);//温度过采样率
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_PRES, BMP2_OS_4X);//压力过采样率
                    break;
                case BMP2_OS_MODE_HIGH_RESOLUTION:
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_TEMP, BMP2_OS_1X);//温度过采样率
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_PRES, BMP2_OS_8X);//压力过采样率
                    break;
                case BMP2_OS_MODE_ULTRA_HIGH_RESOLUTION:
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_TEMP, BMP2_OS_2X);//温度过采样率
                    reg_data[0] = BMP2_SET_BITS(reg_data[0], BMP2_OS_PRES, BMP2_OS_16X);//压力过采样率
                    break;
                default:
                    break;
            }
        }
        /*!
        *  @brief 此内部API用于解析压力和温度数据，并将其存储在bmp2_uncomp_data结构实例中。
        */
        static int8_t parse_sensor_data(const uint8_t *reg_data, struct bmp2_uncomp_data *uncomp_data)
        {
            int8_t rslt;

            /* 用于存储传感器数据的变量 */
            uint32_t data_xlsb;
            uint32_t data_lsb;
            uint32_t data_msb;

            /* 存储压力数据的解析寄存器值 */
            data_msb = (uint32_t)reg_data[0] << 12;
            data_lsb = (uint32_t)reg_data[1] << 4;
            data_xlsb = (uint32_t)reg_data[2] >> 4;
            uncomp_data->pressure = data_msb | data_lsb | data_xlsb;

            /* 存储温度数据的解析寄存器值 */
            data_msb = (int32_t)reg_data[3] << 12;
            data_lsb = (int32_t)reg_data[4] << 4;
            data_xlsb = (int32_t)reg_data[5] >> 4;
            uncomp_data->temperature = (int32_t)(data_msb | data_lsb | data_xlsb);

            /* 检查未补偿温度和未补偿压力是否在范围内 */
            rslt = st_check_boundaries((int32_t)uncomp_data->temperature, (int32_t)uncomp_data->pressure);

            return rslt;
        }
        /*!
        * @This 内部接口检查未补偿温度和未补偿压力是否在范围内
        */
        static int8_t st_check_boundaries(int32_t utemperature, int32_t upressure)
        {
            int8_t rslt = 0;

            /* 检查无补偿压力在无效范围内，无补偿温度在有效范围内 */
            if ((upressure < BMP2_ST_ADC_P_MIN || upressure > BMP2_ST_ADC_P_MAX) &&
                (utemperature >= BMP2_ST_ADC_T_MIN && utemperature <= BMP2_ST_ADC_T_MAX))
            {
                rslt = BMP2_E_UNCOMP_PRESS_RANGE;
            }
            /* 检查未补偿温度在无效范围内，未补偿压力在有效范围内 */
            else if ((utemperature < BMP2_ST_ADC_T_MIN || utemperature > BMP2_ST_ADC_T_MAX) &&
                    (upressure >= BMP2_ST_ADC_P_MIN && upressure <= BMP2_ST_ADC_P_MAX))
            {
                rslt = BMP2_E_UNCOMP_TEMP_RANGE;
            }
            /* 检查未补偿压力在无效范围内，未补偿温度在无效范围内 */
            else if ((upressure < BMP2_ST_ADC_P_MIN || upressure > BMP2_ST_ADC_P_MAX) &&
                    (utemperature < BMP2_ST_ADC_T_MIN || utemperature > BMP2_ST_ADC_T_MAX))
            {
                rslt = BMP2_E_UNCOMP_TEMP_AND_PRESS_RANGE;
            }
            else
            {
                rslt = BMP2_OK;
            }

            return rslt;
        }

    }
}
