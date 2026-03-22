# BMP280

## 代码

### conf_sensor

```C++

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
        // rslt = bmp2_get_regs(BMP2_REG_CTRL_MEAS, temp, 2, dev);//读取测量控制寄存器、配置寄存器
        rslt = BMP2_OK;
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

```

bmp2_soft_reset(进入睡眠模式) ->
