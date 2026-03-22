#ifndef _BMP2_COMMON_H
#define _BMP2_COMMON_H

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include "bmp2.h"
#include "bmp2_defs.h"
#include "stm32f1xx_hal.h"

/***********************************************
 *接口选择
************************************************/ 
#define BMP280_IIC     0
#define BMP280_SPI     1

#define BMP280_INTERFACE   BMP280_IIC
#if BMP280_INTERFACE == BMP280_IIC
#define SDO_STATUS 0//定义SDO引脚状态
#endif

/***********************************************
 *IIC接口时的地址定义
************************************************/ 
#if BMP280_INTERFACE == BME280_IIC
    #if SDO_STATUS == 1
        #define BMP280_I2C_ADDR     (BMP2_I2C_ADDR_SEC << 1)
    #else
        #define BMP280_I2C_ADDR     (BMP2_I2C_ADDR_PRIM << 1)
    #endif
#endif

#if BMP280_INTERFACE == BMP280_SPI
#define BMP280_CS_SET() HAL_GPIO_WritePin(BMP280_CS_GPIO_Port,BMP280_CS_Pin,GPIO_PIN_SET);
#define BMP280_CS_RESET() HAL_GPIO_WritePin(BMP280_CS_GPIO_Port,BMP280_CS_Pin,GPIO_PIN_RESET);
#endif

/***********************************************
 *                 用户功能原型
************************************************/ 
BMP2_INTF_RET_TYPE bmp2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
BMP2_INTF_RET_TYPE bmp2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
BMP2_INTF_RET_TYPE bmp2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
BMP2_INTF_RET_TYPE bmp2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);

void bmp2_delay_us(uint32_t period_us, void *intf_ptr);
int8_t bmp2_interface_selection(struct bmp2_dev *dev, void *Bus_Handle);
//void bmp2_error_codes_print_result(const char api_name[], int8_t rslt);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _COMMON_H */
