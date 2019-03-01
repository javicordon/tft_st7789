/**
 * @file i2cdev.h
 *
 * ESP-IDF I2C master thread-safe functions for communication with I2C slave
 *
 * Copyright (C) 2018 Ruslan V. Uss (https://github.com/UncleRus)
 * MIT Licensed as described in the file LICENSE
 */
#ifndef __I2CDEV_H__
#define __I2CDEV_H__

#include <driver/i2c.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * I2C device descriptor
 */
typedef struct
{
    i2c_port_t port;
    i2c_config_t cfg;
    uint8_t addr;       //!< Unshifted address
    SemaphoreHandle_t mutex;
} i2c_dev_t;

void i2c_slave(void* i2c_datas);

#ifdef __cplusplus
}
#endif

#endif /* __I2CDEV_H__ */
