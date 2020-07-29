#include "gyro.h"
#include "../i2c/i2c.h"
static uint8_t* data_buf; 
static int16_t* acc_value;
static int16_t* gyro_value;
static float* gyro_data;
static float* acc_data;

static bool first_update = true;
static float acc_x_static = 0.0;
static float acc_y_static = 0.0;
static float acc_z_static = 0.0;
bool gyro_init()
{
    uint8_t chip_id = i2c_read( MPU6887_ADDR,  WHO_AM_I);
    if(chip_id !=  MPU6887_DEVICE_ID)
    {
      return false;
    }
    data_buf = (uint8_t*)malloc(14);
    acc_value = (int16_t*)malloc(6);
    gyro_value = (int16_t*)malloc(6);
    gyro_data = (float*)malloc(12);
    acc_data = (float*)malloc(12);
    /* close the sleep mode */
    i2c_write( MPU6887_ADDR, PWR_MGMT_1, 0x00);
    /* configurate the digital low pass filter */
    i2c_write( MPU6887_ADDR, CONFIG, 0x01);
    /* set the gyro scale to 500 deg/s */
    i2c_write( MPU6887_ADDR, GYRO_CONFIG, 0x08);
    /* set the acc scale to 4g */
    i2c_write( MPU6887_ADDR, ACCEL_CONFIG, 0x08);
    /* set the Sampling Rate   100Hz */
    i2c_write( MPU6887_ADDR, SMPLRT_DI, 0x09);
    return true;
}
bool gyro_read()
{
    i2c_read_data(MPU6887_ADDR, ACCEL_XOUT_H, data_buf, 14);
    acc_value[0] = (int16_t)((data_buf[0] << 8) | data_buf[1]);
    acc_value[1] = (int16_t)((data_buf[2] << 8) | data_buf[3]);
    acc_value[2] = (int16_t)((data_buf[4] << 8) | data_buf[5]);  
    
    gyro_value[0] = (int16_t)((data_buf[8] << 8)  | data_buf[9]);
    gyro_value[1] = (int16_t)((data_buf[10] << 8) | data_buf[11]);
    gyro_value[2] = (int16_t)((data_buf[12] << 8) | data_buf[13]);
   
      /* calculate other value by acc */
      float acc_filter = 0.8;
      if(first_update)
      {
        acc_x_static = acc_value[0];
        acc_y_static = acc_value[1];
        acc_z_static = acc_value[2];  
        first_update = false;
      }
      else
      {
        acc_x_static = acc_x_static * acc_filter + acc_value[0] * (1 - acc_filter);
        acc_y_static = acc_y_static * acc_filter + acc_value[1] * (1 - acc_filter);
        acc_z_static = acc_z_static * acc_filter + acc_value[2] * (1 - acc_filter);  
      }
      acc_data[0] = acc_x_static;
      acc_data[1] = acc_y_static;
      acc_data[2] = acc_z_static;

      gyro_data[0] = gyro_value[0] / GTRO_SENSITIVITY_DEFAULT;
      gyro_data[1] = gyro_value[1] / GTRO_SENSITIVITY_DEFAULT;
      gyro_data[2] = gyro_value[2] / GTRO_SENSITIVITY_DEFAULT;
    return true;
}
float get_acc_data(uint8_t axis)
{
  return acc_data[axis];
}
float get_gyro_data(uint8_t axis)
{
  return gyro_data[axis];
}
float get_gyro_roll()
{
  if(acc_data[2] > 0)
  {
    return atan2(acc_data[0], -sqrt(pow(acc_data[1], 2) + pow(acc_data[2], 2))) * 180 / PI;
    
  }
  else
  {
    return atan2(acc_data[0], sqrt(pow(acc_data[1], 2) + pow(acc_data[2], 2))) * 180 / PI;
  }
}
float get_gyro_pitch()
{
  if(acc_data[2] > 0)
  {
    return atan2(-acc_data[1], -sqrt(pow(acc_data[0], 2) + pow(acc_data[2], 2))) * 180 / PI;
  }
  else
  {
    return atan2(-acc_data[1], sqrt(pow(acc_data[0], 2) + pow(acc_data[2], 2))) * 180 / PI;
  }
}