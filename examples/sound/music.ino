
#include <Arduino.h>
#include "cyberpi.h"
CyberPi cyber;

int lo[7] = {48,50,52,53,55,57,59};
int mo[7] = {60,62,64,65,67,69,71};
int ho[7] = {72,74,76,77,79,81,83};

void setup() 
{
    Serial.begin(115200);
    for(int i=0;i<14;i++)
    {
        cyber.set_instrument(i);
        int idx = 0;
        while(idx<7)
        {
            cyber.set_pitch(0,lo[idx],100);
            delay(600);
            idx++;
        }
        idx = 0;
        while(idx<7)
        {
            cyber.set_pitch(0,mo[idx],100);
            delay(600);
            idx++;
        }
        idx = 0;
        while(idx<7)
        {
            cyber.set_pitch(0,ho[idx],100);
            delay(600);
            idx++;
        }
    }
}
void loop() 
{
}