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
* @file       bmp2_defs.h
* @date       2021-05-21
* @version    v1.0.1
*
*/

/*! @file bmp2_defs.h
 * @brief Sensor driver for BMP2 sensor
 */

#ifndef _BMP2_DEFS_H
#define _BMP2_DEFS_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************/
/*! @name       Header includes             */
/****************************************************************/
#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stddef.h>
#endif

/****************************************************************/
/*! @name       通用宏               */
/****************************************************************/
#ifdef __KERNEL__
#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x)    S8_C(x)
#define UINT8_C(x)   U8_C(x)
#endif

#if !defined(UINT16_C) && !defined(INT16_C)
#define INT16_C(x)   S16_C(x)
#define UINT16_C(x)  U16_C(x)
#endif

#if !defined(INT32_C) && !defined(UINT32_C)
#define INT32_C(x)   S32_C(x)
#define UINT32_C(x)  U32_C(x)
#endif

#if !defined(INT64_C) && !defined(UINT64_C)
#define INT64_C(x)   S64_C(x)
#define UINT64_C(x)  U64_C(x)
#endif
#endif

/*! @name C标准宏 */
#ifndef NULL
#ifdef __cplusplus
#define NULL         0
#else
#define NULL         ((void *) 0)
#endif
#endif

/******************************************************************************/
/*! @name        编译器开关宏定义                */
/******************************************************************************/
#ifndef BMP2_64BIT_COMPENSATION /*< 检查是否启用了64位(使用BMP2_64bit_Compensation) */
#ifndef BMP2_32BIT_COMPENSATION /*< 检查是否启用了32位(使用BMP2_32bit_Compensation */
#ifndef BMP2_DOUBLE_COMPENSATION /*< 如果任何整数数据类型未启用，则启用 BMP2_DOUBLE_COMPENSATION */
#define BMP2_32BIT_COMPENSATION
#endif
#endif
#endif

/******************************************************************************/
/*! @name        常规宏定义                */
/******************************************************************************/

/*! @name 寄存器的最大写入长度 */
#define BMP2_MAX_LEN                                  UINT8_C(4)

/*! @name 返回代码 */
/*! @name 成功代码*/
#define BMP2_OK                                       INT8_C(0)

/*! @name 错误代码 */
#define BMP2_E_NULL_PTR                               INT8_C(-1)//指针为空
#define BMP2_E_COM_FAIL                               INT8_C(-2)//通信错误
#define BMP2_E_INVALID_LEN                            INT8_C(-3)//长度错误
#define BMP2_E_DEV_NOT_FOUND                          INT8_C(-4)//未发现设备
#define BMP2_E_UNCOMP_TEMP_RANGE                      INT8_C(-5)//未补偿温度超范围
#define BMP2_E_UNCOMP_PRESS_RANGE                     INT8_C(-6)//未补偿压力超范围
#define BMP2_E_UNCOMP_TEMP_AND_PRESS_RANGE            INT8_C(-7)//未补偿温度和压力超范围

/*! @name 警告代码 */
#define BMP2_W_MIN_TEMP                               INT8_C(1)//最小温度
#define BMP2_W_MAX_TEMP                               INT8_C(2)//最大温度
#define BMP2_W_MIN_PRES                               INT8_C(3)//最小压力
#define BMP2_W_MAX_PRES                               INT8_C(4)//最大压力

/*! @name 芯片ID */
#define BMP2_CHIP_ID                                  UINT8_C(0x58)

/*! @name I2C地址 */
#define BMP2_I2C_ADDR_PRIM                            UINT8_C(0x76)
#define BMP2_I2C_ADDR_SEC                             UINT8_C(0x77)

/*! @name 寄存器地址 */
#define BMP2_REG_CHIP_ID                              UINT8_C(0xD0)//芯片ID寄存器
#define BMP2_REG_SOFT_RESET                           UINT8_C(0xE0)//复位寄存器
#define BMP2_REG_STATUS                               UINT8_C(0xF3)//状态寄存器
#define BMP2_REG_CTRL_MEAS                            UINT8_C(0xF4)//测量控制寄存器
#define BMP2_REG_CONFIG                               UINT8_C(0xF5)//配置寄存器
#define BMP2_REG_PRES_MSB                             UINT8_C(0xF7)//压力原始数据高字节
#define BMP2_REG_PRES_LSB                             UINT8_C(0xF8)//压力原始数据低字节
#define BMP2_REG_PRES_XLSB                            UINT8_C(0xF9)//压力原始数据最低字节
#define BMP2_REG_TEMP_MSB                             UINT8_C(0xFA)//温度原始数据高字节
#define BMP2_REG_TEMP_LSB                             UINT8_C(0xFB)//温度原始数据低字节
#define BMP2_REG_TEMP_XLSB                            UINT8_C(0xFC)//温度原始数据最低字节

/* 校准参数寄存器地址 */
#define BMP2_REG_DIG_T1_LSB                           UINT8_C(0x88)//温度标定值dig_T1低字节
#define BMP2_REG_DIG_T1_MSB                           UINT8_C(0x89)//温度标定值dig_T1高字节
#define BMP2_REG_DIG_T2_LSB                           UINT8_C(0x8A)//温度标定值dig_T2低字节
#define BMP2_REG_DIG_T2_MSB                           UINT8_C(0x8B)//温度标定值dig_T2高字节
#define BMP2_REG_DIG_T3_LSB                           UINT8_C(0x8C)//温度标定值dig_T3低字节
#define BMP2_REG_DIG_T3_MSB                           UINT8_C(0x8D)//温度标定值dig_T3高字节
#define BMP2_REG_DIG_P1_LSB                           UINT8_C(0x8E)//压力标定值dig_P1低字节
#define BMP2_REG_DIG_P1_MSB                           UINT8_C(0x8F)//压力标定值dig_P1高字节
#define BMP2_REG_DIG_P2_LSB                           UINT8_C(0x90)//压力标定值dig_P2低字节
#define BMP2_REG_DIG_P2_MSB                           UINT8_C(0x91)//压力标定值dig_P2高字节
#define BMP2_REG_DIG_P3_LSB                           UINT8_C(0x92)//压力标定值dig_P3低字节
#define BMP2_REG_DIG_P3_MSB                           UINT8_C(0x93)//压力标定值dig_P3高字节
#define BMP2_REG_DIG_P4_LSB                           UINT8_C(0x94)//压力标定值dig_P4低字节
#define BMP2_REG_DIG_P4_MSB                           UINT8_C(0x95)//压力标定值dig_P4高字节
#define BMP2_REG_DIG_P5_LSB                           UINT8_C(0x96)//压力标定值dig_P5低字节
#define BMP2_REG_DIG_P5_MSB                           UINT8_C(0x97)//压力标定值dig_P5高字节
#define BMP2_REG_DIG_P6_LSB                           UINT8_C(0x98)//压力标定值dig_P6低字节
#define BMP2_REG_DIG_P6_MSB                           UINT8_C(0x99)//压力标定值dig_P6高字节
#define BMP2_REG_DIG_P7_LSB                           UINT8_C(0x9A)//压力标定值dig_P7低字节
#define BMP2_REG_DIG_P7_MSB                           UINT8_C(0x9B)//压力标定值dig_P7高字节
#define BMP2_REG_DIG_P8_LSB                           UINT8_C(0x9C)//压力标定值dig_P8低字节
#define BMP2_REG_DIG_P8_MSB                           UINT8_C(0x9D)//压力标定值dig_P8高字节
#define BMP2_REG_DIG_P9_LSB                           UINT8_C(0x9E)//压力标定值dig_P9低字节
#define BMP2_REG_DIG_P9_MSB                           UINT8_C(0x9F)//压力标定值dig_P9高字节

/*! @name 接口设置 */
#define BMP2_SPI_RD_MASK                              UINT8_C(0x80)//SPI接口读掩码
#define BMP2_SPI_WR_MASK                              UINT8_C(0x7F)//SPI接口写掩码

/*! @name 延迟定义 */
#define BMP2_DELAY_US_STARTUP_TIME                    UINT8_C(2000)

/*! @name 电源模式 */
#define BMP2_POWERMODE_SLEEP                          UINT8_C(0x00)
#define BMP2_POWERMODE_FORCED                         UINT8_C(0x01)
#define BMP2_POWERMODE_NORMAL                         UINT8_C(0x03)

#define BMP2_POWERMODE_POS                            UINT8_C(0x00)//位置
#define BMP2_POWERMODE_MSK                            UINT8_C(0x03)//掩码

/*! @name 软复位命令 */
#define BMP2_SOFT_RESET_CMD                           UINT8_C(0xB6)

/*! @name 数据长度 */
#define BMP2_P_T_LEN                                  UINT8_C(0X06)

/*! @name 整型(32位、64位)和浮点型温度值范围 */
#define BMP2_MIN_TEMP_INT                             INT32_C(-4000)
#define BMP2_MAX_TEMP_INT                             INT32_C(8500)
#define BMP2_MIN_TEMP_DOUBLE                          -40.0f
#define BMP2_MAX_TEMP_DOUBLE                          85.0f

/*! @name 整型(32位、64位)和浮点型压力值范围 */
#define BMP2_MIN_PRES_32INT                           UINT32_C(30000)
#define BMP2_MAX_PRES_32INT                           UINT32_C(110000)
#define BMP2_MIN_PRES_64INT                           UINT32_C(30000 * 256)
#define BMP2_MAX_PRES_64INT                           UINT32_C(110000 * 256)
#define BMP2_MIN_PRES_DOUBLE                          30000.0f
#define BMP2_MAX_PRES_DOUBLE                          110000.0f

/*! @name 正常模式下两次测量之间的待机时间 */
#define BMP2_ODR_0_5_MS                               UINT8_C(0x00)
#define BMP2_ODR_62_5_MS                              UINT8_C(0x01)
#define BMP2_ODR_125_MS                               UINT8_C(0x02)
#define BMP2_ODR_250_MS                               UINT8_C(0x03)
#define BMP2_ODR_500_MS                               UINT8_C(0x04)
#define BMP2_ODR_1000_MS                              UINT8_C(0x05)
#define BMP2_ODR_2000_MS                              UINT8_C(0x06)
#define BMP2_ODR_4000_MS                              UINT8_C(0x07)

/*! @name 过采样宏定义 */
#define BMP2_OS_NONE                                  UINT8_C(0x00)
#define BMP2_OS_1X                                    UINT8_C(0x01)
#define BMP2_OS_2X                                    UINT8_C(0x02)
#define BMP2_OS_4X                                    UINT8_C(0x03)
#define BMP2_OS_8X                                    UINT8_C(0x04)
#define BMP2_OS_16X                                   UINT8_C(0x05)

#define BMP2_OS_TEMP_POS                              UINT8_C(0x05)//位置
#define BMP2_OS_TEMP_MSK                              UINT8_C(0xE0)//掩码
#define BMP2_OS_PRES_POS                              UINT8_C(0x02)//位置
#define BMP2_OS_PRES_MSK                              UINT8_C(0x1C)//掩码

/*! @name 过采样模式 */
#define BMP2_OS_MODE_ULTRA_LOW_POWER                  UINT8_C(0x00)
#define BMP2_OS_MODE_LOW_POWER                        UINT8_C(0x01)
#define BMP2_OS_MODE_STANDARD_RESOLUTION              UINT8_C(0x02)
#define BMP2_OS_MODE_HIGH_RESOLUTION                  UINT8_C(0x03)
#define BMP2_OS_MODE_ULTRA_HIGH_RESOLUTION            UINT8_C(0x04)

/*! @name 前端电路滤波系数宏 */
#define BMP2_FILTER_OFF                               UINT8_C(0x00)
#define BMP2_FILTER_COEFF_2                           UINT8_C(0x01)
#define BMP2_FILTER_COEFF_4                           UINT8_C(0x02)
#define BMP2_FILTER_COEFF_8                           UINT8_C(0x03)
#define BMP2_FILTER_COEFF_16                          UINT8_C(0x04)

#define BMP2_FILTER_POS                               UINT8_C(0x02)//位置
#define BMP2_FILTER_MSK                               UINT8_C(0x1C)//掩码

/*! @name SPI三线宏 */
#define BMP2_SPI3_WIRE_ENABLE                         UINT8_C(0x01)
#define BMP2_SPI3_WIRE_DISABLE                        UINT8_C(0x00)

#define BMP2_SPI3_ENABLE_POS                          UINT8_C(0x00)//位置
#define BMP2_SPI3_ENABLE_MSK                          UINT8_C(0x01)//掩码

/*! @name 测量状态宏 */
#define BMP2_MEAS_DONE                                UINT8_C(0x00)
#define BMP2_MEAS_ONGOING                             UINT8_C(0x01)

#define BMP2_STATUS_MEAS_POS                          UINT8_C(0x03)//位置
#define BMP2_STATUS_MEAS_MSK                          UINT8_C(0x08)//掩码

/*! @name NVM更新状态 */
#define BMP2_IM_UPDATE_DONE                           UINT8_C(0x00)
#define BMP2_IM_UPDATE_ONGOING                        UINT8_C(0x01)

#define BMP2_STATUS_IM_UPDATE_POS                     UINT8_C(0x00)//位置
#define BMP2_STATUS_IM_UPDATE_MSK                     UINT8_C(0x01)//掩码

/*! @name 待机时间宏 */
#define BMP2_STANDBY_DURN_POS                         UINT8_C(0x05)//位置
#define BMP2_STANDBY_DURN_MSK                         UINT8_C(0xE0)//掩码

/*! @name 校准参数的相对位置 */
#define BMP2_DIG_T1_LSB_POS                           UINT8_C(0)
#define BMP2_DIG_T1_MSB_POS                           UINT8_C(1)
#define BMP2_DIG_T2_LSB_POS                           UINT8_C(2)
#define BMP2_DIG_T2_MSB_POS                           UINT8_C(3)
#define BMP2_DIG_T3_LSB_POS                           UINT8_C(4)
#define BMP2_DIG_T3_MSB_POS                           UINT8_C(5)
#define BMP2_DIG_P1_LSB_POS                           UINT8_C(6)
#define BMP2_DIG_P1_MSB_POS                           UINT8_C(7)
#define BMP2_DIG_P2_LSB_POS                           UINT8_C(8)
#define BMP2_DIG_P2_MSB_POS                           UINT8_C(9)
#define BMP2_DIG_P3_LSB_POS                           UINT8_C(10)
#define BMP2_DIG_P3_MSB_POS                           UINT8_C(11)
#define BMP2_DIG_P4_LSB_POS                           UINT8_C(12)
#define BMP2_DIG_P4_MSB_POS                           UINT8_C(13)
#define BMP2_DIG_P5_LSB_POS                           UINT8_C(14)
#define BMP2_DIG_P5_MSB_POS                           UINT8_C(15)
#define BMP2_DIG_P6_LSB_POS                           UINT8_C(16)
#define BMP2_DIG_P6_MSB_POS                           UINT8_C(17)
#define BMP2_DIG_P7_LSB_POS                           UINT8_C(18)
#define BMP2_DIG_P7_MSB_POS                           UINT8_C(19)
#define BMP2_DIG_P8_LSB_POS                           UINT8_C(20)
#define BMP2_DIG_P8_MSB_POS                           UINT8_C(21)
#define BMP2_DIG_P9_LSB_POS                           UINT8_C(22)
#define BMP2_DIG_P9_MSB_POS                           UINT8_C(23)
#define BMP2_DIG_P10_POS                              UINT8_C(24)
#define BMP2_CALIB_DATA_SIZE                          UINT8_C(25)//校准数据大小

/*! @brief 校准值的最小值和最大值宏 */
#define BMP2_ST_DIG_T1_MIN                            UINT16_C(19000)
#define BMP2_ST_DIG_T1_MAX                            UINT16_C(35000)
#define BMP2_ST_DIG_T2_MIN                            UINT16_C(22000)
#define BMP2_ST_DIG_T2_MAX                            UINT16_C(30000)
#define BMP2_ST_DIG_T3_MIN                            INT16_C(-3000)
#define BMP2_ST_DIG_T3_MAX                            INT16_C(-1000)
#define BMP2_ST_DIG_P1_MIN                            UINT16_C(30000)
#define BMP2_ST_DIG_P1_MAX                            UINT16_C(42000)
#define BMP2_ST_DIG_P2_MIN                            INT16_C(-12970)
#define BMP2_ST_DIG_P2_MAX                            INT16_C(-8000)
#define BMP2_ST_DIG_P3_MIN                            INT16_C(-5000)
#define BMP2_ST_DIG_P3_MAX                            UINT16_C(8000)
#define BMP2_ST_DIG_P4_MIN                            INT16_C(-10000)
#define BMP2_ST_DIG_P4_MAX                            UINT16_C(18000)
#define BMP2_ST_DIG_P5_MIN                            INT16_C(-500)
#define BMP2_ST_DIG_P5_MAX                            UINT16_C(1100)
#define BMP2_ST_DIG_P6_MIN                            INT16_C(-1000)
#define BMP2_ST_DIG_P6_MAX                            UINT16_C(1000)
#define BMP2_ST_DIG_P7_MIN                            INT16_C(-32768)
#define BMP2_ST_DIG_P7_MAX                            UINT16_C(32767)
#define BMP2_ST_DIG_P8_MIN                            INT16_C(-30000)
#define BMP2_ST_DIG_P8_MAX                            UINT16_C(10000)
#define BMP2_ST_DIG_P9_MIN                            INT16_C(-10000)
#define BMP2_ST_DIG_P9_MAX                            UINT16_C(30000)

/*! @brief 未校准值的最大值和最小值宏 */
/* 0x00000 is minimum output value */
#define BMP2_ST_ADC_T_MIN                             INT32_C(0x00000)//温度最小值

/* 0xFFFF0 is maximum 20-bit output value without over sampling */
#define BMP2_ST_ADC_T_MAX                             INT32_C(0xFFFF0)//温度最大值

/* 0x00000 is minimum output value */
#define BMP2_ST_ADC_P_MIN                             INT32_C(0x00000)//压力最小值

/* 0xFFFF0 is maximum 20-bit output value without over sampling */
#define BMP2_ST_ADC_P_MAX                             INT32_C(0xFFFF0)//压力最大值

/*! @brief 用于读出API修订版号的宏 */
/* 保存自定义调整值的寄存器 */
#define BMP2_ST_TRIMCUSTOM_REG                        UINT8_C(0x87)
#define BMP2_ST_TRIMCUSTOM_REG_APIREV_POS             UINT8_C(1)//位置
#define BMP2_ST_TRIMCUSTOM_REG_APIREV_MSK             UINT8_C(0x06)//掩码
#define BMP2_ST_TRIMCUSTOM_REG_APIREV_LEN             UINT8_C(2)//长度
#define BMP2_ST_TRIMCUSTOM_REG_APIREV_REG             BMP2_ST_TRIMCUSTOM_REG//寄存器地址

/* 支持的最高API版本为版本0.xx */
#define BMP2_ST_MAX_APIREVISION                       UINT8_C(0x00)

#define BMP2_MSBLSB_TO_U16(msb, lsb)                  (((uint16_t)msb << 8) | ((uint16_t)lsb))//高低字节换位用宏

/*! @name 位切片宏 */
#define BMP2_GET_BITS(reg_data, bitname)              ((reg_data & bitname##_MSK) >> bitname##_POS)
#define BMP2_SET_BITS(reg_data, bitname, val)         ((reg_data & ~bitname##_MSK) | ((val << bitname##_POS) & bitname##_MSK))
#define BMP2_SET_BITS_POS_0(reg_data, bitname, data)  ((reg_data & ~(bitname##_MSK)) | (data & bitname##_MSK))
#define BMP2_GET_BITS_POS_0(reg_data, bitname)        (reg_data & (bitname##_MSK))

/******************************************************************************/
/*! @name           函数指针                             */
/******************************************************************************/
#ifndef BMP2_INTF_RET_TYPE
#define BMP2_INTF_RET_TYPE                            int8_t
#endif

#ifndef BMP2_INTF_RET_SUCCESS
#define BMP2_INTF_RET_SUCCESS                         INT8_C(0)
#endif

/*!
 * @brief 总线通信函数指针，应映射到用户的平台特定读函数
 *
 * @param[in]     reg_addr : 传感器的8位寄存器地址
 * @param[out]    reg_data : 存储寄存器数据的地址指针
 * @param[in]     length   : reg_data数组的长度
 * @param[in,out] intf_ptr : 可启用与接口相关的回调的描述符链接的空指针
 * @retval 0 for Success
 * @retval Non-zero for Failure
 */
typedef BMP2_INTF_RET_TYPE (*bmp2_read_fptr_t)(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);

/*!
 * @brief 总线通信函数指针，应映射到用户的平台特定写函数
 *
 * @param[in]     reg_addr : 传感器的8位寄存器地址
 * @param[out]    reg_data : 存储寄存器数据的地址指针
 * @param[in]     length   : reg_data数组的长度
 * @param[in,out] intf_ptr : 可启用与接口相关的回调的描述符链接的空指针
 * @retval 0 for Success
 * @retval Non-zero for Failure
 */
typedef BMP2_INTF_RET_TYPE (*bmp2_write_fptr_t)(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length,void *intf_ptr);

/*!
 * @brief 需要映射到用户的延迟函数的延迟函数指针
 *
 * @param period - 以微秒为单位的时间
 * @param[in,out] intf_ptr : 可启用与接口相关的回调的描述符链接的空指针
 */
typedef void (*bmp2_delay_us_fptr_t)(uint32_t period, void *intf_ptr);

/******************************************************************************/
/*!  @name         枚举声明                                  */
/******************************************************************************/

/*!
 * @brief 用于定义BMP2传感器接口的枚举
 */
enum bmp2_intf 
{
    BMP2_SPI_INTF,/*! SPI interface */
    BMP2_I2C_INTF/*! I2C interface */
};

/******************************************************************************/
/*!  @name         结构声明                             */
/******************************************************************************/

/*!
 * @brief 校准参数的结构体
 */
struct bmp2_calib_param
{
    /*! 温度数据的校准参数 */
    uint16_t dig_t1;/*! Calibration t1 data */
    int16_t dig_t2;/*! Calibration t2 data */
    int16_t dig_t3;/*! Calibration t3 data */

    /*! 压力数据的校准参数 */
    uint16_t dig_p1;/*! Calibration p1 data */
    int16_t dig_p2;/*! Calibration p2 data */
    int16_t dig_p3;/*! Calibration p3 data */
    int16_t dig_p4;/*! Calibration p4 data */
    int16_t dig_p5;/*! Calibration p5 data */
    int16_t dig_p6;/*! Calibration p6 data */
    int16_t dig_p7;/*! Calibration p7 data */
    int16_t dig_p8;/*! Calibration p8 data */
    int16_t dig_p9;/*! Calibration p9 data */
    int8_t dig_p10;/*! Calibration p10 data */
    int32_t t_fine;/*! 高分辨率温度值 */
};

/*!
 * @brief 传感器配置结构体
 */
struct bmp2_config
{
    uint8_t os_temp;/*! 温度数据的过采样 */
    uint8_t os_pres;/*! 压力数据的过采样 */
    uint8_t odr;/*! 通过设置待机时长设置ODR */
    uint8_t os_mode;/*! 过采样模式 */
    uint8_t filter;/*! IIR滤波器的时间常数 */
    uint8_t spi3w_en;/*! 启用3线SPI接口 */
};

/*!
 * @brief 传感器状态结构体
 */
struct bmp2_status
{
    uint8_t measuring;/*! 数据寄存器的状态 */
    uint8_t im_update;/*! NVM更新状态 */
};

/*!
 * @brief 未补偿数据结构体
 */
struct bmp2_uncomp_data
{
    int32_t temperature;/*! 未补偿的温度数据 */
    uint32_t pressure;/*! 未补偿的压力数据 */
};

/*!
 * @brief 由温度和压力组成的BMP2传感器结构体
 */
#ifdef BMP2_DOUBLE_COMPENSATION
struct bmp2_data
{
    double pressure;/*! 补偿后的压力 */
    double temperature;/*! 补偿后的温度 */
};
#else
struct bmp2_data
{
    uint32_t pressure;/*! 补偿后的压力 */
    int32_t temperature;/*! 补偿后的温度 */
};
#endif

/*!
 * @brief API设备结构体
 */
struct bmp2_dev
{
    uint8_t                 chip_id;/*! 芯片 Id */
    enum bmp2_intf          intf;/*! SPI/I2C接口选择 */
    void                    *intf_ptr;/*!接口指针允许用户在硬件实现读写接口时链接自己的接口描述符 */
    BMP2_INTF_RET_TYPE      intf_rslt;/*! 保存读/写函数结果的变量 */
    bmp2_read_fptr_t        read;/*! 总线读取函数指针 */
    bmp2_write_fptr_t       write;/*! 总线写入函数指针 */
    bmp2_delay_us_fptr_t    delay_us;/*! 延迟(以us为单位)函数指针 */
    uint8_t                 power_mode;/*! 传感器的电源模式 */
    struct bmp2_calib_param calib_param;/*! 校准参数的结构体' */
};

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _BMP2_DEFS_H */
