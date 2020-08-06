#include "cyberpi.h"

CyberPi cyber;
uint8_t samples[128];
int idx = 0;

void setup() {
    Serial.begin(112500);
    delay(1000);
    cyber.begin();
}

void loop() 
{
    if(idx<128)
    {
        samples[idx] = cyber.get_loudness()>>5;
        idx++;
    }
    else
    {
        for(int i=0;i<128;i++)
        {
            samples[i] = samples[i+1];
        }
        samples[idx-1] = cyber.get_loudness()>>5;
    }
    cyber.clean_lcd();
    for(int i=0;i<128;i++)
    {
      if(i==0)
        cyber.set_lcd_pixel(i,127-samples[i],0xffff);
      else
      {
          int min_level = MIN(samples[i-1],samples[i]);
          int max_level = MAX(samples[i-1],samples[i]);
          for(int j=min_level;j<=max_level;j++)
          {
            cyber.set_lcd_pixel(i,127-j,0xffff);
          }
      }
      
    }
    cyber.render_lcd();
    delay(25);
}