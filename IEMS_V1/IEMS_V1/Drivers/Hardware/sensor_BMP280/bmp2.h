/**
* Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file       bmp2.h
* @date       2021-05-21
* @version    v1.0.1
*
*/

/*! @file bmp2.h
 * @brief Sensor driver for BMP2 sensor
 */

/*!
 * @defgroup bmp2 BMP2
 */

#ifndef _BMP2_H
#define _BMP2_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/

/*!             Header files
 ****************************************************************************/
#include "bmp2_defs.h"

/***************************************************************************/

/*!     BMP2 用户界面功能原型
 ****************************************************************************/

/**
 * \ingroup bmp2
 * \defgroup bmp2ApiInit Initialization
 * @brief 执行传感器的初始化
 */

/*!
 * \ingroup bmp2ApiInit
 * \page bmp2_api_bmp2_init bmp2_init
 * \code
 * int8_t bmp2_init(struct bmp2_dev *dev);
 * \endcode
 * @details 此接口为入口点。它从传感器读取芯片ID和校准数据。
 *
 *  @param[in,out] dev : bmp2_dev结构实例
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_init(struct bmp2_dev *dev);

/**
 * \ingroup bmp2
 * \defgroup bmp2ApiRegs Registers
 * @brief 初始化传感器和器件结构
 */

/*!
 * \ingroup bmp2ApiRegs
 * \page bmp2_api_bmp2_get_regs bmp2_get_regs
 * \code
 * int8_t bmp2_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, struct bmp2_dev *dev);
 * \endcode
 * @details 此API从传感器的给定寄存器地址读取数据。
 *
 * @param[in] reg_addr  : 要从中读取数据的寄存器地址
 * @param[out] reg_data : 指向用于存储读取数据的数据缓冲区的指针。
 * @param[in] len       : 要读取的数据字节数。
 * @param[in] dev       : bmp2_dev的结构实例。
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, struct bmp2_dev *dev);

/*!
 * \ingroup bmp2ApiRegs
 * \page bmp2_api_bmp2_set_regs bmp2_set_regs
 * \code
 * int8_t bmp2_set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint32_t len, struct bmp2_dev *dev);
 * \endcode
 * @details 此API将给定数据写入传感器的寄存器地址。
 *
 * @param[in] reg_addr : 要写入数据的寄存器地址。
 * @param[in] reg_data : 指向要写入传感器的数据缓冲区的指针。
 * @param[in] len      : 要写入的数据字节数。
 * @param[in] dev      : bmp2_dev的结构实例。
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint32_t len, struct bmp2_dev *dev);

/**
 * \ingroup bmp2
 * \defgroup bmp2ApiSoftreset Soft reset
 * @brief 执行自检
 */

/*!
 * \ingroup bmp2ApiSoftreset
 * \page bmp2_api_bmp2_soft_reset bmp2_soft_reset
 * \code
 * int8_t bmp2_soft_reset(struct bmp2_dev *dev);
 * \endcode
 * @details 此接口触发传感器的软重置。
 *
 * @param[in] dev : bmp2_dev的结构实例。
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_soft_reset(struct bmp2_dev *dev);

/**
 * \ingroup bmp2
 * \defgroup bmp2ApiPowermode Power mode
 * @brief 设置/获取传感器的电源模式
 */

/*!
 * \ingroup bmp2ApiPowermode
 * \page bmp2_api_bmp2_get_power_mode bmp2_get_power_mode
 * \code
 * int8_t bmp2_get_power_mode(uint8_t *mode, struct bmp2_dev *dev);
 * \endcode
 * @details 此接口读取电源模式。
 *
 * @param[out] mode : BMP2_POWERMODE_SLEEP, BMP2_POWERMODE_NORMAL,
 *     BMP2_POWERMODE_FORCED
 * @param[in] dev   : bmp2_dev的结构实例。
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_get_power_mode(uint8_t *mode, struct bmp2_dev *dev);

/*!
 * \ingroup bmp2ApiPowermode
 * \page bmp2_api_bmp2_set_power_mode bmp2_set_power_mode
 * \code
 * int8_t bmp2_set_power_mode(uint8_t mode, const struct bmp2_config *conf, struct bmp2_dev *dev);
 * \endcode
 * @details 此接口写入电源模式。
 *
 * @param[in] mode : BMP2_POWERMODE_SLEEP, BMP2_POWERMODE_NORMAL,
 *     BMP2_POWERMODE_FORCED
 * @param[in] conf   : bmp2_config的结构实例
 * @param[in] dev    : bmp2_dev的结构实例。
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_set_power_mode(uint8_t mode, const struct bmp2_config *conf, struct bmp2_dev *dev);

/**
 * \ingroup bmp2
 * \defgroup bmp2ApiConfig Configuration
 * @brief 设置/获取传感器的配置
 */

/*!
 * \ingroup bmp2ApiConfig
 * \page bmp2_api_bmp2_get_config bmp2_get_config
 * \code
 * int8_t bmp2_get_config(struct bmp2_config *conf, struct bmp2_dev *dev);
 * \endcode
 * @details 此API从ctrl_meas寄存器和配置寄存器读取数据。
 * 给出了当前设定的温度和压力过采样配置、电源模式配置、休眠时间和IIR滤波器系数。
 *
 * @param[out] conf : BMP2的当前配置
 * conf->osrs_t, conf->osrs_p = BMP2_OS_NONE, BMP2_OS_1X,
 *     BMP2_OS_2X, BMP2_OS_4X, BMP2_OS_8X, BMP2_OS_16X
 *
 * conf->odr = BMP2_ODR_0_5_MS, BMP2_ODR_62_5_MS, BMP2_ODR_125_MS,
 *     BMP2_ODR_250_MS, BMP2_ODR_500_MS, BMP2_ODR_1000_MS,
 *     BMP2_ODR_2000_MS, BMP2_ODR_4000_MS
 *
 * conf->filter = BMP2_FILTER_OFF, BMP2_FILTER_COEFF_2,
 *     BMP2_FILTER_COEFF_4, BMP2_FILTER_COEFF_8, BMP2_FILTER_COEFF_16
 *
 * conf->spi3w_en = BMP2_SPI3_WIRE_ENABLE, BMP2_SPI3_WIRE_DISABLE
 *
 * @param[in] dev   : bmp2_dev的结构实例。
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_get_config(struct bmp2_config *conf, struct bmp2_dev *dev);

/*!
 * \ingroup bmp2ApiConfig
 * \page bmp2_api_bmp2_set_config bmp2_set_config
 * \code
 * int8_t bmp2_set_config(const struct bmp2_config *conf, struct bmp2_dev *dev);
 * \endcode
 * @details 此接口将数据写入ctrl_meas寄存器和CONFIG寄存器。
 * 它设置过采样模式、电源模式配置、休眠持续时间和IIR滤波器系数。
 *
 * @param[in] conf : BMP2的所需配置
 * conf->osrs_t, conf->osrs_p = BMP2_OS_NONE, BMP2_OS_1X,
 *     BMP2_OS_2X, BMP2_OS_4X, BMP2_OS_8X, BMP2_OS_16X
 *
 * conf->odr = BMP2_ODR_0_5_MS, BMP2_ODR_62_5_MS, BMP2_ODR_125_MS,
 *     BMP2_ODR_250_MS, BMP2_ODR_500_MS, BMP2_ODR_1000_MS,
 *     BMP2_ODR_2000_MS, BMP2_ODR_4000_MS
 *
 * conf->filter = BMP2_FILTER_OFF, BMP2_FILTER_COEFF_2,
 *     BMP2_FILTER_COEFF_4, BMP2_FILTER_COEFF_8, BMP2_FILTER_COEFF_16
 *
 * conf->spi3w_en = BMP2_SPI3_WIRE_ENABLE, BMP2_SPI3_WIRE_DISABLE
 *
 *  Over-sampling settings   |   conf->os_pres   |   conf->os_temp
 *--------------------------|-------------------|---------------------------
 *  Ultra low power         |       1           |       1
 *  Low power               |       2           |       1
 *  Standard resolution     |       4           |       1
 *  High resolution         |       8           |       1
 *  Ultra high resolution   |       16          |       2
 *
 * @param[in] dev  : bmp2_dev的结构实例。
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_set_config(const struct bmp2_config *conf, struct bmp2_dev *dev);

/**
 * \ingroup bmp2
 * \defgroup bmp2ApiStatus Status register
 * @brief 读取状态寄存器
 */

/*!
 * \ingroup bmp2ApiStatus
 * \page bmp2_api_bmp2_get_status bmp2_get_status
 * \code
 * int8_t bmp2_get_status(struct bmp2_status *status, struct bmp2_dev *dev);
 * \endcode
 * @details 此接口读取状态寄存器
 *
 * @param[in,out] status : bmp2_status的结构实例。
 * @param[in] dev        : bmp2_dev结构实例
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_get_status(struct bmp2_status *status, struct bmp2_dev *dev);

/**
 * \ingroup bmp2
 * \defgroup bmp2ApiSensorData Sensor Data
 * @brief 传感器的数据处理
 */

/*!
 * \ingroup bmp2ApiSensorData
 * \page bmp2_api_bmp2_get_sensor_data bmp2_get_sensor_data
 * \code
 * int8_t bmp2_get_sensor_data(struct bmp2_data *comp_data, struct bmp2_dev *dev);
 * \endcode
 * @details 此API从传感器读取压力和温度数据，补偿数据并将其存储在用户传递的bmp2_data结构实例中。
 *
 * @param[in] comp_data : bmp2_data的结构实例
 * @param[in] dev       : bmp2_dev结构实例
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 */
int8_t bmp2_get_sensor_data(struct bmp2_data *comp_data, struct bmp2_dev *dev);

/*!
 * \ingroup bmp2ApiSensorData
 * \page bmp2_api_bmp2_compensate_data bmp2_compensate_data
 * \code
 * int8_t bmp2_compensate_data(const struct bmp2_uncomp_data *uncomp_data,
 *                               struct bmp2_data *comp_data,
 *                               struct bmp2_dev *dev);
 * \endcode
 * @details 本接口用于对压力和温度数据进行补偿。
 *
 * @param[in] uncomp_data : 包含未补偿的压力、温度数据。
 * @param[out] comp_data  : 包含补偿的压力和/或温度数据。
 * @param[in] dev         : bmp2_dev的结构实例。
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval > 0 -> Warning.
 * @retval < 0 -> Fail.
 *
 */
int8_t bmp2_compensate_data(const struct bmp2_uncomp_data *uncomp_data,
                            struct bmp2_data *comp_data,
                            struct bmp2_dev *dev);

/**
 * \ingroup bmp2
 * \defgroup bmp2ApiMeasTime Compute measurement time
 * @brief 计算测量时间 (以微秒为单位)
 */

/*!
 * \ingroup bmp2ApiMeasTime
 * \page bmp2_api_bmp2_compute_meas_time bmp2_compute_meas_time
 * \code
 * int8_t bmp2_compute_meas_time(uint32_t *sampling_time, const struct bmp2_config *conf, const struct bmp2_dev *dev);
 * \endcode
 * @details 该接口根据待机时间(conf->odr)和过采样模式(conf->os_mode)计算活动配置的测量时间，单位为微秒
 *
 * @param[out] sampling_time : 活动配置的测量时间(微秒)
 * @param[in] conf           : bmp2_config的结构实例
 * @param[in] dev            : bmp2_dev结构实例
 *
 * @return API执行状态的结果。
 *
 * @retval   0 -> Success.
 * @retval < 0 -> Fail.
 *
 */
int8_t bmp2_compute_meas_time(uint32_t *sampling_time, const struct bmp2_config *conf, const struct bmp2_dev *dev);


#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _BMP2_H */
