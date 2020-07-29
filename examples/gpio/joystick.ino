#include "cyberpi.h"

CyberPi cyber;
void setup()
{
    Serial.begin(115200);
    cyber.begin();
}

void loop()
{ 
    Serial.print("x:");
    Serial.print(cyber.get_joystick_x());
    Serial.print(" y:");
    Serial.print(cyber.get_joystick_y());
    Serial.print(" pressed:");
    Serial.println(cyber.get_joystick_pressed());
    delay(500);
}