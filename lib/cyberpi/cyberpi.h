#ifndef __CYBER_PI__
#define __CYBER_PI__
#include <stdint.h>
#include <Arduino.h>
extern "C"
{
#include "esp_heap_caps.h"
}
#define MALLOC_SPI(a) (heap_caps_malloc((a),MALLOC_CAP_SPIRAM))
#define MALLOC_INTERNAL(a) (heap_caps_malloc((a),MALLOC_CAP_DMA))

#define JOYSTICK_UP_IO                      AW_P0_1                         
#define JOYSTICK_DOWN_IO                    AW_P0_4
#define JOYSTICK_RIGHT_IO                   AW_P0_2
#define JOYSTICK_LEFT_IO                    AW_P0_0
#define JOYSTICK_CENTER_IO                  AW_P0_3
#define BUTTON_A_IO                         AW_P0_6
#define BUTTON_B_IO                         AW_P0_5
#define BUTTON_MENU_IO                      AW_P1_0

union
{
  uint8_t byteVal[2];
  int16_t shortVal;
}val2byte;

#define ROUND_X(x) ((int)(x + 0.5f))
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
typedef void (*data_callback)(uint8_t*,int);

struct Bitmap
{
    float x;
    float y;
    uint8_t width;
    uint8_t height;
    uint16_t *buffer;
};

class CyberPi
{
    public:
        CyberPi();
        void begin();
        
        void set_lcd_light(bool on);
        void clean_lcd();
        void set_lcd_pixel(uint8_t x,uint8_t y,uint16_t color);
        Bitmap* create_text(wchar_t*chars,uint16_t color,uint8_t font_size);
        void set_bitmap(uint8_t x,uint8_t y, Bitmap* buffer);
        uint16_t color24_to_16(uint32_t rgb);
        uint16_t swap_color(uint16_t color);
        void render_lcd();

        void set_rgb(int idx,uint8_t red,uint8_t greeen,uint8_t blue);

        uint16_t get_gpio();

        int get_joystick_x();
        int get_joystick_y();
        bool get_joystick_pressed();

        bool get_button_a();
        bool get_button_b();
        bool get_button_menu();
        
        uint16_t get_light();

        float get_gyro_x();
        float get_gyro_y();
        float get_gyro_z();
        float get_acc_x();
        float get_acc_y();
        float get_acc_z();
        float get_roll();
        float get_pitch();

        void set_pitch(uint8_t channel, uint8_t pitch,uint8_t time);
        void set_instrument(uint8_t instrument);
        int get_loudness();
        void on_microphone_data(data_callback func);
        void on_sound_data(data_callback func);
        uint8_t* malloc(uint32_t len);
    private:
        long lastTime;
        void begin_gyro();
        void begin_gpio();
        void begin_lcd();
        void begin_sound();
        void begin_microphone();
        void read_char(Bitmap*bitmap,int x,int y,float w,float h,uint8_t* buffer, float font_width,float font_height,bool elongate,uint16_t color);
        static void _on_sound_thread(void*p);
        static void _on_sensor_thread(void* parameter);
        static void _on_lcd_thread(void *p);
        static void _render_audio(uint8_t *audio_buf,uint16_t audio_buf_len);
};
#endif