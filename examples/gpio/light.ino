#include "cyberpi.h"

CyberPi cyber;
void setup()
{
    Serial.begin(115200);
    cyber.begin();
}

void loop()
{ 
    Serial.print("light:");
    Serial.println(cyber.get_light());
    delay(500);
}