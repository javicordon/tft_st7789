/**
 * @file i2cdev.c
 *
 * ESP-IDF I2C master thread-safe functions for communication with I2C slave
 *
 * Copyright (C) 2018 Ruslan V. Uss (https://github.com/UncleRus)
 * Copyright (C) 2018 Javier Cordon Highly modified, deleted code not needed and duplicating values and functions.
 * MIT Licensed as described in the file LICENSE
 */

#include "i2c_slave.h"
#include <string.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "driver/i2c.h"

#define CONFIG_I2CDEV_TIMEOUT 160000
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE  0                /*!< I2C master do not need buffer */

static const char *TAG = "I2C_DEV";

#define DATA_LENGTH 256                  /*!< Data buffer length of test buffer */
#define BUFF 1
#define RW_TEST_LENGTH 3               /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000 /*!< delay time between different test items */

#define I2C_SLAVE_SCL_IO           26               /*!<gpio number for i2c slave clock  */
#define I2C_SLAVE_SDA_IO           4               /*!<gpio number for i2c slave data */
#define I2C_SLAVE_NUM              I2C_NUM_0        /*!<I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN       (2*DATA_LENGTH)  /*!<I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN       (2*DATA_LENGTH)  /*!<I2C slave rx buffer size */

#define ESP_SLAVE_ADDR                     0x64             /*!< ESP32 slave address, you can set any 7bit value */

#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */

SemaphoreHandle_t print_mux = NULL;

/**
 * @brief i2c slave initialization
 */
static esp_err_t i2c_slave_init()
{
    int i2c_slave_port = I2C_SLAVE_NUM;
    i2c_config_t conf_slave;
    conf_slave.sda_io_num = I2C_SLAVE_SDA_IO;
    conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf_slave.scl_io_num = I2C_SLAVE_SCL_IO;
    conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf_slave.mode = I2C_MODE_SLAVE;
    conf_slave.slave.addr_10bit_en = 0;
    conf_slave.slave.slave_addr = ESP_SLAVE_ADDR;
    i2c_param_config(i2c_slave_port, &conf_slave);
    return i2c_driver_install(i2c_slave_port, conf_slave.mode,
                              I2C_SLAVE_RX_BUF_LEN,
                              I2C_SLAVE_TX_BUF_LEN, 0);
}

/**
 * @brief test function to show buffer
 */
static void disp_buf(uint32_t *buf, int len, int signal_no)
{
	int i;
	//printf("S%i=[",signal_no);
    for (i = 0; i < len; i++) {
        printf("%i\n", buf[i]);
        //vTaskDelay(50 / portTICK_RATE_MS);
        //if ((i + 1) % 8 == 0) {
        //    printf("\n");
        //}
    }
    //printf("]\n");
}

void i2c_slave(void* i2c_datas)
{
	uint32_t* i2c_data =((uint32_t*)i2c_datas);
	vTaskDelay(7000 / portTICK_RATE_MS);
	uint8_t *data = (uint8_t *)malloc(DATA_LENGTH);
	int size;
	int32_t pos = 0;
	uint32_t *posi = (uint32_t *)calloc(BUFF,sizeof(int32_t));
	assert(posi!=NULL);
	int k=0;
	int l=1;
	int signal_number=0;
	ESP_ERROR_CHECK(i2c_slave_init());
	ESP_LOGI(TAG,"Started I2C Slave");
	while (1){
		size = i2c_slave_read_buffer(I2C_SLAVE_NUM, data, RW_TEST_LENGTH, 50 / portTICK_RATE_MS);
		if(size>0){
			pos = (data[0]<<16|data[1]<<8|data[2])&0xFFFFFF;
			posi[k++] = pos;
			//ESP_LOGI(TAG,"%x %x %x\t%i",data[0],data[1],data[2], pos);
			//ESP_LOGI(TAG,"%i",pos);
			if (k==BUFF){
				k=0;
				//disp_buf(posi,BUFF,signal_number++);
			}
			i2c_data[0]=l;
			i2c_data[l] = pos;
			l++;
			if(l==320+1){
				l=1;
			}

		}
		//vTaskDelay(5 / portTICK_RATE_MS);
	}

}



