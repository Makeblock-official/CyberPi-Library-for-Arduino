#include "cyberpi.h"

CyberPi cyber;
void setup()
{
    Serial.begin(115200);
    cyber.begin();
}

void loop()
{    
    Serial.print("roll:");
    Serial.print(cyber.get_roll());
    Serial.print(" pitch:");
    Serial.println(cyber.get_pitch());
    delay(25);
}