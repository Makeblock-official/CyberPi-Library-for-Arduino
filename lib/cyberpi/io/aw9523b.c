#include "aw9523b.h"
#include <Arduino.h>
#include "../i2c/i2c.h"
uint8_t _portIn = AW9523B_P0_IN_STATE;
uint8_t _portOut = AW9523B_P0_OUT_STATE;
uint8_t pinDataP0 = 0;
uint8_t pinDataP1 = 0;
uint8_t pinModeP0 = 0xff;
uint8_t pinModeP1 = 0xff;
enum AW9523BPortMode portMode = MODE_OPEN_DRAIN;
static uint8_t s_i2c_address[2] = {0x5b,0x58};
void aw_init()
{
    uint8_t index = 0;
    i2c_write( s_i2c_address[index], REG_SWRST, 0x00); 
    vTaskDelay(10/portTICK_RATE_MS);
    // config all IO led mode
    i2c_write( s_i2c_address[index], REG_WORK_MODE_P0, 0x00); // default as gpio mode
    i2c_write( s_i2c_address[index], REG_WORK_MODE_P1, 0x00);
    // config Imax/1
    i2c_write( s_i2c_address[index], REG_CTRL, 0x00); 

    ////////////////////////////// init aw9523 chip 1
    // reset
    index = 1;
    i2c_write( s_i2c_address[index], REG_SWRST, 0x00); 
    vTaskDelay(10/portTICK_RATE_MS);

    i2c_write( s_i2c_address[index], configPort0, pinDataP0); // 00: default all as output.
    i2c_write( s_i2c_address[index], configPort1, pinDataP1); // 00: default all as output.

    i2c_write( s_i2c_address[index], REG_WORK_MODE_P0, 0xff); // default as gpio mode
    i2c_write( s_i2c_address[index], REG_WORK_MODE_P1, 0xf9); // 1 1 1 1 1 0 0 0:config as gpio/led mode
    // config Imax/1
    i2c_write( s_i2c_address[index], REG_CTRL, 0x00); 
}

int modifyBit(int currentByte, int position, int bit) // bit field, change position, change value
{ 
  int mask = 1 << position; 
  return (currentByte & ~mask) | ((bit << position) & mask); 
}
void aw_pinMode(enum aw_gpio_num_t pin, uint8_t mode)
{
    if(pin < 8)
    {
        pinModeP0  = modifyBit(pinModeP0, pin, !mode);
        i2c_write(s_i2c_address[1], 0x4, pinModeP0); // config register p0
    }
    else
    {
        pinModeP1 = modifyBit(pinModeP1 , (pin - 8), !mode);
        i2c_write(s_i2c_address[1], 0x5, pinModeP1); // config register p0
    }
}

void aw_digitalWrite(enum aw_gpio_num_t pin, uint8_t value)
{  
    if(pin<8)
    {  
        // for 02H register
        pinDataP0 = modifyBit(pinDataP0, pin, value); // change relavent bit in pin mode
        i2c_write(s_i2c_address[1],  0x2, pinDataP0); // config register p0
    }
    else
    {
        pinDataP1 = modifyBit(pinDataP1, pin-8, value); // change relavent bit in pin mode
        i2c_write(s_i2c_address[1],  0x3, pinDataP1); // config register p0
    }
}
bool aw_digitalRead(enum aw_gpio_num_t pin)
{
  uint8_t ret = 0;
  uint8_t reg_data = 0;  
  if(pin<8)
  {  
    reg_data = i2c_read(s_i2c_address[1], inputPort0);
    ret = (reg_data >> pin)&0x01;
  }
  else
  {
    reg_data = i2c_read(s_i2c_address[1], inputPort1);
    ret = (reg_data >> (pin-8))&0x01;
  }
  return 1-ret;
}
uint16_t aw_get_gpio()
{
    return i2c_read(s_i2c_address[1], inputPort0)+(i2c_read(s_i2c_address[1], inputPort1)<<8);
}
void aw_config_inout(enum AW9523BPort port, uint8_t inout)
{
    i2c_write(s_i2c_address[port],AW9523B_P0_CONF_STATE, inout);
}

void aw_config_led_gpio(enum AW9523BPort port, uint8_t ledGpio)
{
    i2c_write(s_i2c_address[port], AW9523B_P0_LED_MODE, ledGpio);
}

uint8_t aw_read(enum AW9523BPort port)
{
    return i2c_read(s_i2c_address[port], AW9523B_P0_IN_STATE);
}

void aw_write(enum AW9523BPort port, uint8_t data)
{
    i2c_write(s_i2c_address[port],AW9523B_P0_OUT_STATE, data);
}

void aw_reset(enum AW9523BPort port)
{
    i2c_write(s_i2c_address[port],AW9523B_REG_SOFT_RST, 0);
}
