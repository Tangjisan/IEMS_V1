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
* @file       bmp2.c
* @date       2021-05-21
* @version    v1.0.1
*
*/

/*! @file bmp2.c
 * @brief Sensor driver for BMP2 sensor
 */

#include "bmp2.h"

/********************** Static function declarations ************************/

/*!
 * @brief 此内部API用于检查设备结构体中的空指针。
 *
 * @param[in] dev : bmp2_dev的结构实例。
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval <0 -> Fail
 */
static int8_t null_ptr_check(const struct bmp2_dev *dev);

/*!
 * @brief 此内部API以交错的方式将寄存器地址和各自寄存器数据写入缓存
 *
 * @param[in] reg_addr   : 寄存器地址数组
 * @param[out] temp_buff : 交错方式存储寄存器地址及各自数据的缓存
 * @param[in] reg_data   : 寄存器数据数组
 * @param[in] len        : reg_addr和reg_data数组的长度
 *
 */
static void interleave_data(const uint8_t *reg_addr, uint8_t *temp_buff, const uint8_t *reg_data, uint32_t len);

/*!
 * @brief 本接口用于读取用于计算补偿数据的校准参数。
 *
 * @param[in] dev : bmp2_dev结构实例
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval <0 -> Fail
 */
static int8_t get_calib_param(struct bmp2_dev *dev);

/*!
 * @brief 此内部API用于重置传感器、恢复/设置conf、恢复/设置模式
 *
 * @param[in] mode : 所需模式
 * @param[in] conf : bmp2的所需配置
 *
 * conf->os_mode = BMP2_OS_MODE_ULTRA_LOW_POWER, BMP2_OS_MODE_LOW_POWER, BMP2_OS_MODE_STANDARD_RESOLUTION,
 *     BMP2_OS_MODE_HIGH_RESOLUTION, BMP2_OS_MODE_ULTRA_HIGH_RESOLUTION
 *
 * conf->mode = BMP2_POWERMODE_SLEEP, BMP2_POWERMODE_FORCED, BMP2_POWERMODE_NORMAL
 *
 * conf->odr = BMP2_ODR_0_5_MS, BMP2_ODR_62_5_MS, BMP2_ODR_125_MS,
 *     BMP2_ODR_250_MS, BMP2_ODR_500_MS, BMP2_ODR_1000_MS,
 *     BMP2_ODR_2000_MS, BMP2_ODR_4000_MS
 *
 * conf->filter = BMP2_FILTER_OFF, BMP2_FILTER_COEFF_2,
 *     BMP2_FILTER_COEFF_4, BMP2_FILTER_COEFF_8, BMP2_FILTER_COEFF_16
 *
 * @param[in] dev  : bmp2_dev结构实例
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval <0 -> Fail
 */
static int8_t conf_sensor(uint8_t mode, const struct bmp2_config *conf, struct bmp2_dev *dev);

/*!
 *  @brief 本接口用于根据过采样模式设置温度和压力的过采样率。
 *
 *  @param[in] reg_data  : 包含寄存器数据
 *  @param[out] conf     : bmp2的所需配置
 *
 */
static void set_os_mode(uint8_t *reg_data, const struct bmp2_config *conf);

/*!
 *  @brief 此接口用于解析压力和温度数据，并将其存储在bmp2_uncomp_data结构实例中。
 *
 *  @param[in] reg_data     : 包含需要解析的寄存器数据
 *  @param[out] uncomp_data : 包含未补偿的压力、温度
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval <0 -> Fail
 */
static int8_t parse_sensor_data(const uint8_t *reg_data, struct bmp2_uncomp_data *uncomp_data);

#ifdef BMP2_DOUBLE_COMPENSATION

/*!
 * @brief 内部接口用于从未补偿的温度中获取补偿温度。
 * 返回双浮点精度数据，单位：摄氏度(来源于datasheet 第8.1章)
 *
 * @param[out] comp_temperature : double型补偿温度数据。
 * @param[in] uncomp_data       : 包含未补偿的温度数据。
 * @param[in] dev               : bmp2_dev结构实例.
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
static int8_t compensate_temperature(double *comp_temperature,
                                     const struct bmp2_uncomp_data *uncomp_data,
                                     struct bmp2_dev *dev);

/*!
 * @brief 内部接口用于从未补偿的压力中获取补偿后的压力。
 * 返回双浮点精度数据。单位：帕斯卡(来源于datasheet 第8.1章)
 *
 * @param[out] comp_pressure : double型补偿压力数据。
 * @param[in] uncomp_data : 包含未补偿的压力数据。
 * @param[in] dev         : bmp2_dev结构实例.
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
static int8_t compensate_pressure(double *comp_pressure,
                                  const struct bmp2_uncomp_data *uncomp_data,
                                  const struct bmp2_dev *dev);

#else

/*!
 * @brief 内部接口用于从未补偿的温度中获取补偿温度。
 * 返回32位整数数据类型。单位：0.01摄氏度(来源于datasheet 第3.11.3章)
 *
 * @param[out] comp_temperature : 以整数形式补偿的温度数据。
 * @param[in] uncomp_data       : 包含未补偿的温度数据。
 * @param[in] dev               : bmp2_dev结构实例.
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
static int8_t compensate_temperature(int32_t *comp_temperature,
                                     const struct bmp2_uncomp_data *uncomp_data,
                                     struct bmp2_dev *dev);

/*!
 * @brief 内部接口用于从未补偿的压力中获取补偿后的压力。
 * 返回64位整数数据类型。单位：1/256帕斯卡(来源于datasheet 第3.11.3章)
 *
 * @param[out] comp_pressure : 以整数表示的补偿压力数据。
 * @param[in] uncomp_data    : 包含未补偿的压力数据。
 * @param[in] dev            : bmp2_dev结构实例.
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Fail
 */
static int8_t compensate_pressure(uint32_t *comp_pressure,
                                  const struct bmp2_uncomp_data *uncomp_data,
                                  const struct bmp2_dev *dev);

#endif

/*!
 * @This 内部接口检查未补偿的温度和压力是否在范围内
 *
 * @param[in] utemperature : 未补偿温度
 * @param[in] upressure    : 未补偿压力
 *
 * @return API执行状态结果
 * @retval 0 -> Success
 * @retval <0 -> Fail
 */
static int8_t st_check_boundaries(int32_t utemperature, int32_t upressure);

/****************** 用户函数定义 *******************************/

/*!
 * @brief 此接口为入口点。
 * 它从传感器读取芯片ID和校准数据。
 */
int8_t bmp2_init(struct bmp2_dev *dev)
{
    int8_t rslt;

    rslt = null_ptr_check(dev);//检查设备结构体中的空指针

    if (rslt == BMP2_OK)
    {
        rslt = bmp2_get_regs(BMP2_REG_CHIP_ID, &dev->chip_id, 1, dev);//读取芯片ID

        /* 检查芯片ID的有效性 */
        if (rslt == BMP2_OK)
        {
            if (dev->chip_id == BMP2_CHIP_ID)
            {
                rslt = get_calib_param(dev);//读取用于计算补偿数据的校准参数
            }
            else
            {
                rslt = BMP2_E_DEV_NOT_FOUND;//未发现设备
            }
        }
    }

    return rslt;
}

/*!
 * @brief 此API从传感器的给定寄存器地址读取数据。
 */
int8_t bmp2_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, struct bmp2_dev *dev)
{
    int8_t rslt;

    rslt = null_ptr_check(dev);//检查设备结构体中的空指针

    if ((rslt == BMP2_OK) && (reg_data != NULL))
    {
        /* 如果所选接口为SPI，则屏蔽寄存器地址的MSB */
        if (dev->intf == BMP2_SPI_INTF)
        {
            reg_addr = reg_addr | BMP2_SPI_RD_MASK;
        }

        dev->intf_rslt = dev->read(reg_addr, reg_data, len, dev->intf_ptr);

        /* 检查通信错误并使用内部错误代码进行掩码 */
        if (dev->intf_rslt != BMP2_INTF_RET_SUCCESS)
        {
            rslt = BMP2_E_COM_FAIL;
        }
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;
    }

    return rslt;
}

/*!
 * @brief 此API将给定数据写入传感器的指定寄存器地址。
 */
int8_t bmp2_set_regs(uint8_t *reg_addr, const uint8_t *reg_data, uint32_t len, struct bmp2_dev *dev)
{
    int8_t rslt;
    uint8_t temp_buff[8]; /* 通常不会写入超过4个寄存器 */
    uint32_t temp_len;
    uint8_t reg_addr_cnt;

    if (len > BMP2_MAX_LEN)
    {
        len = BMP2_MAX_LEN;
    }

    rslt = null_ptr_check(dev);//检查设备结构体中的空指针

    if ((rslt == BMP2_OK) && (reg_addr != NULL) && (reg_data != NULL))
    {
        if (len > 0)
        {
            temp_buff[0] = reg_data[0];

            /* 如果所选接口为SPI，则屏蔽寄存器地址的MSB */
            if (dev->intf == BMP2_SPI_INTF)
            {
                /* 将所有REG地址转换为正确的SPI写入地址，即使MSB(R/`W)位0 */
                for (reg_addr_cnt = 0; reg_addr_cnt < len; reg_addr_cnt++)
                {
                    reg_addr[reg_addr_cnt] = reg_addr[reg_addr_cnt] & BMP2_SPI_WR_MASK;
                }
            }

            /* Burst写模式 */
            if (len > 1)
            {
                /* 以交错的方式将寄存器地址和各自寄存器数据写入缓存 */
                interleave_data(reg_addr, temp_buff, reg_data, len);
                temp_len = ((len * 2) - 1);
            }
            else
            {
                temp_len = len;
            }

            dev->intf_rslt = dev->write(reg_addr[0], temp_buff, temp_len, dev->intf_ptr);

            /* 检查通信错误并使用内部错误代码进行掩码 */
            if (dev->intf_rslt != BMP2_INTF_RET_SUCCESS)
            {
                rslt = BMP2_E_COM_FAIL;//通信错误
            }
        }
        else
        {
            rslt = BMP2_E_INVALID_LEN;//长度错误
        }
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;//指针为空
    }

    return rslt;
}

/*!
 * @brief 此接口触发传感器的软重置。
 */
int8_t bmp2_soft_reset(struct bmp2_dev *dev)
{
    int8_t rslt;
    uint8_t reg_addr = BMP2_REG_SOFT_RESET;//复位寄存器
    uint8_t soft_rst_cmd = BMP2_SOFT_RESET_CMD;//复位命令

    rslt = bmp2_set_regs(&reg_addr, &soft_rst_cmd, 1, dev);

    return rslt;
}

/*!
 * @brief 此API从ctrl_meas寄存器和配置寄存器读取数据。
 * 给出了当前设定的温度和压力过采样配置、电源模式配置、休眠时间和IIR滤波器系数。
 */
int8_t bmp2_get_config(struct bmp2_config *conf, struct bmp2_dev *dev)
{
    int8_t rslt;
    uint8_t temp[2] = { 0, 0 };

    if (conf != NULL)
    {
        rslt = bmp2_get_regs(BMP2_REG_CTRL_MEAS, temp, 2, dev);

        if (rslt == BMP2_OK)
        {
            conf->os_temp = BMP2_GET_BITS(temp[0], BMP2_OS_TEMP);
            conf->os_pres = BMP2_GET_BITS(temp[0], BMP2_OS_PRES);
            conf->odr = BMP2_GET_BITS(temp[1], BMP2_STANDBY_DURN);
            conf->filter = BMP2_GET_BITS(temp[1], BMP2_FILTER);
            conf->spi3w_en = BMP2_GET_BITS_POS_0(temp[1], BMP2_SPI3_ENABLE);
        }
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;//指针为空
    }

    return rslt;
}

/*!
 * @brief 此接口将数据写入ctrl_meas寄存器和CONFIG寄存器。
 * 设置过采样模式、电源模式配置、休眠持续时间和IIR滤波器系数。
 */
int8_t bmp2_set_config(const struct bmp2_config *conf, struct bmp2_dev *dev)
{
    return conf_sensor(BMP2_POWERMODE_SLEEP, conf, dev);
}

/*!
 * @brief 此接口读取状态寄存器
 */
int8_t bmp2_get_status(struct bmp2_status *status, struct bmp2_dev *dev)
{
    int8_t rslt;
    uint8_t temp;

    if (status != NULL)
    {
        rslt = bmp2_get_regs(BMP2_REG_STATUS, &temp, 1, dev);

        if (rslt == BMP2_OK)
        {
            status->measuring = BMP2_GET_BITS(temp, BMP2_STATUS_MEAS);
            status->im_update = BMP2_GET_BITS_POS_0(temp, BMP2_STATUS_IM_UPDATE);
        }
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;//指针为空
    }

    return rslt;
}

/*!
 * @brief 此接口读取电源模式。
 */
int8_t bmp2_get_power_mode(uint8_t *mode, struct bmp2_dev *dev)
{
    int8_t rslt;
    uint8_t temp;

    if (mode != NULL)
    {
        rslt = bmp2_get_regs(BMP2_REG_CTRL_MEAS, &temp, 1, dev);

        *mode = BMP2_GET_BITS_POS_0(temp, BMP2_POWERMODE);
        dev->power_mode = *mode;
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;//指针为空
    }

    return rslt;
}

/*!
 * @brief 此接口写入电源模式。
 */
int8_t bmp2_set_power_mode(uint8_t mode, const struct bmp2_config *conf, struct bmp2_dev *dev)
{
    int8_t rslt;

    rslt = conf_sensor(mode, conf, dev);

    return rslt;
}

/*!
 * @brief 此API从传感器读取压力和温度数据，补偿数据并将其存储在用户传递的BMP2_Data结构实例中。
 */
int8_t bmp2_get_sensor_data(struct bmp2_data *comp_data, struct bmp2_dev *dev)
{
    int8_t rslt;
    uint8_t temp[BMP2_P_T_LEN] = { 0 };
    struct bmp2_uncomp_data uncomp_data = { 0 };

    if (comp_data != NULL)
    {
        rslt = bmp2_get_regs(BMP2_REG_PRES_MSB, temp, BMP2_P_T_LEN, dev);

        if (rslt == BMP2_OK)
        {
            /* 解析从传感器读取的数据 */
            rslt = parse_sensor_data(temp, &uncomp_data);

            if (rslt == BMP2_OK)
            {
                /* 补偿来自传感器的压力和/或温度数据 */
                rslt = bmp2_compensate_data(&uncomp_data, comp_data, dev);
            }
        }
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;//指针为空
    }

    return rslt;
}

/*!
 * @brief 本接口用于对压力和温度数据进行补偿。
 */
int8_t bmp2_compensate_data(const struct bmp2_uncomp_data *uncomp_data,
                            struct bmp2_data *comp_data,
                            struct bmp2_dev *dev)
{
    int8_t rslt;

    rslt = null_ptr_check(dev);//检查设备结构体中的空指针

    if ((rslt == BMP2_OK) && (uncomp_data != NULL) && (comp_data != NULL))
    {
        /* 初始化为零 */
        comp_data->temperature = 0;
        comp_data->pressure = 0;

        rslt = compensate_temperature(&comp_data->temperature, uncomp_data, dev);

        if (rslt == BMP2_OK)
        {
            rslt = compensate_pressure(&comp_data->pressure, uncomp_data, dev);
        }
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;//指针为空
    }

    return rslt;
}

/*!
 * @brief 该接口根据待机时间(conf->odr)和过采样模式(conf->os_mode)计算活动配置的测量时间，单位为微秒
 */
int8_t bmp2_compute_meas_time(uint32_t *sampling_time, const struct bmp2_config *conf, const struct bmp2_dev *dev)
{
    int8_t rslt;

    /* Array contains measurement time in microseconds */
    uint32_t measurement_time[] = { 5500, 7500, 11500, 19500, 37500 };
    uint32_t standby_time[] = { 500, 62500, 125000, 250000, 500000, 1000000, 2000000, 4000000 };

    rslt = null_ptr_check(dev);//检查设备结构体中的空指针

    if ((rslt == BMP2_OK) && (conf != NULL))
    {
        if (dev->power_mode == BMP2_POWERMODE_NORMAL)
        {
            /* 时间(以微秒为单位) */
            (*sampling_time) = measurement_time[conf->os_mode] + standby_time[conf->odr];
        }
        else
        {
            /* 时间(以微秒为单位) */
            (*sampling_time) = measurement_time[conf->os_mode];
        }
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;//指针为空
    }

    return rslt;
}

/****************** Static Function Definitions *******************************/

/*!
 * @brief 此内部API用于检查设备结构体中的空指针。
 */
static int8_t null_ptr_check(const struct bmp2_dev *dev)
{
    int8_t rslt;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) || (dev->delay_us == NULL))
    {
        /* 找到空指针 */
        rslt = BMP2_E_NULL_PTR;//指针为空
    }
    else
    {
        rslt = BMP2_OK;
    }

    return rslt;
}

/*!
 * @brief 此内部API以交错的方式将寄存器地址和各自寄存器数据写入缓存
 */
static void interleave_data(const uint8_t *reg_addr, uint8_t *temp_buff, const uint8_t *reg_data, uint32_t len)
{
    uint32_t index;

    for (index = 1; index < len; index++)
    {
        temp_buff[(index * 2) - 1] = reg_addr[index];
        temp_buff[index * 2] = reg_data[index];
    }
}

/*!
 * @brief 本接口用于读取用于计算补偿数据的校准参数。
 */
static int8_t get_calib_param(struct bmp2_dev *dev)
{
    int8_t rslt;
    uint8_t temp[BMP2_CALIB_DATA_SIZE] = { 0 };

    rslt = bmp2_get_regs(BMP2_REG_DIG_T1_LSB, temp, BMP2_CALIB_DATA_SIZE, dev);//读取全部校准数据

    if (rslt == BMP2_OK)
    {
        dev->calib_param.dig_t1 = (uint16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_T1_MSB_POS], temp[BMP2_DIG_T1_LSB_POS]));
        dev->calib_param.dig_t2 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_T2_MSB_POS], temp[BMP2_DIG_T2_LSB_POS]));
        dev->calib_param.dig_t3 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_T3_MSB_POS], temp[BMP2_DIG_T3_LSB_POS]));
        dev->calib_param.dig_p1 = (uint16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P1_MSB_POS], temp[BMP2_DIG_P1_LSB_POS]));
        dev->calib_param.dig_p2 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P2_MSB_POS], temp[BMP2_DIG_P2_LSB_POS]));
        dev->calib_param.dig_p3 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P3_MSB_POS], temp[BMP2_DIG_P3_LSB_POS]));
        dev->calib_param.dig_p4 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P4_MSB_POS], temp[BMP2_DIG_P4_LSB_POS]));
        dev->calib_param.dig_p5 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P5_MSB_POS], temp[BMP2_DIG_P5_LSB_POS]));
        dev->calib_param.dig_p6 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P6_MSB_POS], temp[BMP2_DIG_P6_LSB_POS]));
        dev->calib_param.dig_p7 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P7_MSB_POS], temp[BMP2_DIG_P7_LSB_POS]));
        dev->calib_param.dig_p8 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P8_MSB_POS], temp[BMP2_DIG_P8_LSB_POS]));
        dev->calib_param.dig_p9 = (int16_t) (BMP2_MSBLSB_TO_U16(temp[BMP2_DIG_P9_MSB_POS], temp[BMP2_DIG_P9_LSB_POS]));
        dev->calib_param.dig_p10 = (int8_t) ((uint8_t)(temp[BMP2_DIG_P10_POS]));
    }

    return rslt;
}

/*!
 * @brief 此内部API用于重置传感器、恢复/设置conf、恢复/设置模式
 */
static int8_t conf_sensor(uint8_t mode, const struct bmp2_config *conf, struct bmp2_dev *dev)
{
    int8_t rslt;
    uint8_t temp[2] = { 0, 0 };
    uint8_t reg_addr[2] = { BMP2_REG_CTRL_MEAS, BMP2_REG_CONFIG };

    if (conf != NULL)
    {
        rslt = bmp2_get_regs(BMP2_REG_CTRL_MEAS, temp, 2, dev);//读取测量控制寄存器、配置寄存器

        if (rslt == BMP2_OK)
        {
            /* 这里的目的是让设备在最短的时间内进入睡眠状态 */
            rslt = bmp2_soft_reset(dev);

            if (rslt == BMP2_OK)
            {
                set_os_mode(temp, conf);//根据过采样模式设置温度和压力的过采样率
                temp[1] = BMP2_SET_BITS(temp[1], BMP2_STANDBY_DURN, conf->odr);//待机时间
                temp[1] = BMP2_SET_BITS(temp[1], BMP2_FILTER, conf->filter);//前端电路滤波系数
                temp[1] = BMP2_SET_BITS_POS_0(temp[1], BMP2_SPI3_ENABLE, conf->spi3w_en);//3线SPI使能

                rslt = bmp2_set_regs(reg_addr, temp, 2, dev);

                if ((rslt == BMP2_OK) && (mode != BMP2_POWERMODE_SLEEP))
                {
                    dev->power_mode = mode;

                    /* 在单独的写入中仅写入电源模式寄存器 */
                    temp[0] = BMP2_SET_BITS_POS_0(temp[0], BMP2_POWERMODE, mode);
                    rslt = bmp2_set_regs(reg_addr, temp, 1, dev);
                }
            }
        }
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;//指针为空
    }

    return rslt;
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

#ifdef BMP2_DOUBLE_COMPENSATION

/*!
 * @brief 内部接口用于从未补偿的温度中获取补偿温度。
 * 返回双浮点精度数据，单位：摄氏度(来源于datasheet 第8.1章)
 */
static int8_t compensate_temperature(double *comp_temperature,
                                     const struct bmp2_uncomp_data *uncomp_data,
                                     struct bmp2_dev *dev)
{
    int8_t rslt = BMP2_OK;
    double var1, var2;
    double temperature;

    var1 = (((double) uncomp_data->temperature) / 16384.0 - ((double) dev->calib_param.dig_t1) / 1024.0) *
           ((double) dev->calib_param.dig_t2);
    var2 =
        ((((double) uncomp_data->temperature) / 131072.0 - ((double) dev->calib_param.dig_t1) / 8192.0) *
         (((double) uncomp_data->temperature) / 131072.0 - ((double) dev->calib_param.dig_t1) / 8192.0)) *
        ((double) dev->calib_param.dig_t3);

    dev->calib_param.t_fine = (int32_t) (var1 + var2);
    temperature = (var1 + var2) / 5120.0;

    if (temperature < BMP2_MIN_TEMP_DOUBLE)
    {
        temperature = BMP2_MIN_TEMP_DOUBLE;
        rslt = BMP2_W_MIN_TEMP;
    }

    if (temperature > BMP2_MAX_TEMP_DOUBLE)
    {
        temperature = BMP2_MAX_TEMP_DOUBLE;
        rslt = BMP2_W_MAX_TEMP;
    }

    (*comp_temperature) = temperature;

    return rslt;
}

/*!
 * @brief 内部接口用于从未补偿的压力中获取补偿后的压力。
 * 返回双浮点精度数据。单位：帕斯卡(来源于datasheet 第8.1章)
 */
static int8_t compensate_pressure(double *comp_pressure,
                                  const struct bmp2_uncomp_data *uncomp_data,
                                  const struct bmp2_dev *dev)
{
    int8_t rslt = BMP2_OK;
    double var1, var2;
    double pressure = 0.0;

    var1 = ((double) dev->calib_param.t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double) dev->calib_param.dig_p6) / 32768.0;
    var2 = var2 + var1 * ((double) dev->calib_param.dig_p5) * 2.0;
    var2 = (var2 / 4.0) + (((double) dev->calib_param.dig_p4) * 65536.0);
    var1 = (((double)dev->calib_param.dig_p3) * var1 * var1 / 524288.0 + ((double)dev->calib_param.dig_p2) * var1) /
           524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double) dev->calib_param.dig_p1);

    if (var1 < 0 || var1 > 0)
    {
        pressure = 1048576.0 - (double)uncomp_data->pressure;
        pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
        var1 = ((double)dev->calib_param.dig_p9) * pressure * pressure / 2147483648.0;
        var2 = pressure * ((double)dev->calib_param.dig_p8) / 32768.0;

        pressure = pressure + (var1 + var2 + ((double)dev->calib_param.dig_p7)) / 16.0;

        if (pressure < BMP2_MIN_PRES_DOUBLE)
        {
            pressure = BMP2_MIN_PRES_DOUBLE;
            rslt = BMP2_W_MIN_PRES;
        }

        if (pressure > BMP2_MAX_PRES_DOUBLE)
        {
            pressure = BMP2_MAX_PRES_DOUBLE;
            rslt = BMP2_W_MAX_PRES;
        }

        (*comp_pressure) = pressure;
    }

    return rslt;
}

#else

/*!
 * @brief 内部接口用于从未补偿的温度中获取补偿温度。
 * 返回32位整数数据类型。单位：0.01摄氏度(来源于datasheet 第3.11.3章)
 */
static int8_t compensate_temperature(int32_t *comp_temperature,
                                     const struct bmp2_uncomp_data *uncomp_data,
                                     struct bmp2_dev *dev)
{
    int8_t rslt = BMP2_OK;
    int32_t var1, var2;
    int32_t temperature;

    var1 =
        ((((uncomp_data->temperature / 8) - ((int32_t) dev->calib_param.dig_t1 * 2))) *
         ((int32_t) dev->calib_param.dig_t2)) / 2048;
    var2 =
        (((((uncomp_data->temperature / 16) - ((int32_t) dev->calib_param.dig_t1)) *
           ((uncomp_data->temperature / 16) - ((int32_t) dev->calib_param.dig_t1))) / 4096) *
         ((int32_t) dev->calib_param.dig_t3)) / 16384;

    dev->calib_param.t_fine = var1 + var2;

    temperature = (dev->calib_param.t_fine * 5 + 128) / 256;

    if (temperature < BMP2_MIN_TEMP_INT)
    {
        temperature = BMP2_MIN_TEMP_INT;
        rslt = BMP2_W_MIN_TEMP;
    }

    if (temperature > BMP2_MAX_TEMP_INT)
    {
        temperature = BMP2_MAX_TEMP_INT;
        rslt = BMP2_W_MAX_TEMP;
    }

    (*comp_temperature) = temperature;

    return rslt;
}

#ifndef BMP2_32BIT_COMPENSATION

/*!
 * @brief 内部接口用于从未补偿的压力中获取补偿后的压力。
 * 返回64位整数数据类型。单位：1/256帕斯卡(来源于datasheet 第3.11.3章)
 */
static int8_t compensate_pressure(uint32_t *comp_pressure,
                                  const struct bmp2_uncomp_data *uncomp_data,
                                  const struct bmp2_dev *dev)
{
    int8_t rslt = BMP2_OK;
    int64_t var1, var2, p;
    uint32_t pressure = 0;

    var1 = ((int64_t) dev->calib_param.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t) dev->calib_param.dig_p6;
    var2 = var2 + ((var1 * (int64_t) dev->calib_param.dig_p5) * 131072);
    var2 = var2 + (((int64_t) dev->calib_param.dig_p4) * 34359738368);
    var1 = ((var1 * var1 * (int64_t) dev->calib_param.dig_p3) / 256) +
           ((var1 * (int64_t) dev->calib_param.dig_p2) * 4096);
    var1 = (((((int64_t)1) * 140737488355328) + var1)) * ((int64_t)dev->calib_param.dig_p1) / 8589934592;

    if (var1 != 0)
    {
        p = 1048576 - uncomp_data->pressure;
        p = (((p * 2147483648) - var2) * 3125) / var1;
        var1 = (((int64_t) dev->calib_param.dig_p9) * (p / 8192) * (p / 8192)) / 33554432;
        var2 = (((int64_t) dev->calib_param.dig_p8) * p) / 524288;

        p = ((p + var1 + var2) / 256) + (((int64_t)dev->calib_param.dig_p7) * 16);
        pressure = (uint32_t)p;

        if (pressure < BMP2_MIN_PRES_64INT)
        {
            pressure = BMP2_MIN_PRES_64INT;
            rslt = BMP2_W_MIN_PRES;
        }

        if (pressure > BMP2_MAX_PRES_64INT)
        {
            pressure = BMP2_MAX_PRES_64INT;
            rslt = BMP2_W_MAX_PRES;
        }

        (*comp_pressure) = pressure;
    }

    return rslt;
}

#else

/*!
 * @brief 内部接口用于从补偿压力中获取补偿压力。
 * 返回32位整数数据类型。单位：帕斯卡(来源于datasheet 第8.2章)
 */
static int8_t compensate_pressure(uint32_t *comp_pressure,
                                  const struct bmp2_uncomp_data *uncomp_data,
                                  const struct bmp2_dev *dev)
{
    int8_t rslt = BMP2_OK;
    int32_t var1, var2;
    uint32_t pressure = 0;

    var1 = (((int32_t) dev->calib_param.t_fine) / 2) - (int32_t) 64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t) dev->calib_param.dig_p6);
    var2 = var2 + ((var1 * ((int32_t) dev->calib_param.dig_p5)) * 2);
    var2 = (var2 / 4) + (((int32_t) dev->calib_param.dig_p4) * 65536);
    var1 =
        (((dev->calib_param.dig_p3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8) +
         ((((int32_t) dev->calib_param.dig_p2) * var1) / 2)) / 262144;
    var1 = ((((32768 + var1)) * ((int32_t) dev->calib_param.dig_p1)) / 32768);

    /* Avoid exception caused by division with zero */
    if (var1 != 0)
    {
        pressure = (((uint32_t)(((int32_t)1048576) - uncomp_data->pressure) - (var2 / 4096))) * 3125;

        /* Check for overflows against UINT32_MAX/2; if pressure is left-shifted by 1 */
        if (pressure < 0x80000000)
        {
            pressure = (pressure * 2) / ((uint32_t) var1);
        }
        else
        {
            pressure = (pressure / (uint32_t) var1) * 2;
        }

        var1 = (((int32_t) dev->calib_param.dig_p9) * ((int32_t) (((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((int32_t) (pressure / 4)) * ((int32_t) dev->calib_param.dig_p8)) / 8192;
        pressure = (uint32_t) ((int32_t) pressure + ((var1 + var2 + dev->calib_param.dig_p7) / 16));

        if (pressure < BMP2_MIN_PRES_32INT)
        {
            pressure = BMP2_MIN_PRES_32INT;
            rslt = BMP2_W_MIN_PRES;
        }

        if (pressure > BMP2_MAX_PRES_32INT)
        {
            pressure = BMP2_MAX_PRES_32INT;
            rslt = BMP2_W_MAX_PRES;
        }

        (*comp_pressure) = pressure;
    }

    return rslt;
}

#endif
#endif

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
