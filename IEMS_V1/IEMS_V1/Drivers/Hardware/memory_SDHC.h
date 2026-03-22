#ifndef MEMORY_SDHC_H
#define MEMORY_SDHC_H

#include "stm32f1xx_hal.h"
#include "ff.h"
#include "diskio.h"

#include "spi.h"
#include "gpio.h"

// SD卡SPI引脚定义（根据你的硬件修改）
#define SD_SPI_PORT        SPI1
#define SD_SPI_CLK_ENABLE  __HAL_RCC_SPI1_CLK_ENABLE()

#define SD_CS_PIN          _SDHC_SPI_CS_Pin
#define SD_CS_PORT         _SDHC_SPI_CS_GPIO_Port
#define SD_CS_CLK_ENABLE   __HAL_RCC_GPIOA_CLK_ENABLE()

#define SD_CS_HIGH()       HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_SET)
#define SD_CS_LOW()        HAL_GPIO_WritePin(SD_CS_PORT, SD_CS_PIN, GPIO_PIN_RESET)

// SD卡状态枚举
typedef enum {
    SD_OK = 0,
    SD_ERROR,
    SD_TIMEOUT,
    SD_NOT_DETECTED,
    SD_INIT_FAILED,
    SD_WRITE_PROTECTED
} SD_StatusTypeDef;

// 命名空间封装
namespace n_SDHC {
    // 基础功能
    SD_StatusTypeDef init(void);
    SD_StatusTypeDef read_sector(uint32_t sector, uint8_t *buffer, uint32_t count);
    SD_StatusTypeDef write_sector(uint32_t sector, const uint8_t *buffer, uint32_t count);
    uint32_t get_sector_count(void);
    uint32_t get_sector_size(void);
    
    // FATFS文件操作
    namespace n_FILE {
        FRESULT open_file(FIL *file, const char *path, BYTE mode);
        FRESULT read_file(FIL *file, void *buffer, UINT bytes_to_read, UINT *bytes_read);
        FRESULT write_file(FIL *file, const void *buffer, UINT bytes_to_write, UINT *bytes_written);
        FRESULT close_file(FIL *file);
        FRESULT create_file(const char *path);
        FRESULT delete_file(const char *path);
    }
    
    // DMA相关（可选，提升传输速度）
    namespace n_DMA {
        SD_StatusTypeDef init(void);
        SD_StatusTypeDef read_sector_dma(uint32_t sector, uint8_t *buffer, uint32_t count);
        SD_StatusTypeDef write_sector_dma(uint32_t sector, const uint8_t *buffer, uint32_t count);
    }
}

#endif
