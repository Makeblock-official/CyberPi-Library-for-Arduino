#include "aw9523b.h"
#include <Arduino.h>
uint8_t _portIn = AW9523B_P0_IN_STATE;
uint8_t _portOut = AW9523B_P0_OUT_STATE;
uint8_t pinDataP0 = 0;
uint8_t pinDataP1 = 0;
uint8_t pinModeP0 = 0xff;
uint8_t pinModeP1 = 0xff;
enum AW9523BPortMode portMode = MODE_OPEN_DRAIN;
static uint8_t s_i2c_address[2] = {0x5b,0x58};
void aw_init(uint8_t address)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;   
    io_conf.mode = GPIO_MODE_DISABLE;//
    io_conf.pin_bit_mask = (1 << I2C_MASTER_SCL_IO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = (1 << I2C_MASTER_SDA_IO);
    gpio_config(&io_conf);

    int i2c_master_port = I2C_NUM_1;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = 0;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = 0;
    conf.master.clk_speed = 400000;
    i2c_driver_install(i2c_master_port, conf.mode,0,0,0);
    i2c_param_config(i2c_master_port, &conf);


    uint8_t index = 0;
    aw_write_i2c( s_i2c_address[index], REG_SWRST, 0x00); 
    vTaskDelay(10/portTICK_RATE_MS);
    // config all IO led mode
    aw_write_i2c( s_i2c_address[index], REG_WORK_MODE_P0, 0x00); // default as gpio mode
    aw_write_i2c( s_i2c_address[index], REG_WORK_MODE_P1, 0x00);
    // config Imax/1
    aw_write_i2c( s_i2c_address[index], REG_CTRL, 0x00); 

    ////////////////////////////// init aw9523 chip 1
    // reset
    index = 1;
    aw_write_i2c( s_i2c_address[index], REG_SWRST, 0x00); 
    vTaskDelay(10/portTICK_RATE_MS);

    aw_write_i2c( s_i2c_address[index], configPort0, pinDataP0); // 00: default all as output.
    aw_write_i2c( s_i2c_address[index], configPort1, pinDataP1); // 00: default all as output.

    aw_write_i2c( s_i2c_address[index], REG_WORK_MODE_P0, 0xff); // default as gpio mode
    aw_write_i2c( s_i2c_address[index], REG_WORK_MODE_P1, 0xf9); // 1 1 1 1 1 0 0 0:config as gpio/led mode
    // config Imax/1
    aw_write_i2c( s_i2c_address[index], REG_CTRL, 0x00); 
}

/**************************************************************************/
/*!
    @brief  Write a byte to an I2C register
    @param reg Register address
    @param val Value to write
    @return I2C transmission result 
*/
/**************************************************************************/
void aw_write_i2c(uint8_t addr, uint8_t reg, uint8_t val)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr<<1, 1);
    i2c_master_write_byte(cmd, reg, 1);
    i2c_master_write_byte(cmd, val, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}
void aw_write_data(int32_t port, uint8_t slaver_addr, uint8_t reg_addr, uint8_t *data, uint16_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();  // create a command link,command data will be added to this link and then sent at once
    i2c_master_start(cmd);                         // I2C logic has been packaged in these functions
    i2c_master_write_byte(cmd, (slaver_addr << 1), 1);
    i2c_master_write(cmd, &reg_addr, 1, 1);
    for(uint16_t i = 0; i < size; i++)
    {
        i2c_master_write(cmd, (data + i), 1, 1);
    }
    
    i2c_master_stop(cmd);
    esp_err_t res = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return res;
}
/**************************************************************************/
/*!
    @brief  Read a byte from an I2C register
    @param reg Register address
    @return Register value
*/
/**************************************************************************/
uint8_t aw_read_i2c(uint8_t addr, uint8_t reg)
{
    uint8_t data = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,  (addr << 1) | 0, 1);
    i2c_master_write_byte(cmd, reg, 1);
    // i2c_master_stop(cmd);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,  (addr << 1) | 1, 1);
    i2c_master_read_byte(cmd, &data, 1);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return data;
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
        aw_write_i2c(s_i2c_address[1], 0x4, pinModeP0); // config register p0
    }
    else
    {
        pinModeP1 = modifyBit(pinModeP1 , (pin - 8), !mode);
        aw_write_i2c(s_i2c_address[1], 0x5, pinModeP1); // config register p0
    }
}

void aw_digitalWrite(enum aw_gpio_num_t pin, uint8_t value)
{  
    if(pin<8)
    {  
        // for 02H register
        pinDataP0 = modifyBit(pinDataP0, pin, value); // change relavent bit in pin mode
        aw_write_i2c(s_i2c_address[1],  0x2, pinDataP0); // config register p0
    }
    else
    {
        pinDataP1 = modifyBit(pinDataP1, pin-8, value); // change relavent bit in pin mode
        aw_write_i2c(s_i2c_address[1],  0x3, pinDataP1); // config register p0
    }
}
bool aw_digitalRead(enum aw_gpio_num_t pin)
{
  uint8_t ret = 0;
  uint8_t reg_data = 0;  
  if(pin<8)
  {  
    reg_data = aw_read_i2c(s_i2c_address[1], inputPort0);
    ret = (reg_data >> pin)&0x01;
  }
  else
  {
    reg_data = aw_read_i2c(s_i2c_address[1], inputPort1);
    ret = (reg_data >> (pin-8))&0x01;
  }
  return 1-ret;
}
uint8_t aw_get_gpio()
{
    return aw_read_i2c(s_i2c_address[1], inputPort0)+aw_read_i2c(s_i2c_address[1], inputPort1);
}
void aw_config_inout(enum AW9523BPort port, uint8_t inout)
{
    aw_write_i2c(s_i2c_address[port],AW9523B_P0_CONF_STATE, inout);
}

void aw_config_led_gpio(enum AW9523BPort port, uint8_t ledGpio)
{
    aw_write_i2c(s_i2c_address[port], AW9523B_P0_LED_MODE, ledGpio);
}

uint8_t aw_read(enum AW9523BPort port)
{
    return aw_read_i2c(s_i2c_address[port], AW9523B_P0_IN_STATE);
}

void aw_write(enum AW9523BPort port, uint8_t data)
{
    aw_write_i2c(s_i2c_address[port],AW9523B_P0_OUT_STATE, data);
}

void aw_reset(enum AW9523BPort port)
{
    aw_write_i2c(s_i2c_address[port],AW9523B_REG_SOFT_RST, 0);
}
