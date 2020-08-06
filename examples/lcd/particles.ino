#include "cyberpi.h"

CyberPi cyber;
#define POINTS_COUNT 200
Bitmap points[POINTS_COUNT];
float speed_x[POINTS_COUNT];
float speed_y[POINTS_COUNT];
void setup() {
    Serial.begin(112500);
    delay(1000);
    cyber.begin();
    for(int i=0;i<POINTS_COUNT;i++)
    {
        points[i].x = 64;
        points[i].y = 64;
        speed_x[i] = random(100)/20.0f-2.5f;
        speed_y[i] = random(100)/20.0f-2.5f;
        points[i].width = 1;
        points[i].height = 1;
        points[i].buffer = (uint16_t*)cyber.malloc(2);
        points[i].buffer[0] = 0xffff;
    }
}

void loop() 
{
    cyber.clean_lcd();
    for(int i=0;i<POINTS_COUNT;i++)
    {
        cyber.set_bitmap(points[i].x,points[i].y,&points[i]);
        points[i].x+=speed_x[i];
        points[i].y+=speed_y[i];
        if(points[i].x<0||points[i].y<0||points[i].x>127||points[i].y>127)
        {
            points[i].x = 64;
            points[i].y = 64;
        }
    }
    cyber.render_lcd();
}