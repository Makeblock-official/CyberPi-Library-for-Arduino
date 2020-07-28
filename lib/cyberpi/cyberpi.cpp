#include "cyberpi.h"
#include "FreeRTOS.h"
#include "driver/i2s.h"
#include "driver/dac.h"
extern "C" { 
#include "io/aw9523b.h"
#include "lcd/lcd.h"
#include "sound/synth.h"
}
static MSynth _audio;
static uint16_t *_framebuffer;
static SemaphoreHandle_t _render_ready ;
CyberPi::CyberPi()
{ 
    _render_ready = xSemaphoreCreateBinary();
    _framebuffer = (uint16_t*)this->malloc(128*128*2);
    memset(_framebuffer,0,128*128*2);
    aw_init(0);
    begin_sound();
    TaskHandle_t threadTask;
    xTaskCreatePinnedToCore(CyberPi::_on_thread,"_on_thread",4096,NULL,10,&threadTask,1);
}

void CyberPi::_on_thread(void *p){
    while(true)
    {
        _audio.render();
        if(xSemaphoreTake(_render_ready, portMAX_DELAY)==pdTRUE)
        {
            lcd_draw(_framebuffer,128,128);
        }
    }
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
    
}

int CyberPi::get_joystick_x()
{
    
}
int CyberPi::get_joystick_y()
{
    
}
bool CyberPi::get_joystick_pressed()
{
    
}

bool CyberPi::get_button_a()
{
    
}
bool CyberPi::get_button_b()
{
    
}
bool CyberPi::get_button_c()
{
    
}

void CyberPi::begin_gyro()
{
    
}
float CyberPi::get_gyro_x()
{
    
}
float CyberPi::get_gyro_y()
{
    
}
float CyberPi::get_gyro_z()
{
    
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