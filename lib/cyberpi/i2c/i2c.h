#ifndef __I2C__
#define __I2C__
#include <stdint.h>
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO 18         /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 19        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master do not need buffer */


void i2c_init();
void i2c_write(uint8_t addr, uint8_t reg, uint8_t val);
uint8_t i2c_read(uint8_t addr, uint8_t reg);
void i2c_write_data(uint8_t slaver_addr, uint8_t reg_addr, uint8_t *data, uint16_t size);
void i2c_read_data(uint8_t slaver_addr, uint8_t reg_addr, uint8_t *data, uint16_t size);
#endif