#include <string.h>
#include <stdint.h>
#include "es8218e.h"
#include "../i2c/i2c.h"

static const char *ES_TAG = "ES8218E_DRIVER";

#define ES_ASSERT(a, format, b, ...) \
  if ((a) != 0) { \
    ESP_LOGE(ES_TAG, format, ##__VA_ARGS__); \
    return b;\
  }

void es8218e_read_all()
{
  for(int16_t i = 0; i < 50; i++)
  {
    uint8_t reg = i2c_read(ES8218E_ADDR, i);
  }
}

void es8218e_reset(void)
{
  i2c_write(ES8218E_ADDR, ES8218E_RESET, 0x3f);
  vTaskDelay(1 / portTICK_RATE_MS);
  i2c_write(ES8218E_ADDR, ES8218E_RESET, 0x00);
  i2c_write(ES8218E_ADDR, ES8218E_RESET, 0x80);
}

void es8218e_start(void)
{
  i2c_write(ES8218E_ADDR, ES8218E_RESET, 0x3f);
  i2c_write(ES8218E_ADDR, ES8218E_RESET, 0x00);
  i2c_write(ES8218E_ADDR, ES8218E_CLOCK_MANAGER1, 0x10);
  vTaskDelay(1 / portTICK_RATE_MS);
  i2c_write(ES8218E_ADDR, ES8218E_CLOCK_MANAGER1, 0x00);
  i2c_write(ES8218E_ADDR, ES8218E_CLOCK_MANAGER1, 0x0f);    //slave mode
  i2c_write(ES8218E_ADDR, ES8218E_CLOCK_MANAGER2, 0x01);    //1
  i2c_write(ES8218E_ADDR, ES8218E_CLOCK_MANAGER3, 0x20);    //4096000/16000/8 = 32
  // mb_esp32_i2c_master_write_reg_t(ES8218E_ADDR, 0x06, 0x00);                      //
  i2c_write(ES8218E_ADDR, ES8218E_SERIAL_DATA_PORT, 0x0c);  //16 bit data, i2s format(0x0c）  //left justify 0x0d
  i2c_write(ES8218E_ADDR, ES8218E_ADC_CONTROL2, 0x18);      //high pass filter     //0x18 open   0x00 close
                                               //0x18(adc soft ramp and high pass filter) //0x10(adc soft ramp)
  i2c_write(ES8218E_ADDR, ES8218E_ADC_CONTROL6, 0xA0);      //ALCLVL=-1.5db
  i2c_write(ES8218E_ADDR, ES8218E_SYSTEM_CONTROL6, 0x30);   //POWER-INI time
  i2c_write(ES8218E_ADDR, ES8218E_SYSTEM_CONTROL7, 0x20);   //POWER-UP time
  i2c_write(ES8218E_ADDR, ES8218E_ADC_CONTROL10, 0x04);     //HPF slow setting coeff defalut value
  i2c_write(ES8218E_ADDR, ES8218E_ADC_CONTROL11, 0x04);     //HPF fast setting coeff defalut value
  i2c_write(ES8218E_ADDR, ES8218E_ADC_CONTROL1, 0x32);      //LIN2/RIN2, PGA=+18db(0x30)   PGA = 0db(0x20)   //0x20
                                               //低三位，0(0db) 1(3.5db) 2(6db) 3(9db) 4(12db) 5(15db) 6(18db)
  i2c_write(ES8218E_ADDR, ES8218E_SYSTEM_CONTROL1, 0x00);   //POWER ON
  i2c_write(ES8218E_ADDR, ES8218E_RESET, 0x80);             //IC start

  uint8_t pga_gain = 0xc0 | (uint8_t)((20.5 + 6.5)/1.5);
  i2c_write(ES8218E_ADDR, ES8218E_ADC_CONTROL4, pga_gain);  //ALC ON
  // mb_esp32_i2c_master_write_reg_t(ES8218E_ADDR, ES8218E_ADC_CONTROL4, 0x1c);   //ALC OFF
  i2c_write(ES8218E_ADDR, ES8218E_ADCVOLUMEL, 0x00);        //0db

}

void es8218e_stop( )
{
  i2c_write(ES8218E_ADDR, ES8218E_CLOCK_MANAGER1, 0x80); //enable dac
  i2c_write(ES8218E_ADDR, ES8218E_CLOCK_MANAGER1, 0x00); // 0x00 audio on LIN1&RIN1,  0x09 LIN2&RIN2
}


void es8218e_init(void)
{
  int16_t res = 0;
  return res;
}

void es8218e_config_fmt(es_i2s_fmt_t fmt)
{
  uint8_t reg = 0;
  reg = i2c_read(ES8218E_ADDR, ES8218E_SERIAL_DATA_PORT);
  reg = reg & 0xfc;
  i2c_write(ES8218E_ADDR, ES8218E_SERIAL_DATA_PORT, reg | fmt);
}