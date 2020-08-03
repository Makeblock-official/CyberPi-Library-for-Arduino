#include <driver/i2s.h>
#include "cyberpi.h"
#include "arduinoFFT.h"
//
// Configuration
//

// I2S peripheral to use (0 or 1)
#define I2S_PORT          I2S_NUM_1

#define CONFIG_I2S_BACK_IO_NUM                    (13)
#define CONFIG_I2S_WS_IO_NUM                      (14)
#define CONFIG_I2S_DATA_OUT_NUM                   (-1)
#define CONFIG_I2S_DATA_IN_NUM                    (35)

#define CONFIG_I2S_BUFFER_COUNT                   (2)
#define CONFIG_I2S_BUFFER_SIZE                    (512)

#define CONFIG_I2S_NUM_FOR_MIC                    (1)
#define CONFIG_I2S_MIC_SAMPLE_RATE                (16000)
#define CONFIG_I2S_MIC_SAMPLE_BITS                (16)
#define CONFIG_I2S_MIC_READ_LEN                   (1 * 512)
#define CONFIG_I2S_MIC_FORMAT                     (I2S_CHANNEL_FMT_ONLY_RIGHT)
#define CONFIG_I2S_MIC_CHANNEL_NUM                ((CONFIG_I2S_MIC_FORMAT < I2S_CHANNEL_FMT_ONLY_RIGHT) ? (2) : (1))
#define CONFIG_I2S_MIC_CALCULATE_LOUDNESS_SAMPLE  (16)
#define AUDIO_RECORD_SIZE_PER_SECOND              (CONFIG_I2S_MIC_CHANNEL_NUM * CONFIG_I2S_MIC_SAMPLE_RATE * CONFIG_I2S_MIC_SAMPLE_BITS / 8)
#define I2S_MIC_NORMALIZATION_FOR_RETURN_VALUE    (327)    // 32768/327 = 100
//IO configure
#define GPIO_8218E_EN                             (GPIO_NUM_21)

#define CONFIG_I2S_MIC_TASK_STACK_SIZE            (512 * 5)
#define ESP_I2S_MIC_TASK_PRIO                     (7)

// Data we push to 'samples_queue'
struct sum_queue_t {
  // Sum of squares of mic samples, after Equalizer filter
  float sum_sqr_SPL;
  // Sum of squares of weighted mic samples
  float sum_sqr_weighted;
  // Debug only, FreeRTOS ticks we spent processing the I2S data
  uint32_t proc_ticks;
};
QueueHandle_t samples_queue;

// Static buffer for block of samples
uint8_t samples[CONFIG_I2S_MIC_READ_LEN] __attribute__((aligned(4)));

//
// I2S Microphone sampling setup 
//
void mic_i2s_init() {
  
  i2s_config_t i2s_config =
  {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
    .sample_rate =  CONFIG_I2S_MIC_SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t)CONFIG_I2S_MIC_SAMPLE_BITS,
    .channel_format = (i2s_channel_fmt_t)CONFIG_I2S_MIC_FORMAT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = CONFIG_I2S_BUFFER_COUNT,
    .dma_buf_len = CONFIG_I2S_BUFFER_SIZE,
    .use_apll = true
  };
  //install and start i2s driver
  i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);

  i2s_pin_config_t pin_config = 
  {
      .bck_io_num = CONFIG_I2S_BACK_IO_NUM,
      .ws_io_num = CONFIG_I2S_WS_IO_NUM,
      .data_out_num = CONFIG_I2S_DATA_OUT_NUM,
      .data_in_num = CONFIG_I2S_DATA_IN_NUM   //Not used
  };
  i2s_set_pin(I2S_NUM_1, &pin_config);
  
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);

}

#define I2S_TASK_PRI   4
#define I2S_TASK_STACK 2048
union
{
  uint8_t byteVal[2];
  int16_t shortVal;
}val2byte;

CyberPi cyber;
int calculate_the_loudness(uint8_t* buf, int16_t length)
{
  int32_t dac_value = 0;
  int32_t dac_value_addition = 0;
  int32_t average_value = 0;
  int16_t maximum_value = 0;
  for(int i = 0; i < length; i += CONFIG_I2S_MIC_CALCULATE_LOUDNESS_SAMPLE)
  {
    val2byte.byteVal[1] = buf[i + 1];
    val2byte.byteVal[0] = buf[i + 0];
    dac_value = val2byte.shortVal;
    dac_value_addition = dac_value_addition + abs(dac_value);
    if(abs(dac_value) > maximum_value)
    {
      maximum_value = abs(dac_value);
    }
  }
  average_value = CONFIG_I2S_MIC_CALCULATE_LOUDNESS_SAMPLE * dac_value_addition / length;
  return average_value;
}
#define ROUND_X(x) ((int)(x + 0.5f))
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

int __cross_points(int x1, int y1, int x2, int y2, int x3, int y3)
{
    return x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
}

void __draw_vline(int x, int y1, int y2)
{
  int start = min(y1,y2);
  int end = max(y1,y2);
  for(int i=start;i<=end;i++)
  {
    cyber.set_lcd_pixel(x,i,0xffff);
  }
}
static void __draw_line(float xx1, float yy1, float xx2, float yy2)
{
    int x1 = ROUND_X(xx1);
    int x2 = ROUND_X(xx2);
    int y1 = ROUND_X(yy1);
    int y2 = ROUND_X(yy2);
    int x, y, xx=0, yy=0;
    int dxmin, dymin;

    dxmin = (x1 < x2) ? 0 : -1;
    dymin = (y1 < y2) ? 0 : -1;
    x = x1;
    y = y1;
    
    int crossp, min_crossp = INT_MAX;
    float ddx = x1-x2;
    float ddy = y1-y2;
    while (true) 
    {
        int dx, dy, x3, y3;
        cyber.set_lcd_pixel(x,y,0xffff);
        if (x == x2 && y == y2)
            break;
        min_crossp = INT_MAX;
        for (dx = dxmin; dx <= dxmin+1; dx++) 
        {
            for (dy = dymin; dy <= dymin+1; dy++) 
            {
                if (!dx && !dy)
                    continue;
                x3 = x+dx;
                y3 = y+dy;
                crossp = abs(__cross_points(x1, y1, x2, y2, x3, y3));
                if (crossp < min_crossp) 
                {
                    min_crossp = crossp;
                    xx = x3;
                    yy = y3;
                }
            }
        }
        x = xx;
        y = yy;
    }
}
int bands[8] = {0, 0, 0, 0, 0, 0, 0, 0};
const int BLOCK_SIZE = 512;
double vReal[512];
double vImag[512];
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

const double signalFrequency = 1000;
const double samplingFrequency = 10000;
const uint8_t amplitude = 150;
void mic_i2s_reader_task(void* parameter) {
  mic_i2s_init();
  size_t bytes_read = 0;
  int t = 0;
  // vReal=(double*)cyber.malloc(BLOCK_SIZE*4);
  // vImag=(double*)cyber.malloc(BLOCK_SIZE*4);
  while (true) {
    i2s_read(I2S_PORT, &samples, CONFIG_I2S_MIC_READ_LEN, &bytes_read, portMAX_DELAY);
    for (uint16_t i = 0; i < BLOCK_SIZE; i++) {
      vReal[i] = samples[i] << 8;
      vImag[i] = 0.0; 
    }

    FFT.Compute(vReal, vImag, BLOCK_SIZE, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, BLOCK_SIZE);
    for (int i = 0; i < 8; i++) 
    {
      bands[i] = 0;
    }
    
    for (int i = 2; i < (BLOCK_SIZE/2); i++)
    { 
      if (vReal[i] > 2000) 
      { // Add a crude noise filter, 10 x amplitude or more
        if (i<=2 )             bands[0] = max(bands[0], (int)(vReal[i]/amplitude)); // 125Hz
        if (i >3   && i<=5 )   bands[1] = max(bands[1], (int)(vReal[i]/amplitude)); // 250Hz
        if (i >5   && i<=7 )   bands[2] = max(bands[2], (int)(vReal[i]/amplitude)); // 500Hz
        if (i >7   && i<=15 )  bands[3] = max(bands[3], (int)(vReal[i]/amplitude)); // 1000Hz
        if (i >15  && i<=30 )  bands[4] = max(bands[4], (int)(vReal[i]/amplitude)); // 2000Hz
        if (i >30  && i<=53 )  bands[5] = max(bands[5], (int)(vReal[i]/amplitude)); // 4000Hz
        if (i >53  && i<=200 ) bands[6] = max(bands[6], (int)(vReal[i]/amplitude)); // 8000Hz
        if (i >200           ) bands[7] = max(bands[7], (int)(vReal[i]/amplitude)); // 16000Hz
      }
    }
    int loudness = calculate_the_loudness(samples, bytes_read);
    cyber.clean_lcd();
    for(int i=0;i<8;i++)
    {
        int v = (bands[i]>>8)+20;
        __draw_vline(i*14+16,127,MIN(127,MAX(0,v)));
        __draw_vline(i*14+15,127,MIN(127,MAX(0,v)));
        __draw_vline(i*14+14,127,MIN(127,MAX(0,v)));
    }
    cyber.render_lcd();
    // 
    // t++;
    // if(t%1==0&&loudness>100)
    // {
    //   cyber.clean_lcd();
    //   val2byte.byteVal[0] = samples[1];
    //   val2byte.byteVal[1] = samples[0];
    //   int prev = val2byte.shortVal;
    //   for(int i=0;i<1024;i+=8)
    //   {
    //     val2byte.byteVal[0] = samples[i+1];
    //     val2byte.byteVal[1] = samples[i];
    //     int current = val2byte.shortVal;
    //     // Serial.printf("current:%d\n",current);
    //     if(prev<64&&prev>-64&&current>-64&&current<64)
    //     {
    //       __draw_line(i/8-1,prev+64,i/8,current+64);
    //     }
    //     prev = current;
    //   }
    //   cyber.render_lcd();
    // }
  }
}

void setup() {
  Serial.begin(112500);
  delay(1000);
  cyber.begin();

  samples_queue = xQueueCreate(8, sizeof(sum_queue_t));
  xTaskCreate(mic_i2s_reader_task, "Mic I2S Reader", I2S_TASK_STACK, NULL, I2S_TASK_PRI, NULL);

}

void loop() 
{
}