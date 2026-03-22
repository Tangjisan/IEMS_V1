#include "sensor_PMS5003.h"

namespace n_PMS5003{
    // 硬件相关定义
    auto _set_pin    = _PMS5003_SET_Pin;
    auto _set_port   = _PMS5003_SET_GPIO_Port;
    auto _reset_pin  = _PMS5003_RESET_Pin;
    auto _reset_port = _PMS5003_RESET_GPIO_Port;

    UART_HandleTypeDef* _p_husart = nullptr;

    // 软件相关定义
    uint8_t* _buffer_rx     = nullptr;
    uint8_t  _buffer_rx_sz  = 32;       // 硬件需32字节接收
    uint8_t  _buffer_rx_idx = 0;        // 用于记录当前处理到那个数据

    // PMS5003协议相关定义
    const uint8_t _START0_CMD      = 0x42;
    const uint8_t _START1_CMD      = 0x4d;
    const uint16_t _FRAMELENTH_CMD = 0x1C;
    
    // 数据相关定义
    data_t _data = {0};

    // 状态机
    uint8_t      _data_flag     = 0;                  // 0:数据异常，不能读取，1:数据正常
    send_state_t _send_state    = SEND_STATE_FORCED;
    run_state_t  _run_state     = RUN_STATE_NORMAL;
    uint8_t      _is_use_driver = 0;                  // 0:未用本硬件，1:使用本硬件

    namespace n_cmd{
        // PMS5003命令定义
        const uint8_t _receive_cmd[7]     = {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71};
        const uint8_t _forced_mode_cmd[7] = {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};
        const uint8_t _auto_mode_cmd[7]   = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71};
        const uint8_t _sleep_mode_cmd[7]  = {0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73};
        const uint8_t _normal_mode_cmd[7] = {0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74};
        uint8_t _cmd_sz = 7;

        inline HAL_StatusTypeDef request_receive(void){
            return HAL_UART_Transmit(_p_husart, _receive_cmd, _cmd_sz, 300);
        }
        inline HAL_StatusTypeDef request_receive_IT(void){
            return HAL_UART_Transmit_IT(_p_husart, _receive_cmd, _cmd_sz);
        }
        inline HAL_StatusTypeDef transform_forced_mode(void){
            return HAL_UART_Transmit(_p_husart, _forced_mode_cmd, _cmd_sz, 300);
        }
        inline HAL_StatusTypeDef transform_auto_mode(void){
            return HAL_UART_Transmit(_p_husart, _auto_mode_cmd, _cmd_sz, 300);
        }
        inline HAL_StatusTypeDef sleep_mode(void){
            return HAL_UART_Transmit(_p_husart, _sleep_mode_cmd, _cmd_sz, 300);
        }
        inline HAL_StatusTypeDef normal_mode(void){
            return HAL_UART_Transmit(_p_husart, _normal_mode_cmd, _cmd_sz, 300);
        }
        // 轮询接收数据
        inline HAL_StatusTypeDef receive_32data(void){
            return HAL_UART_Receive(_p_husart, _buffer_rx, _buffer_rx_sz, 300);
        }
        // 轮询接收数据
        inline HAL_StatusTypeDef receive_32data_IT(void){
            return HAL_UART_Receive_IT(_p_husart, _buffer_rx, _buffer_rx_sz);
        }
        // 中断接收第0，1个字节
        inline HAL_StatusTypeDef receive_startcmd_IT(void){
            return HAL_UART_Receive_IT(_p_husart, _buffer_rx, 2);
        }
        // 中断接收完整数据帧
        inline HAL_StatusTypeDef receive_data_IT(void){
            return HAL_UART_Receive_IT(_p_husart, _buffer_rx + 2, _buffer_rx_sz - 2);
        }

    } // namespace n_cmd
    


    void sleep(void){
        HAL_GPIO_WritePin(_set_port, _set_pin, GPIO_PIN_RESET);
    }
    void wake(void){
        HAL_GPIO_WritePin(_set_port, _set_pin, GPIO_PIN_SET);
    }
    void reset(void){
        HAL_GPIO_WritePin(_reset_port, _reset_pin, GPIO_PIN_RESET);
        HAL_Delay(10);
        HAL_GPIO_WritePin(_reset_port, _reset_pin, GPIO_PIN_SET);
    }

    // _send_state == SEND_STATE_AUTO goto HAL_UART_RxCpltCallback
    HAL_StatusTypeDef init(Config_t& config){
            _p_husart   = config.husart;
            _set_port   = config.set_port;
            _set_pin    = config.set_pin;
            _reset_port = config.reset_port;
            _reset_pin  = config.reset_pin;
            _run_state  = config.run_state;
            _send_state = config.send_state;

        sleep();
        reset();
        wake();
        HAL_Delay(200);

        if(_buffer_rx == nullptr)
            _buffer_rx = new(std::nothrow) uint8_t[_buffer_rx_sz]();

        if(_buffer_rx == nullptr){
            deinit();
            return HAL_ERROR;
        }
        // 设置工作模式
        switch(_run_state){
            case RUN_STATE_SLEEP:
                n_cmd::sleep_mode();
                HAL_Delay(100);
                break;
            case RUN_STATE_NORMAL:
                n_cmd::normal_mode();
                HAL_Delay(100);
                break;
            default:
                break;
        }

        // 设置发送模式
        switch(_send_state){
            case SEND_STATE_AUTO:
                n_cmd::transform_auto_mode();
                HAL_Delay(100);
                break;
            case SEND_STATE_FORCED:
                n_cmd::transform_forced_mode();
                HAL_Delay(100);
                break;
            default:
                break;
        }

        // 准备接收数据
        switch(_send_state){
            case SEND_STATE_AUTO:
                n_cmd::receive_startcmd_IT();    // 开启中断接收
                break;
            default:
                break;
        }

        _is_use_driver = 1;
        _buffer_rx_idx = 0;
        _data_flag     = 0;

        return HAL_OK;
    }
    HAL_StatusTypeDef deinit(void){
        sleep();
        delete[] _buffer_rx;
        _buffer_rx     = nullptr;
        _p_husart      = nullptr;
        _is_use_driver = 0;
        return HAL_OK;
    }

    bool parseData(uint8_t* buf, data_t* pms_data){
        if(buf[0] != _START0_CMD || buf[1] != _START1_CMD){
            return false;
        }   
        // 计算帧长度
        uint16_t frameLength = ((buf[2] << 8) | buf[3]);
        if(frameLength != _FRAMELENTH_CMD)  return false;
        
        // 计算校验码
        uint16_t checksum = 0;
        for(int i = 0; i < 30; ++i){
            checksum += buf[i];
        }
        uint16_t frame_check = (buf[30] << 8) | buf[31];
        if(checksum != frame_check) return false;
        
        pms_data->pm1_0 = (buf[4] << 8) | buf[5];
        pms_data->pm2_5 = (buf[6] << 8) | buf[7];
        pms_data->pm10  = (buf[8] << 8) | buf[9];

        return true;
    }


    namespace n_forced_mode{
        uint8_t _is_use_timmer = 0; // 0:不使用定时器，1:使用定时器
        // 轮询接收数据
        // 仅在强制模式下有效
        // 返回HAL_OK表示命令发送成功，数据解析结果通过n_get::is_data_ready获取
        // 返回HAL_ERROR表示命令发送失败或当前不在强制模式下
        HAL_StatusTypeDef receive(void){
            if(n_get::is_use_driver() == false)     return HAL_ERROR;
            if(_run_state != RUN_STATE_NORMAL)      return HAL_ERROR;   // 仅在正常模式下接收
            if(_send_state != SEND_STATE_FORCED)    return HAL_ERROR;   // 仅在强制发送模式下接收

            HAL_StatusTypeDef rslt = HAL_OK;

            n_cmd::request_receive();
            rslt = n_cmd::receive_32data();
            if(rslt != HAL_OK)  return rslt;

            _data_flag = parseData(_buffer_rx, &_data) ? 1 : 0;

            return HAL_OK;
        }


        

        HAL_StatusTypeDef use_timmer(TIM_HandleTypeDef* htim){
            if(n_get::is_use_driver() == false)     return HAL_ERROR;
            if(_run_state != RUN_STATE_NORMAL)      return HAL_ERROR;   // 仅在正常模式下接收
            if(_send_state != SEND_STATE_FORCED)    return HAL_ERROR;   // 仅在强制发送模式下接收
            if(_is_use_timmer == 1)                 return HAL_OK;      // 已经使用定时器
            HAL_StatusTypeDef rslt = HAL_OK;

            if(HAL_TIM_Base_GetState(htim) == HAL_TIM_STATE_READY){
                rslt = HAL_TIM_Base_Start_IT(htim);
            }
            _is_use_timmer = 1;

            if(HAL_TIM_Base_GetState(htim) != HAL_TIM_STATE_BUSY){
                _is_use_timmer = 0;
                return HAL_ERROR;
            }

            return HAL_OK;
        }
        
        namespace n_cb{
            CbState_t _cb_forced_state = CB_STATE_WAITTING;
            /// @brief 强制模式下发送请求数据回调
            // GOTO HAL_UART_TxCpltCallback
            HAL_StatusTypeDef request_receive(void){
                if(n_get::is_use_driver() == false)     return HAL_ERROR;
                if(_run_state != RUN_STATE_NORMAL)      return HAL_ERROR;   // 仅在正常模式下接收
                if(_send_state != SEND_STATE_FORCED)    return HAL_ERROR;   // 仅在强制发送模式下接收

                HAL_StatusTypeDef rslt = HAL_OK;
// HAL_GPIO_TogglePin(LED_P5_GPIO_Port, LED_P5_Pin);
                
                switch (_cb_forced_state){
                case CB_STATE_WAITTING:
                    rslt = n_cmd::request_receive_IT();
                    if(rslt != HAL_OK){
                        _cb_forced_state = CB_STATE_WAITTING;
                        return rslt;
                    }
                    _cb_forced_state = CB_STATE_RECEIVE;
                    break;
                default:
                    _cb_forced_state = CB_STATE_WAITTING;
                    break;
                }
                
                return rslt;
            }
            /// @brief 强制模式下接收数据回调
            /// @param  
            // GOTO HAL_UART_RxCpltCallback
            HAL_StatusTypeDef receive(void){
                if(n_get::is_use_driver() == false) return HAL_ERROR;
                if(_run_state != RUN_STATE_NORMAL)  return HAL_ERROR;     // 仅在正常模式下接收
                if(_send_state != SEND_STATE_FORCED)  return HAL_ERROR;     // 仅在强制发送模式下接收

                HAL_StatusTypeDef rslt = HAL_OK;

                switch (_cb_forced_state){
                case CB_STATE_RECEIVE:
                    /* code */
                    rslt = n_cmd::receive_32data_IT();
                    if(rslt != HAL_OK){
                        _cb_forced_state = CB_STATE_WAITTING;
                        return rslt;
                    }
                    _cb_forced_state = CB_STATE_READY;
                    break;
                
                default:
                    _cb_forced_state = CB_STATE_WAITTING;
                    break;
                }
                
                return rslt;
            }

            /// @brief 解析强制模式下接收的数据
            // goto none
            HAL_StatusTypeDef parse_data(void){
                if(n_get::is_use_driver() == false) return HAL_ERROR;
                if(_run_state != RUN_STATE_NORMAL)  return HAL_ERROR;     // 仅在正常模式下接收
                if(_send_state != SEND_STATE_FORCED)  return HAL_ERROR;     // 仅在强制发送模式下接收
                
                switch (_cb_forced_state){
                case CB_STATE_READY:
                    _data_flag = parseData(_buffer_rx, &_data) ? 1 : 0;
                    _cb_forced_state = CB_STATE_WAITTING;
                    break;
                default:
                    _cb_forced_state = CB_STATE_WAITTING;
                    break;
                }
                
                return HAL_OK;
            }
        }
    } // namespace n_forced_mode

    namespace n_auto_mode{
        namespace n_cb{
            CbState_t _cb_auto_state = CB_STATE_WAITTING;
            // 中断接收数据回调
            // CbState CB_STATE_RECEIVE  => goto HAL_UART_RxCpltCallback
            // CbState CB_STATE_READY    => goto HAL_UART_RxCpltCallback
            // CbState CB_STATE_WAITTING => goto HAL_UART_RxCpltCallback
            HAL_StatusTypeDef receive(void){
                if(n_get::is_use_driver() == false) return HAL_ERROR;
                if(_run_state != RUN_STATE_NORMAL)  return HAL_ERROR;     // 仅在正常模式下接收
                if(_send_state != SEND_STATE_AUTO)  return HAL_ERROR;     // 仅在自动发送模式下接收

                // if(_state != CB_STATE_READY)    return;

                HAL_StatusTypeDef rslt = HAL_OK;

                if(_cb_auto_state == CB_STATE_WAITTING && _buffer_rx[0] == _START0_CMD && _buffer_rx[1] == _START1_CMD){
                    _cb_auto_state = CB_STATE_READY;
                }

                switch(_cb_auto_state){
                    case CB_STATE_WAITTING:
                        rslt = n_cmd::receive_startcmd_IT();
                        break;
                    case CB_STATE_READY:
                        _cb_auto_state = CB_STATE_RECEIVE;
                        rslt = n_cmd::receive_data_IT();
                        break;
                    case CB_STATE_RECEIVE:
                        _cb_auto_state = CB_STATE_WAITTING;
                        if(parseData(_buffer_rx, &_data) == false)  _data_flag = 0;
                        else _data_flag = 1;
                        // 开启下一次测量
                        n_cmd::receive_startcmd_IT();
                        break;
                    default:
                        // 重置索引
                        _cb_auto_state = CB_STATE_WAITTING;
                        _buffer_rx[0]  = 0;
                        _buffer_rx[1]  = 0;
                        rslt = n_cmd::receive_startcmd_IT();
                        break;
                }
                return rslt;
            }

        } // namespace n_cb
    }
    
    namespace n_get{
        bool is_data_ready(void){
            return _data_flag == 1;
        }
        bool is_use_driver(void){
            return _is_use_driver == 1;
        }
        uint16_t pm1_0(void){
            return _data.pm1_0;
        }
        uint16_t pm2_5(void){
            return _data.pm2_5;
        }
        uint16_t pm10(void){
            return _data.pm10;
        }
    } // namespace n_get
    
    
}


