#include "cyberpi.h"

CyberPi cyber;

void mic_recv(int8_t* samples,int len) {
    cyber.clean_lcd();
    for(int i=0;i<len;i+=8)
    {
      int current = samples[i+1];
      if(current>-64&&current<64)
      {
        cyber.set_lcd_pixel(i/8,current+64,0xffff);
      }
    }
    cyber.render_lcd();
}

void setup() {
  Serial.begin(112500);
  delay(1000);
  cyber.begin();
  cyber.on_microphone_data(mic_recv);
}

void loop() 
{
}