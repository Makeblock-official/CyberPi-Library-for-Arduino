#include "cyberpi.h"
#include "driver/i2s.h"
#include "driver/dac.h"
extern "C" { 
#include "io/aw9523b.h"
#include "lcd/lcd.h"
#include "i2c/i2c.h"
#include "gyro/gyro.h"
#include "sound/synth.h"
#include "microphone/es8218e.h"
}
static MSynth _audio;
static uint16_t *_framebuffer;
static uint8_t*_led_data;
static uint8_t*_gyro_data;
static SemaphoreHandle_t _render_ready;
static bool sound_enabled = false;
static bool microphone_enabled = false;
data_callback _mic_callback;
data_callback _sound_callback;
static long prev_time = 0;
static int _loudness;
CyberPi::CyberPi()
{ 
    _render_ready = xSemaphoreCreateBinary();
    _led_data = this->malloc(15);
    _gyro_data = this->malloc(14);
    
    memset(_led_data,0,15);
}
void CyberPi::begin()
{
    i2c_init();
    aw_init();
    begin_gpio();
    begin_lcd();
    begin_gyro();
    begin_sound();
    begin_microphone();

    TaskHandle_t threadTask_lcd;
    xTaskCreatePinnedToCore(this->_on_lcd_thread,"_on_lcd_thread",4096,NULL,10,&threadTask_lcd,1);
    TaskHandle_t threadTask_sensor;
    xTaskCreatePinnedToCore(this->_on_sensor_thread, "on_sensor_read", 10000, this, 8, &threadTask_sensor, 1);
    TaskHandle_t threadTask_sound;
    xTaskCreatePinnedToCore(this->_on_sound_thread, "on_sound_read", 10000, this, 9, &threadTask_sound, 1);
}
void CyberPi::_on_lcd_thread(void *p)
{
    while(true)
    {
        if(xSemaphoreTake(_render_ready, 25)==pdTRUE)
        {
            lcd_draw(_framebuffer,128,128);
        }
    }
}


void CyberPi::begin_gpio()
{
    pinMode(33,INPUT);
    aw_pinMode(AW_P1_3,AW_GPIO_MODE_OUTPUT);
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
    _framebuffer = (uint16_t*)this->malloc(128*128*2);
    memset(_framebuffer,0,128*128*2);
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
Bitmap* CyberPi::create_text(wchar_t*chars,uint16_t color,uint8_t font_size)
{
    Bitmap *bitmap = new Bitmap();
    int cx=0,cy=0,x=0,y=0;
    int i=0;
    uint32_t c;
    uint8_t font_width = font_size;
    uint8_t font_height = font_size;
    uint8_t font_max_height = font_size;
    uint8_t *buf = (uint8_t*)MALLOC_SPI(400);
    bool elongate = false;
    uint8_t font = 10;
    while((c = *(chars + i)) != 0) 
    {
        if(c == 0x08) 
        { // Backspace
            cx -= font_width;
        } 
        else if(c == 0x0A) 
        {// New line
            cx = 0;
            cy += font_height;
        }
        else
        {
            get_utf8_data(c, c<256?6:font,buf,&elongate,&font_width,&font_height);
            cx += font_width*font_max_height/font_height+0.5f;
        }
        i++;
    }
    bitmap->width = cx;
    bitmap->height = cy+font_max_height+1;
    bitmap->buffer = (uint16_t*)this->malloc(bitmap->width*bitmap->height*2);
    memset(bitmap->buffer,0,bitmap->width*bitmap->height*2);
    cx = 0;
    cy = 0;
    i = 0;
    while((c = *(chars + i)) != 0) 
    {
        if(c == 0x08) 
        {
            cx -= font_width;
        } 
        else if(c == 0x0A) 
        {
            cx = 0;
            cy += font_height;
        } 
        else 
        {
            get_utf8_data(c, c<256?6:font,buf,&elongate,&font_width,&font_height);
            read_char(bitmap,cx, cy,font_size,font_max_height, buf+(elongate?2:0), font_width,font_height,elongate,color);
            cx += font_width*font_max_height/font_height+0.5f;
        }
        i++;
    }
    free(buf);
    return bitmap;
}
void CyberPi::read_char(Bitmap*bitmap,int x,int y,float w,float h,uint8_t* buffer, float font_width,float font_height,bool elongate,uint16_t color)
{
    if(buffer)
    {
        float width = font_width,height = font_height;
        int widthBytes = (int)ceil((double)width / 8);
        float scale = h/height;
        if(scale==1)
        {
            for(int v=0; v<height; v++) 
            {
                for(int u=0; u<width; u++) 
                {
                    int b = u >> 3; 
                    int bit = ~u & 7;
                    if((buffer[widthBytes * v + b]>>bit)&1)
                    {
                        bitmap->buffer[x+u+(y+v)*bitmap->width] = color;
                    }
                }
            }
        }
        else
        {
            uint8_t *buf = (uint8_t*)this->malloc(width*height);
            memset(buf,0,width*height);
            for(int v=0,hh=height; v<hh; v++) 
            {
                for(int u=0,ww=width; u<ww; u++) 
                {
                    int b = u >> 3; 
                    int bit = ~u & 7;
                    if((buffer[widthBytes * v + b]>>bit)&1)
                    {
                        buf[u+(v*ww)] = 1;
                        bitmap->buffer[x+(int)(u*scale)+(y+(int)(v*scale))*bitmap->width] = color;
                    }
                }
            }
            for(int i=0,hh=height*scale+0.5f;i<hh;i++)
            {
                for(int j=0,ww=width*scale+0.5f,www=width;j<ww;j++)
                {
                    if(buf[(int)(i/scale)*www+(int)(j/scale)])
                    {
                        bitmap->buffer[x+j+(y+i)*bitmap->width] = color;
                    }
                }
            }
            free(buf);
        }
    }
}
void CyberPi::clean_lcd()
{
    memset(_framebuffer,0x0,128*128*2);
}

void CyberPi::set_lcd_pixel(uint8_t x,uint8_t y,uint16_t color)
{
    if(y>=0&&y<128&&x>=0&&x<128)
    {
        _framebuffer[y*128+x] = color;
    }
}

void CyberPi::set_bitmap(uint8_t x,uint8_t y, Bitmap* bitmap)
{
    Serial.printf("%d,%d\n",bitmap->width,bitmap->height);
    for(int i=0;i<bitmap->height;i++)
    {
        for(int j=0;j<bitmap->width;j++)
        {
            set_lcd_pixel(x+j,y+i,bitmap->buffer[i*bitmap->width+j]);
        }
    }
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
    if(millis()-lastTime>20)
    {
        lastTime = millis();
        xSemaphoreGive(_render_ready);
    }
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
    sound_enabled = true;
    aw_digitalWrite(AW_P1_3,1);
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN ),
        .sample_rate =  20000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0,
        .dma_buf_count = 16,
        .dma_buf_len = 256,
        }; 
        i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
        i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN); 
        i2s_set_clk(I2S_NUM_0, 20000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

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
    if(sound_enabled)
    {
        size_t bytes_written;
        i2s_write(I2S_NUM_0, audio_buf, audio_buf_len, &bytes_written, portMAX_DELAY);
        if(_sound_callback)
        {
            _sound_callback(audio_buf,audio_buf_len);
        }
    }
}
void CyberPi::_on_sound_thread(void*p)
{
    while(1)
    {
        if(sound_enabled)
        {
            _audio.render();
        }
    }
}
void CyberPi::_on_sensor_thread(void* p) {
    size_t bytes_read = 0;
    int mic_length = 1024;
    uint8_t *samples = (uint8_t*)((CyberPi*)p)->malloc(1024);
    while (true) 
    {
        long current_time = millis();
        if(current_time-prev_time>25)
        {
            prev_time = current_time;
            gyro_read();
        }
        if(microphone_enabled)
        {
            i2s_read(I2S_NUM_1, samples, mic_length, &bytes_read, portMAX_DELAY);
            int32_t dac_value = 0;
            int32_t dac_value_addition = 0;
            int16_t maximum_value = 0;
            for(int i = 0; i < mic_length; i += 16)
            {
                val2byte.byteVal[1] = samples[i + 1];
                val2byte.byteVal[0] = samples[i + 0];
                dac_value = val2byte.shortVal;
                dac_value_addition = dac_value_addition + abs(dac_value);
                if(abs(dac_value) > maximum_value)
                {
                    maximum_value = abs(dac_value);
                }
            }
            _loudness = 16 * dac_value_addition / mic_length;
            if(_mic_callback)
            {
                _mic_callback(samples,mic_length);
            }
        }
    }
}
int CyberPi::get_loudness()
{
    return _loudness;
}
void CyberPi::begin_microphone ()
{
    microphone_enabled = true;
    _mic_callback = NULL;
    i2s_config_t i2s_config =
    {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate =  160000,
        .bits_per_sample = (i2s_bits_per_sample_t)16,
        .channel_format = (i2s_channel_fmt_t)I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = true
    };
    i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
    i2s_pin_config_t pin_config = 
    {
        .bck_io_num = 13,
        .ws_io_num = 14,
        .data_out_num = -1,
        .data_in_num = 35 
    };
    i2s_set_pin(I2S_NUM_1, &pin_config);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    es8218e_start();
}

void CyberPi::on_microphone_data(data_callback func)
{
    _mic_callback = func;
}

void CyberPi::on_sound_data(data_callback func)
{
    _sound_callback = func;
}
uint8_t* CyberPi::malloc(uint32_t len)
{
    if (heap_caps_get_free_size( MALLOC_CAP_SPIRAM )==0)
    {
        return (uint8_t*)MALLOC_INTERNAL(len);
    }
    return (uint8_t*)MALLOC_SPI(len);
}
