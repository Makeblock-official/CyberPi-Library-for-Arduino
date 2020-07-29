#ifndef __GYRO__
#define __GYRO__
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#define MPU6887_ADDR 0x69
#define MPU6887_DEVICE_ID         (0x2E)

/********* REGISTERS ADDRESS *************/
#define XG_OFFS_TC_H        0x04
#define XG_OFFS_TC_L        0x05
#define YG_OFFS_TC_H        0x07
#define YG_OFFS_TC_L        0x08
#define ZG_OFFS_TC_H        0x0A
#define ZG_OFFS_TC_L        0x0B

#define SELF_TEST_X_ACCEL   0x0D
#define SELF_TEST_Y_ACCEL   0x0E
#define SELF_TEST_Z_ACCEL   0x0F

#define XG_OFFS_USRH        0x13
#define XG_OFFS_USRL        0x14
#define YG_OFFS_USRH        0x15
#define YG_OFFS_USRL        0x16
#define ZG_OFFS_USRH        0x17
#define ZG_OFFS_USRL        0x18

#define SMPLRT_DI           0x19

#define CONFIG              0x1A
#define GYRO_CONFIG         0x1B
#define ACCEL_CONFIG        0x1C
#define ACCEL_CONFIG_2      0x1D   
#define LP_MODE_CFG         0x1E

#define ACCEL_WOM_X_THR     0x20
#define ACCEL_WOM_Y_THR     0x21
#define ACCEL_WOM_Z_THR     0x22
#define FIFO_EN             0x23

#define FSYNC_INT           0x36
#define INT_PIN_CFG         0x37
#define INT_ENABLE          0x38
#define FIFO_WM_INT_STATUS  0x39
#define INTSTATU            0x3A

#define ACCEL_XOUT_H        0x3B
#define ACCEL_XOUT_L        0x3C
#define ACCEL_YOUT_H        0x3D
#define ACCEL_YOUT_L        0x3E
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40
#define TEMP_OUT_H          0x41
#define TEMP_OUT_L          0x42
#define GYRO_XOUT_H         0x43
#define GYRO_XOUT_L         0x44
#define GYRO_YOUT_H         0x45
#define GYRO_YOUT_L         0x46
#define GYRO_ZOUT_H         0x47
#define GYRO_ZOUT_L         0x48

#define SELF_TEST_X_GYRO    0x50
#define SELF_TEST_Y_GYRO    0x51
#define SELF_TEST_Z_GYRO    0x52
#define E_ID0               0x53
#define E_ID1               0x54
#define E_ID2               0x55
#define E_ID3               0x56
#define E_ID4               0x57
#define E_ID5               0x58
#define E_ID6               0x59
#define FIFO_WM_TH1         0x60
#define FIFO_WM_TH2         0x61

#define SIGNAL_PATH_RESET   0x68
#define ACCEL_INTEL_CTRL    0x69
#define USER_CTRL           0x6A
#define PWR_MGMT_1          0x6B
#define PWR_MGMT_2          0x6C

#define I2C_IF              0x70
#define FIFO_COUNTH         0x72
#define FIFO_COUNTL         0x73
#define FIFO_R_W            0x74
#define WHO_AM_I            0x75

#define XA_OFFSET_H         0x77
#define XA_OFFSET_L         0x78
#define YA_OFFSET_H         0x7A
#define YA_OFFSET_L         0x7B
#define ZA_OFFSET_H         0x7D
#define ZA_OFFSET_L         0x7E

#define GYRO_AXIS_DIR     {1, 2, 3}

#define MAX_I2C_RESTORE_SCL_CNT       (8 * 32)
#define PI 3.1415926535
#define ACC_FILTER                    (0.8)
#define TILT_ANGLE_FILTER             (0.4)
#define TILT_RANGE_GAIN               (0.2588) // cos(75)
#define ARROW_ORIENTATION_RANGE_GAIN  (0.2588) // cos(75)   
#define SCREEN_STATUS_RANGE_GAIN      (0.906) //  cos(25)   
#define SHAKE_CHECK_DATA_BUFFER_SIZE  (60)
#define GRAVITY_DEFAULT_VALUE         (17000 / 2)
#define I2C_NUM                       (I2C_NUM_1)
#define I2C_FREQUENCY                 (400000)

#define SHAKE_STRENGTH_FREQUENCY_GAIN (4)       // this is a empirical value (0 - 20)
#define SHAKE_STRENGTH_ACC_VALUE_GAIN (5.0 / GRAVITY_DEFAULT_VALUE)   // this is a empirical value
#define SHAKE_STRENGTH_START_CHECK_GAIN (0.05)

#define SHAKED_THRESHOLD_DEFAULT      (20)
#define SHAKED_THRESHOLD_MAX          (80)
#define SHAKED_THRESHOLD_MIN          (10)

#define BRANDISH_CHECK_START_THRESHOLD      (100)
#define BRANDISH_CHECK_THRESHOLD            (300)

#define GTRO_SENSITIVITY_DEFAULT      (65.5) // for 500 deg/s, check data sheet
#define GYRO_UPDATE_THRESHOLD_ACC     (100)  // acc
#define GYRO_UPDATE_THRESHOLD_GYRO    (10)   // gyro  
bool gyro_init();
bool gyro_read();
float get_acc_data(uint8_t axis);
float get_gyro_data(uint8_t axis);
float get_gyro_roll();
float get_gyro_pitch();
#endif