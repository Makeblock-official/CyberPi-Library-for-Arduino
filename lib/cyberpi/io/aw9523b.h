#ifndef __AW9523B__
#define __AW9523B__

#include <stdint.h>
#include <stdlib.h>
#include "driver/i2c.h"
#define _REG(port, reg) (port == P0 ? reg : reg + 1)
/** Registers */
#define AW9523B_I2C_ADDRESS 0x58   ///< I2C base address for AW9523B
#define AW9523B_REG_ID 0x10        ///< id register
#define AW9523B_ID 0x23            ///< id value
#define AW9523B_P0_IN_STATE 0x00   ///< P0 port input state
#define AW9523B_P1_IN_STATE 0x01   ///< P1 port input state
#define AW9523B_P0_OUT_STATE 0x02  ///< P0 port output state
#define AW9523B_P1_OUT_STATE 0x03  ///< P1 port output state
#define AW9523B_P0_CONF_STATE 0x04 ///< P0 port config state
#define AW9523B_P1_CONF_STATE 0x05 ///< P1 port config state
#define AW9523B_REG_GLOB_CTR 0x11  ///< Global control register
#define AW9523B_P0_LED_MODE 0x12   ///< P0 port led mode switch register
#define AW9523B_P1_LED_MODE 0x13   ///< P1 port led mode switch register
#define AW9523B_REG_SOFT_RST 0x7F  ///< Soft reset register

#define I2C_SDA_PIN 19
#define I2C_SCL_PIN 18

#define AW_GPIO_MODE_INPUT  0
#define AW_GPIO_MODE_OUTPUT 1

/** AW9523B Port constants */
typedef enum AW9523BPort
{
    P0 = 0x00, // Port 0
    P1 = 0x01, // Port 1
}
AW9523BPort;

typedef enum aw_gpio_num_t{
    AW_P0_0 = 0x00,
    AW_P0_1 = 0x01,
    AW_P0_2 = 0x02,
    AW_P0_3 = 0x03,
    AW_P0_4 = 0x04,
    AW_P0_5 = 0x05,
    AW_P0_6 = 0x06,
    AW_P0_7 = 0x07,
    AW_P1_0 = 0x08,
    AW_P1_1 = 0x09,
    AW_P1_2 = 0x0a,
    AW_P1_3 = 0x0b,
    AW_P1_4 = 0x0c,
    AW_P1_5 = 0x0d,
    AW_P1_6 = 0x0e,
    AW_P1_7 = 0x0f,
}
aw_gpio_num_t;

typedef enum AW9523BPortMode
{
    MODE_OPEN_DRAIN = 0x00, // Port 0 open drain mode
    MODE_PUSH_PULL = 1 << 4 // Port 0 push pull mode
}
AW9523BPortMode;
#define I2C_MASTER_SCL_IO 18         /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 19        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master do not need buffer */


void aw_init(uint8_t address);

/**************************************************************************/
/*!
    @brief  Write a byte to an I2C register
    @param reg Register address
    @param val Value to write
    @return I2C transmission result 
*/
/**************************************************************************/
void aw_write_i2c(uint8_t addr, uint8_t reg, uint8_t val);

/**************************************************************************/
/*!
    @brief  Read a byte from an I2C register
    @param reg Register address
    @return Register value
*/
/**************************************************************************/
uint8_t aw_read_i2c(uint8_t addr, uint8_t reg);
int modifyBit(int currentByte, int position, int bit);
void aw_pinMode(enum aw_gpio_num_t pin, uint8_t mode);
void aw_digitalWrite(enum aw_gpio_num_t pin, uint8_t value);
void aw_config_inout(enum AW9523BPort port, uint8_t inout);

void aw_config_led_gpio(enum AW9523BPort port, uint8_t ledGpio);
uint8_t aw_read(enum AW9523BPort port);


void aw_write(enum AW9523BPort port, uint8_t data);

void aw_reset(enum AW9523BPort port);

#endif