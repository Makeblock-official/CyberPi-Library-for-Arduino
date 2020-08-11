# cyberpi library for arduino

### 使用Arduino IDE
1. Arduino IDE的开发板管理器中安装ESP32环境

2. 复制"lib/cyberpi/platform.local.txt"文件至Arduino IDE的资源文件夹"/packages/esp32/hardware/esp32/1.0.4/"

3. 安装拷贝lib/cyberpi至"文档/Arduino/libraries"

4. examples示例供参考

### 使用VSCode+PlatformIO插件
1. 本git仓库为完整platformio工程
 
2. 拷贝lib/cyberpi至自己工程
 
3. 修改platform.ini中的upload_port串口号

4. examples示例供参考


```
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