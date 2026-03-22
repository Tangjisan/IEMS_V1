#include "bmp2_common.h"
#include "stdio.h"
#include <stdint.h>
#include "main.h"

/***********************************************
 *静态变量定义
************************************************/ 
#if BMP280_INTERFACE == BMP280_IIC
static uint8_t dev_addr;//保存I2C设备地址变量 */
#endif

/***********************************************
 * 用户功能函数
************************************************/ 
#if BMP280_INTERFACE == BMP280_IIC
/***********************************************
 *I2C读取功能映射到STM32 HAL库
 *函数名：BMP2_INTF_RET_TYPE bmp2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
 *参数：reg_addr-传感器的8位寄存器地址;  *reg_data-存储寄存器数据的地址指针;
        length-reg_data数组的长度;       intf_ptr-可启用与接口相关的回调的描述符链接的空指针
 *返回值：0-成功；1-失败
************************************************/ 
BMP2_INTF_RET_TYPE bmp2_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    return HAL_I2C_Mem_Read((I2C_HandleTypeDef*)intf_ptr, dev_addr, (uint16_t)reg_addr, 1, reg_data, (uint16_t)length, 5000);
}

/***********************************************
 *I2C写入函数映射到STM32 HAL库
 *函数名：BMP2_INTF_RET_TYPE bmp2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
 *参数：reg_addr-传感器的8位寄存器地址;  *reg_data-存储寄存器数据的地址指针;
        length-reg_data数组的长度;       intf_ptr-可启用与接口相关的回调的描述符链接的空指针
 *返回值：0-成功；1-失败
************************************************/ 
BMP2_INTF_RET_TYPE bmp2_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    return HAL_I2C_Mem_Write((I2C_HandleTypeDef*)intf_ptr, dev_addr, (uint16_t)reg_addr, 1, (uint8_t *)reg_data, (uint16_t)length,5000);
}

/***********************************************
 *I2C读取功能映射到STM32 HAL库 DMA自动转运版本
 *函数名：BMP2_INTF_RET_TYPE bmp2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
 *参数：reg_addr-传感器的8位寄存器地址;  *reg_data-存储寄存器数据的地址指针;
        length-reg_data数组的长度;       intf_ptr-可启用与接口相关的回调的描述符链接的空指针
 *返回值：0-成功；1-失败
************************************************/ 
BMP2_INTF_RET_TYPE bmp2_read_DMA(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    return HAL_I2C_Mem_Read_DMA((I2C_HandleTypeDef*)intf_ptr, dev_addr, (uint16_t)reg_addr, 1, reg_data, (uint16_t)length);
}

/***********************************************
 *I2C写入函数映射到STM32 HAL库 DMA自动转运版本
 *函数名：BMP2_INTF_RET_TYPE bmp2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
 *参数：reg_addr-传感器的8位寄存器地址;  *reg_data-存储寄存器数据的地址指针;
        length-reg_data数组的长度;       intf_ptr-可启用与接口相关的回调的描述符链接的空指针
 *返回值：0-成功；1-失败
************************************************/ 
BMP2_INTF_RET_TYPE bmp2_write_DMA(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    return HAL_I2C_Mem_Write_DMA((I2C_HandleTypeDef*)intf_ptr, dev_addr, (uint16_t)reg_addr, 1, (uint8_t *)reg_data, (uint16_t)length);
}

#endif
#if BMP280_INTERFACE == BMP280_SPI
/*!
 * 
 */
/***********************************************
 *SPI读取功能映射到STM32 HAL库
 *函数名：BMP2_INTF_RET_TYPE bmp2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
 *参数：reg_addr-传感器的8位寄存器地址;  *reg_data-存储寄存器数据的地址指针;
        length-reg_data数组的长度;       intf_ptr-可启用与接口相关的回调的描述符链接的空指针
 *返回值：0-成功；1-失败
************************************************/ 
BMP2_INTF_RET_TYPE bmp2_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    BMP2_INTF_RET_TYPE result;
    BMP280_CS_RESET();
    HAL_SPI_Transmit((SPI_HandleTypeDef *)intf_ptr, &reg_addr, 1, 5000);//写寄存器地址
    result = HAL_SPI_Receive((SPI_HandleTypeDef *)intf_ptr, reg_data, (uint16_t)length, 5000);//读数据
    BMP280_CS_SET();
    return result;
}

/***********************************************
 *SPI将函数映射写入STM32 HAL库
 *函数名：BMP2_INTF_RET_TYPE bmp2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
 *参数：reg_addr-传感器的8位寄存器地址;  *reg_data-存储寄存器数据的地址指针;
        length-reg_data数组的长度;       intf_ptr-可启用与接口相关的回调的描述符链接的空指针
 *返回值：0-成功；1-失败
************************************************/ 
BMP2_INTF_RET_TYPE bmp2_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    BMP2_INTF_RET_TYPE result;
    BMP280_CS_RESET();
    HAL_SPI_Transmit((SPI_HandleTypeDef *)intf_ptr, &reg_addr, 1, 5000);//写寄存器地址
    result = HAL_SPI_Transmit((SPI_HandleTypeDef *)intf_ptr, (uint8_t *)reg_data, (uint16_t)length, 5000);//写数据
    BMP280_CS_SET();
    return result;
}
#endif

/***********************************************
 *延迟函数映射到STM32 HAL库
 *函数名：void bmp2_delay_us(uint32_t period, void *intf_ptr)
 *参数：period-以微秒为单位的时间;  *intf_ptr-可启用与接口相关的回调的描述符链接的空指针;
 *返回值：0-成功；1-失败
************************************************/ 
void bmp2_delay_us(uint32_t period, void *intf_ptr)
{
    uint32_t delta;
    
    period = period * (SysTick->LOAD/(1000000));//获得延时经过的 tick 数
    delta = SysTick->VAL;//获得当前时间
    while (delta - SysTick->VAL< period);//循环获得当前时间，直到达到指定的时间后退出循环
}

/***********************************************
 *打印API的执行状态
 *函数名：void bmp2_error_codes_print_result(const char api_name[], int8_t rslt)
 *参数：api_name-API;  *intf_ptr-可启用与接口相关的回调的描述符链接的空指针;
 *返回值：0-成功；1-失败
************************************************/ 
/* void bmp2_error_codes_print_result(const char api_name[], int8_t rslt)
{
    if (rslt != BMP2_OK)
    {
        printf("%s\t", api_name);

        switch (rslt)
        {
            case BMP2_E_NULL_PTR:
                printf("Error [%d] : Null pointer error.", rslt);
                break;
            case BMP2_E_COM_FAIL:
                printf("Error [%d] : Communication failure error.", rslt);
                break;
            case BMP2_E_INVALID_LEN:
                printf("Error [%d] : Invalid length error.", rslt);
                break;
            case BMP2_E_DEV_NOT_FOUND:
                printf("Error [%d] : Device not found error.", rslt);
                break;
            case BMP2_E_UNCOMP_TEMP_RANGE:
                printf("Error [%d] : Uncompensated temperature data not in valid range error.", rslt);
                break;
            case BMP2_E_UNCOMP_PRESS_RANGE:
                printf("Error [%d] : Uncompensated pressure data not in valid range error.", rslt);
                break;
            case BMP2_E_UNCOMP_TEMP_AND_PRESS_RANGE:
                printf("Error [%d] : Uncompensated pressure data and uncompensated temperature data are not in valid range error.", rslt);
                break;
            default:
                printf("Error [%d] : Unknown error code\r\n", rslt);
                break;
        }
    }
} */


/***********************************************
 *BMP2结构体实例化,链接总线
 *函数名：int8_t bmp2_interface_selection(struct bmp2_dev *dev, void *Bus_Handle)
 *参数：*dev-bmp2_dev设备实例;
 *返回值：0-成功；1-失败
************************************************/ 
int8_t bmp2_interface_selection(struct bmp2_dev *dev, void *Bus_Handle)
{
    int8_t rslt = BMP2_OK;

    if (dev != NULL)
    {
        #if BMP280_INTERFACE == BMP280_IIC
        dev_addr = BMP280_I2C_ADDR;//I2C设备地址
        dev->intf = BMP2_I2C_INTF;//BMP280接口选择
        #endif
        #if BMP280_INTERFACE == BMP280_SPI
        dev->intf = BMP2_SPI_INTF;//BMP280接口选择
        #endif
        
        dev->read = bmp2_read;
        dev->write = bmp2_write;
        dev->intf_ptr = Bus_Handle;
        dev->delay_us = bmp2_delay_us;
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;
    }

    return rslt;
}

int8_t bmp2_interface_selection_DMA(struct bmp2_dev *dev, void *Bus_Handle)
{
    int8_t rslt = BMP2_OK;

    if (dev != NULL)
    {
        #if BMP280_INTERFACE == BMP280_IIC
        dev_addr = BMP280_I2C_ADDR;//I2C设备地址
        dev->intf = BMP2_I2C_INTF;//BMP280接口选择
        #endif
        #if BMP280_INTERFACE == BMP280_SPI
        dev->intf = BMP2_SPI_INTF;//BMP280接口选择
        #endif
        
        dev->read = bmp2_read_DMA;
        dev->write = bmp2_write_DMA;
        dev->intf_ptr = Bus_Handle;
        dev->delay_us = bmp2_delay_us;
    }
    else
    {
        rslt = BMP2_E_NULL_PTR;
    }

    return rslt;
}

