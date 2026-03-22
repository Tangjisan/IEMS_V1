# BMP2 sensor API

### Sensor overview 传感器概述

BMP2 is an absolute barometric pressure sensor especially designed for mobile applications.
The sensor module is housed in an extremely compact package. 
Its small dimensions and its low power consumption allow the implementation in battery driven devices such as 
mobile phones, GPS modules or watches. 
BMP2 is based on Bosch's proven Piezo-resistive pressure sensor technology featuring high accuracy and linearity as 
well as long term stability and high EMC robustness.

BMP2是一款专为移动应用设计的绝对气压传感器。
传感器模块安装在极其紧凑的封装中。
它的小尺寸和低功耗允许在电池驱动的设备中实现，如手机、GPS模块或手表。
BMP2基于博世久经考验的压阻式压力传感器技术，具有高精度、高线性度、长期稳定性和高EMC稳健性。

### Sensor support 传感器支持
BMP28X sensor driver supports the following Bosch MEMS sensors

BMP28X传感器驱动程序支持以下博世MEMS传感器
* BMP280

### Target Application 目标应用程序
- Handsets such as mobile phones, tablet PCs, GPS devices
- Navigation systems
- Home weather stations
- Flying toys
- Watches
- 手机、平板电脑、GPS设备等手持设备
- 导航系统
- 家庭气象站
- 飞行玩具
- 手表

### Features 特征
- Pressure
- Temperature
- 压力
- 温度

### Important links 重要链接

- [BMP280 product page](https://www.bosch-sensortec.com/products/environmental-sensors/pressure-sensors/bmp280/)
- [BMP280 datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf)
- [BMP280 shuttle board flyer](https://www.bosch-sensortec.com/media/boschsensortec/downloads/shuttle_board_flyer/bst-dhw-fl033.pdf)
- [Community support page](https://community.bosch-sensortec.com)

### Examples 示例程序说明
- common:由博世(Bosch)提供的示例程序，将BMP280驱动映射到实际物理硬件上(基于coines平台)。
- sensor_data:由博世(Bosch)提供的示例程序，演示了BMP280驱动程序的使用方法。
- BMP280_IIC:在STM32F103ZET6上的移植，基于正点原子精英开发板（MDK5.25+CubeMX6.0.1），IIC接口
- BMP280_SPI:在STM32F103ZET6上的移植，基于正点原子精英开发板（MDK5.25+CubeMX6.0.1），SPI接口

### Known problem 已知问题 
- 1.IIC示例：受限于开发的硬件限制（主要是懒），IIC接口未加上拉电阻，100K的通信速率下，大概率出现开发板按键复位后温度、压力读数异常，断电再上电就恢复。速度降至10K以下基本不出现
---
#### Copyright (C) 2021 Bosch Sensortec GmbH