#include "i2c.h"
void i2c_init()
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

}

void i2c_write(uint8_t addr, uint8_t reg, uint8_t val)
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
uint8_t i2c_read(uint8_t addr, uint8_t reg)
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
void i2c_write_data(uint8_t slaver_addr, uint8_t reg_addr, uint8_t *data, uint16_t size)
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
    i2c_master_cmd_begin(I2C_NUM_1, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}

void i2c_read_data(uint8_t slaver_addr, uint8_t start, uint8_t *buffer, uint16_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | 0, 1);
    i2c_master_write(cmd, &start, 1, 1);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slaver_addr << 1) | 1, 1);
    if(size > 1) 
    {
        i2c_master_read(cmd, buffer, size - 1, 0);
    }
    i2c_master_read_byte(cmd, buffer + size - 1, 1); //the lastest byte will not give a ASK
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_1, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}