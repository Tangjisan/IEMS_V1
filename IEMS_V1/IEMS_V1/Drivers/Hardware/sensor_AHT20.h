#ifndef __SENSOR_AHT20_H
#define __SENSOR_AHT20_H

#include "new"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "tim.h"

// AHT20传感器相关功能命名空间
namespace n_AHT20 {

    /**
     * @brief 初始化AHT20传感器
     * @return HAL状态类型，指示初始化是否成功
     */
    HAL_StatusTypeDef init(void);

    /**
     * @brief 读取AHT20传感器数据
     * @return 读取成功返回true，否则返回false
     */
    bool readData(void);

    /**
     * @brief 重置AHT20传感器
     * @return 重置成功返回true，否则返回false
     */
    bool reset(void);
    
    namespace n_get{
        /**
         * @brief 获取最近一次测量的湿度值
         * @return 湿度值
         */
        float humidity(void);

        /**
         * @brief 获取最近一次测量的温度值
         * @return 温度值
         */
        float temperature(void);

        /**
         * @brief 检查传感器数据是否准备就绪
         * @return 数据就绪返回true，否则返回false
         */
        bool is_data_ready(void);
    }
    
    // AHT20传感器DMA相关功能子命名空间
    namespace n_DMA{

        /**
         * @brief DMA回调状态枚举
         * CB_STATE_READY: 就绪状态
         * CB_STATE_RECEIVE: 接收状态
         * CB_STATE_RESET: 重置状态
         */
        typedef enum {
            CB_STATE_READY,
            CB_STATE_RECEIVE,
            CB_STATE_RESET
        }CbState_t;

        /**
         * @brief 初始化DMA相关功能
         * @return HAL状态类型，指示初始化是否成功
         */
        HAL_StatusTypeDef init(void);

        /**
         * @brief 反初始化DMA相关功能
         * @return HAL状态类型，指示反初始化是否成功
         */
        HAL_StatusTypeDef deinit(void);

        // 回调函数
        namespace n_cb{
            void receive(void);
            void process_measure(void);
            void reset(void);
        }
        
        namespace n_get{
            /**
             * @brief 检查DMA方式下数据是否准备就绪
             * @return 数据就绪返回true，否则返回false
             */
            bool is_data_ready(void);

            /**
             * @brief 检查是否正在使用DMA方式
             * @return 使用DMA返回true，否则返回false
             */
            bool is_use_dma(void);
        }
    }
}

#endif