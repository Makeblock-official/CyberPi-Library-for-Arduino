#include "cyberpi.h"

CyberPi cyber;
void setup()
{
    cyber.begin();
}
float j, f, k;

void loop()
{ 
    for(uint8_t t = 0; t < 5; t++)
    {
        uint8_t red	= 32 * (1 + sin(t / 2.0 + j / 4.0) );
        uint8_t green = 32 * (1 + sin(t / 1.0 + f / 9.0 + 2.1) );
        uint8_t blue = 32 * (1 + sin(t / 3.0 + k / 14.0 + 4.2) );
        cyber.set_rgb(t, red, green, blue);
    }
    j += random(1, 6) / 6.0;
    f += random(1, 6) / 6.0;
    k += random(1, 6) / 6.0;
    delay(10);
}