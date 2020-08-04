#include "cyberpi.h"

CyberPi cyber;
void setup()
{
    Serial.begin(115200);
    cyber.begin();
    for(int y=0;y<128;y++)
    {
        for(int x=0;x<128;x++)
        {
            int R = (128-x)*255/128;
            int G = x*255/128;
            int B = y*255/128;
            cyber.set_lcd_pixel(x,y,cyber.swap_color(cyber.color24_to_16((R<<16)+(G<<8)+B)));
        }
    }
    cyber.render_lcd();
}
void loop()
{ 
    cyber.set_lcd_light(false);
    delay(2000);
    cyber.set_lcd_light(true);
    delay(2000);
}
