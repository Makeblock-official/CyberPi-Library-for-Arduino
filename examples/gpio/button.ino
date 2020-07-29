#include "cyberpi.h"

CyberPi cyber;
void setup()
{
    Serial.begin(115200);
    cyber.begin();
}

void loop()
{ 
    Serial.print("a:");
    Serial.print(cyber.get_button_a());
    Serial.print(" b:");
    Serial.print(cyber.get_button_b());
    Serial.print(" menu:");
    Serial.println(cyber.get_button_menu());
    delay(500);
}