/**\
 * Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
#ifndef _COMMON_H
#define _COMMON_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "bmp2.h"

/***************************************************************************/

/*!                 User function prototypes
 ****************************************************************************/

/*!
 *  @brief 通过SPI总线读取传感器寄存器的功能。
 *
 *  @param[in] reg_addr   : 寄存器地址。
 *  @param[out] reg_data  : 指向用于存储读取数据的数据缓冲区的指针。
 *  @param[in] length     : 要读取的字节数。
 *  @param[in] intf_ptr   : 接口指针
 *
 *  @return 执行情况
 *
 *  @retval BMP2_INTF_RET_SUCCESS -> Success.
 *  @retval != BMP2_INTF_RET_SUCCESS -> Failure.
 *
 */
BMP2_INTF_RET_TYPE bmp2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 *  @brief 通过I2C总线读取传感器寄存器的功能。
 *
 *  @param[in] reg_addr   : 寄存器地址。
 *  @param[out] reg_data  : 指向用于存储读取数据的数据缓冲区的指针。
 *  @param[in] length     : 要读取的字节数。
 *  @param[in] intf_ptr   : 接口指针
 *
 *  @return 执行情况
 *
 *  @retval BMP2_INTF_RET_SUCCESS -> Success.
 *  @retval != BMP2_INTF_RET_SUCCESS -> Failure.
 *
 */
BMP2_INTF_RET_TYPE bmp2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 *  @brief 通过SPI总线写入传感器寄存器的函数。
 *
 *  @param[in] reg_addr   : 寄存器地址。
 *  @param[in] reg_data   : 指向要写入其值的数据缓冲区的指针。
 *  @param[in] length     : 要写入的字节数。
 *  @param[in] intf_ptr   : 接口指针
 *
 *  @return 执行情况
 *
 *  @retval BMP2_INTF_RET_SUCCESS -> Success.
 *  @retval != BMP2_INTF_RET_SUCCESS -> Failure.
 *
 */
BMP2_INTF_RET_TYPE bmp2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 *  @brief 通过I2C总线写入传感器寄存器的功能。
 *
 *  @param[in] reg_addr : 寄存器地址。
 *  @param[in] reg_data : 指向要写入其值的数据缓冲区的指针。
 *  @param[in] length   : 要写入的字节数。
 *  @param[in] intf_ptr : 接口指针
 *
 *  @return 执行情况
 *
 *  @retval BMP2_INTF_RET_SUCCESS -> Success.
 *  @retval != BMP2_INTF_RET_SUCCESS -> Failure.
 *
 */
BMP2_INTF_RET_TYPE bmp2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 *  @brief 此函数根据一些API中提供的输入，提供所需时间的延迟(微秒)。
 *
 *  @param[in] period_us  : 所需等待时间(以微秒为单位)。
 *  @param[in] intf_ptr   : 接口指针
 *
 *  @return void.
 */
void bmp2_delay_us(uint32_t period_us, void *intf_ptr);

/*!
 *  @brief 该功能用于选择SPI和I2C之间的接口。
 *
 *  @param[in] dev    : bmp2_dev结构实例
 *  @param[in] intf   : 接口选择参数
 *                          For I2C : BMP2_I2C_INTF
 *                          For SPI : BMP2_SPI_INTF
 *
 *  @return 执行情况
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure
 */
int8_t bmp2_interface_selection(struct bmp2_dev *dev, uint8_t intf);

/*!
 *  @brief 本接口用于打印执行状态。
 *
 *  @param[in] api_name : 待打印其执行状态的API的名称。
 *  @param[in] rslt     : 需要打印执行状态的接口返回的错误码。
 *
 *  @return void.
 */
void bmp2_error_codes_print_result(const char api_name[], int8_t rslt);

/*!
 * @brief 此功能取消初始化Coines平台
 *
 *  @return void.
 *
 */
void bmp2_coines_deinit(void);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _COMMON_H */
