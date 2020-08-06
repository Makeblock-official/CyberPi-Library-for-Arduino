#include "cyberpi.h"

CyberPi cyber;
uint8_t samples[128];
int idx = 0;

void setup() 
{
    Serial.begin(112500);
    delay(1000);
    cyber.begin();
    int font_size = 16;
    Bitmap *bitmap1 = cyber.create_text(L"你好",0xffff,font_size);
    cyber.set_bitmap(4,4,bitmap1);
    Bitmap *bitmap2 = cyber.create_text(L"簡體",0xff00,font_size);
    cyber.set_bitmap(4,24,bitmap2);
    Bitmap *bitmap3 = cyber.create_text(L"hello",0x00ff,font_size);
    cyber.set_bitmap(4,44,bitmap3);
    Bitmap *bitmap4 = cyber.create_text(L"こんにちは",0x0ff0,font_size);
    cyber.set_bitmap(4,64,bitmap4);
    Bitmap *bitmap5 = cyber.create_text(L"여보세요",0x0f0f,font_size);
    cyber.set_bitmap(4,84,bitmap5);
    Bitmap *bitmap6 = cyber.create_text(L"Привет",0xf0f0,font_size);
    cyber.set_bitmap(4,104,bitmap6);
    cyber.render_lcd();
}

void loop() 
{
    
}