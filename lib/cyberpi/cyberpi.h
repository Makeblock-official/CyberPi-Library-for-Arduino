#ifndef __CYBER_PI__
#define __CYBER_PI__
#include <stdint.h>
#include <Arduino.h>
extern "C"
{
#include <esp32/himem.h>
#include <esp32/spiram.h>
#include "esp_heap_caps.h"
}
#define MALLOC_SPI(a) (ps_malloc(a))
#define MALLOC_INTERNAL(a) (heap_caps_malloc((a),MALLOC_CAP_DMA))
class CyberPi
{
    public:
        CyberPi();

        void begin_lcd();
        void set_lcd_light(bool on);
        void set_lcd_pixel(uint8_t x,uint8_t y,uint16_t color);
        uint16_t color24_to_16(uint32_t rgb);
        uint16_t swap_color(uint16_t color);
        void render_lcd();

        void set_rgb(int idx,uint8_t red,uint8_t greeen,uint8_t blue);

        int get_joystick_x();
        int get_joystick_y();
        bool get_joystick_pressed();

        bool get_button_a();
        bool get_button_b();
        bool get_button_c();

        void begin_gyro();
        float get_gyro_x();
        float get_gyro_y();
        float get_gyro_z();

        void begin_sound();
        void set_pitch(uint8_t channel, uint8_t pitch,uint8_t time);
        void set_instrument(uint8_t instrument);

        uint8_t* malloc(uint32_t len);

    private:
        static void _on_thread(void *p);
        static void _render_audio(uint8_t *audio_buf,uint16_t audio_buf_len);
};
#endif