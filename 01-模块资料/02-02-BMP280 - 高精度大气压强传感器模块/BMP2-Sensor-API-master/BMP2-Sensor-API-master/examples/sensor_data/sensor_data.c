/**\
 * Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

/******************************************************************************/
/*!                 Header Files                                              */
#include <stdio.h>
#include "bmp2.h"
#include "common.h"

/******************************************************************************/
/*!         Static Function Declaration                                       */

/*!
 *  @brief 内部接口用于获取补偿后的压力和温度数据。
 *
 *  @param[in] period   : 包含以微秒为单位的延迟
 *  @param[in] conf     : bmp2_config的结构实例。
 *  @param[in] dev      : bmp2_dev的结构实例。
 *
 *  @return 执行状态。
 */
static int8_t get_data(uint32_t period, struct bmp2_config *conf, struct bmp2_dev *dev);

/******************************************************************************/
/*!            Functions                                        */

/* 此函数用于启动程序的执行. */
int main(void)
{
    int8_t rslt;
    uint32_t meas_time;
    struct bmp2_dev dev;
    struct bmp2_config conf;

    /* 界面选择更新为参数
     * For I2C :  BMP2_I2C_INTF
     * For SPI :  BMP2_SPI_INTF
     */
    rslt = bmp2_interface_selection(&dev, BMP2_I2C_INTF);
    bmp2_error_codes_print_result("bmp2_interface_selection", rslt);

    rslt = bmp2_init(&dev);
    bmp2_error_codes_print_result("bmp2_init", rslt);

    /* 在写入之前始终读取当前设置，特别是在未修改所有配置的情况下 */
    rslt = bmp2_get_config(&conf, &dev);
    bmp2_error_codes_print_result("bmp2_get_config", rslt);

    /* 配置过采样模式、滤波器系数和输出数据速率。 */
    /* 覆盖所需设置 */
    conf.filter = BMP2_FILTER_OFF;

    /* 过采样模式设置为高分辨率，即os_pres=8x和os_temp=1x */
    conf.os_mode = BMP2_OS_MODE_HIGH_RESOLUTION;

    /* 设置输出数据速率 */
    conf.odr = BMP2_ODR_250_MS;

    rslt = bmp2_set_config(&conf, &dev);
    bmp2_error_codes_print_result("bmp2_set_config", rslt);

    /* 设置normal电源模式 */
    rslt = bmp2_set_power_mode(BMP2_POWERMODE_NORMAL, &conf, &dev);
    bmp2_error_codes_print_result("bmp2_set_power_mode", rslt);

    /* 以微秒为单位计算测量时间 */
    rslt = bmp2_compute_meas_time(&meas_time, &conf, &dev);
    bmp2_error_codes_print_result("bmp2_compute_meas_time", rslt);

    /* 读取压力和温度数据 */
    rslt = get_data(meas_time, &conf, &dev);
    bmp2_error_codes_print_result("get_data", rslt);

    bmp2_coines_deinit();

    return 0;
}

/*!
 *  @brief 内部接口用于获取补偿后的压力和温度数据。
 */
static int8_t get_data(uint32_t period, struct bmp2_config *conf, struct bmp2_dev *dev)
{
    int8_t rslt = BMP2_E_NULL_PTR;
    int8_t idx = 1;
    struct bmp2_status status;
    struct bmp2_data comp_data;

    printf("Measurement delay : %lu us\n", (long unsigned int)period);

    while (idx <= 50)
    {
        rslt = bmp2_get_status(&status, dev);
        bmp2_error_codes_print_result("bmp2_get_status", rslt);

        if (status.measuring == BMP2_MEAS_DONE)
        {
            /* Delay between measurements */
            dev->delay_us(period, dev->intf_ptr);

            /* Read compensated data */
            rslt = bmp2_get_sensor_data(&comp_data, dev);
            bmp2_error_codes_print_result("bmp2_get_sensor_data", rslt);

            #ifdef BMP2_64BIT_COMPENSATION
            comp_data.pressure = comp_data.pressure / 256;
            #endif

            #ifdef BMP2_DOUBLE_COMPENSATION
            printf("Data[%d]:    Temperature: %.4lf deg C	Pressure: %.4lf Pa\n",
                   idx,
                   comp_data.temperature,
                   comp_data.pressure);
            #else
            printf("Data[%d]:    Temperature: %ld deg C	Pressure: %lu Pa\n", idx, (long int)comp_data.temperature,
                   (long unsigned int)comp_data.pressure);
            #endif

            idx++;
        }
    }

    return rslt;
}
