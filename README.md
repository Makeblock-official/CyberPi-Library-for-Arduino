# cyberpi library for arduino

### using Arduino IDE
1. Search and install esp32 from Arduino IDE's Boards Manager

2. copy "lib/cyberpi/platform.local.txt" to Arduino IDE's resources folder "/packages/esp32/hardware/esp32/1.0.4/"

3. copy "lib/cyberpi" to "Documents/Arduino/libraries"

4. "/examples" for reference only.

### using VSCode+PlatformIO
1. the git repo is a full project with pio
 
2. copy "lib/cyberpi" to your pio project
 
3. modify "upload_port" which is the connected cyberpi board in file "platform.ini"

4. "/examples" for reference only.


```c++
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
```
