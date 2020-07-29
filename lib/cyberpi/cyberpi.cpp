#include "cyberpi.h"
#include "FreeRTOS.h"
#include "driver/i2s.h"
#include "driver/dac.h"
extern "C" { 
#include "io/aw9523b.h"
#include "lcd/lcd.h"
#include "i2c/i2c.h"
#include "gyro/gyro.h"
#include "sound/synth.h"
}
static MSynth _audio;
static uint16_t *_framebuffer;
static uint8_t*_led_data;
static uint8_t*_gyro_data;
static SemaphoreHandle_t _render_ready ;

CyberPi::CyberPi()
{ 
    _render_ready = xSemaphoreCreateBinary();
    _framebuffer = (uint16_t*)this->malloc(128*128*2);
    _led_data = this->malloc(15);
    _gyro_data = this->malloc(14);
    memset(_led_data,0,15);
    memset(_framebuffer,0,128*128*2);
}
void CyberPi::begin()
{
    i2c_init();
    begin_gyro();
    begin_gpio();
    begin_sound();
    begin_gyro();
    begin_lcd();
    TaskHandle_t threadTask;
    xTaskCreatePinnedToCore(CyberPi::_on_thread,"_on_thread",4096,NULL,10,&threadTask,1);
}
static long prev_time = 0;
void CyberPi::_on_thread(void *p)
{
    while(true)
    {
        long current_time = millis();
        _audio.render();
        if(current_time-prev_time>25)
        {
            prev_time = current_time;
            gyro_read();
        }
        if(xSemaphoreTake(_render_ready, 25)==pdTRUE)
        {
            lcd_draw(_framebuffer,128,128);
        }
    }
}

#define JOYSTICK_UP_IO                      AW_P0_1                         
#define JOYSTICK_DOWN_IO                    AW_P0_4
#define JOYSTICK_RIGHT_IO                   AW_P0_2
#define JOYSTICK_LEFT_IO                    AW_P0_0
#define JOYSTICK_CENTER_IO                  AW_P0_3
#define BUTTON_A_IO                         AW_P0_6
#define BUTTON_B_IO                         AW_P0_5
#define BUTTON_MENU_IO                      AW_P1_0

void CyberPi::begin_gpio()
{
    aw_init();
    pinMode(33,INPUT);
    aw_pinMode(JOYSTICK_UP_IO,AW_GPIO_MODE_INPUT);
    aw_pinMode(JOYSTICK_DOWN_IO,AW_GPIO_MODE_INPUT);
    aw_pinMode(JOYSTICK_RIGHT_IO,AW_GPIO_MODE_INPUT);
    aw_pinMode(JOYSTICK_LEFT_IO,AW_GPIO_MODE_INPUT);
    aw_pinMode(JOYSTICK_CENTER_IO,AW_GPIO_MODE_INPUT);
    aw_pinMode(BUTTON_A_IO,AW_GPIO_MODE_INPUT);
    aw_pinMode(BUTTON_B_IO,AW_GPIO_MODE_INPUT);
    aw_pinMode(BUTTON_MENU_IO,AW_GPIO_MODE_INPUT);
}
void CyberPi::begin_lcd()
{
    lcd_init();
}
void CyberPi::set_lcd_light(bool on)
{
    if(on)
    {
        lcd_on();
    }
    else 
    {
        lcd_off();
    }
}

void CyberPi::set_lcd_pixel(uint8_t x,uint8_t y,uint16_t color)//uint16_t*buffer,uint16_t width,uint16_t height)
{
    _framebuffer[y*128+x] = color;
}
uint16_t CyberPi::color24_to_16(uint32_t rgb)
{
    return color24to16(rgb);
}

uint16_t CyberPi::swap_color(uint16_t color)
{
    return ((color&0xff)<<8)|(color>>8);
}
void CyberPi::render_lcd()
{
    xSemaphoreGive(_render_ready);
}

void CyberPi::set_rgb(int idx,uint8_t red,uint8_t greeen,uint8_t blue)
{
    _led_data[idx*3] = red;
    _led_data[idx*3+1] = greeen;
    _led_data[idx*3+2] = blue;
    i2c_write_data(0x5B, REG_DIM00, _led_data, 15);
}
uint16_t CyberPi::get_gpio()
{
    return aw_get_gpio();
}
uint16_t CyberPi::get_light()
{
    return analogRead(33);
}
int CyberPi::get_joystick_x()
{
    if(aw_digitalRead(JOYSTICK_RIGHT_IO))
    {
        return 1;
    }
    if(aw_digitalRead(JOYSTICK_LEFT_IO))
    {
        return -1;
    }
    return 0;
}
int CyberPi::get_joystick_y()
{
    if(aw_digitalRead(JOYSTICK_DOWN_IO))
    {
        return 1;
    }
    if(aw_digitalRead(JOYSTICK_UP_IO))
    {
        return -1;
    }
    return 0;
}
bool CyberPi::get_joystick_pressed()
{
    return aw_digitalRead(JOYSTICK_CENTER_IO);
}

bool CyberPi::get_button_a()
{
    return aw_digitalRead(BUTTON_A_IO);
}
bool CyberPi::get_button_b()
{
    return aw_digitalRead(BUTTON_B_IO);
}
bool CyberPi::get_button_menu()
{
    return aw_digitalRead(BUTTON_MENU_IO);
}

void CyberPi::begin_gyro()
{
    gyro_init();
}
float CyberPi::get_gyro_x()
{
    return get_gyro_data(0);
}
float CyberPi::get_gyro_y()
{
    return get_gyro_data(1);
}
float CyberPi::get_gyro_z()
{
    return get_gyro_data(2);
}
float CyberPi::get_acc_x()
{
    return get_acc_data(0);
}
float CyberPi::get_acc_y()
{
    return get_acc_data(1);
}
float CyberPi::get_acc_z()
{
    return get_acc_data(2);
}
float CyberPi::get_roll()
{
    return get_gyro_roll();
}
float CyberPi::get_pitch()
{
    return get_gyro_pitch();
}
void CyberPi::begin_sound()
{
    i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN ),
    .sample_rate =  44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 256,
    }; 
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);
    i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
    _audio.begin((AudioBack)&CyberPi::_render_audio); 
}
void CyberPi::set_pitch(uint8_t channel, uint8_t pitch,uint8_t time)
{
    _audio.addNote(channel,pitch,time);
}
void CyberPi::set_instrument(uint8_t instrument)
{
    _audio.setInstrument(instrument);
}
void CyberPi::_render_audio(uint8_t *audio_buf,uint16_t audio_buf_len)
{
  size_t bytes_written;
  i2s_write(I2S_NUM_0, audio_buf, audio_buf_len, &bytes_written, portMAX_DELAY);
}

uint8_t* CyberPi::malloc(uint32_t len)
{
    if (heap_caps_get_free_size( MALLOC_CAP_SPIRAM )==0)
    {
        return (uint8_t*)MALLOC_INTERNAL(len);
    }
    return (uint8_t*)MALLOC_SPI(len);
}