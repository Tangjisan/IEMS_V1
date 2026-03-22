#ifndef __SENSOR_SGP30_H
#define __SENSOR_SGP30_H

#include "new"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "tim.h"

/**
 * @brief SGP30传感器相关功能命名空间
 * 
 * 包含SGP30传感器的初始化、数据读取、数据获取等核心功能，
 * 以及DMA模式下的相关操作接口
 */
namespace n_SGP30 {
    /**
     * @brief 初始化SGP30传感器
     * @return HAL_StatusTypeDef HAL状态枚举，指示初始化成功与否
     */
    HAL_StatusTypeDef init(void);

    /**
     * @brief 读取SGP30传感器数据
     * @return bool 数据读取成功返回true，失败返回false
     */
    bool readData(void);


    namespace n_get{
         /**
         * @brief 获取TVOC（总挥发性有机化合物）浓度值
         * @return uint16_t TVOC浓度值，单位为ppb（十亿分之一）
         */
        uint16_t tvoc(void);
        /**
         * @brief 获取eCO2（等效二氧化碳）浓度值
         * @return uint16_t eCO2浓度值，单位为ppm（百万分之一）
         */
        uint16_t eco2(void);
        /**
         * @brief 检查传感器数据是否准备就绪
         * @return bool 数据就绪返回true，未就绪返回false
         */
        bool is_data_ready(void);
        /**
         * @brief 检查传感器是否初始化
         * @return bool 数据就绪返回true，未就绪返回false
         */
        bool is_use_driver(void);
    } // namespace n_get
    
    namespace n_DMA{
        /**
         * @brief DMA回调状态枚举
         * CB_STATE_READY: 就绪状态
         * CB_STATE_RECEIVE: 接收状态
         */
        typedef enum {
            CB_STATE_READY,       // 回调就绪状态
            CB_STATE_RECEIVE      // 接收回调状态
        }CbState_t;

        /**
         * @brief 初始化DMA模块（用于SGP30传感器通信）
         * @return HAL_StatusTypeDef HAL状态枚举，指示初始化成功与否
         */
        HAL_StatusTypeDef init(void);

        /**
         * @brief 反初始化DMA模块（用于SGP30传感器通信）
         * @return HAL_StatusTypeDef HAL状态枚举，指示反初始化成功与否
         */
        HAL_StatusTypeDef deinit(void);

        // 相关回调
        namespace n_cb{
            void receive(void);
            void process_measure(void);
        } 

        namespace n_get{
            /**
             * @brief 检查DMA模式下数据是否准备就绪
             * @return bool 数据就绪返回true，未就绪返回false
             */
            bool is_data_ready(void);

            /**
             * @brief 检查是否正在使用DMA模式
             * @return bool 使用DMA返回true，未使用返回false
             */
            bool is_use_dma(void);
        } // namespace n_get
    }
}

#endif