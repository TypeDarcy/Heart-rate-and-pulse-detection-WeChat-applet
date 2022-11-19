连接方法

mqtt:tcp    (mqtt://)

host: 服务器地址
port: 1883

topic

/mqtt/testtopic

数据格式 :

{"params":{"HR":81,"SPO2":99.1,"Temp":26.1}}

串口baud 74880

## 工程配置 （如果您需要更改代码）

该项目基于 ESP8266 RTOS SDK v3.4 编写
你需要准备 
- 虚拟linux环境(我用的是msys32)
- ESP8266 RTOS SDK (安装部署教程https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/index.html)

而后将该项目复制到部署好的ESP8266_RTOS_SDK文件夹下 树结构如下
ESP8266_RTOS_SDK
│    ├─esp8266mqtt
│    	├─station

## 编译烧写

使用虚拟环境(msys32)进入到ESP8266_RTOS_SDK->esp8266mqtt->station文件夹内

命令行执行 

1.make clean 清理编译文件
2.make menuconfig 配置工程信息 (主要是用来下载的串口，默认是linux格式/tty/USBx， 使用windows需要更改为windows指定串口COMx
3.make 编译
4.make flash 烧录

## 配网过程

通过usb-ttl和usb口供电将 esp8266与电脑串口连接
nodemcu板子问题， usb口只能供电，ttl模块只用插三根线到板子上(TX->RX  , RX->TX, GND->GND)

连接串口后 将D1引脚接入高电平(3.3V)复位，串口提示输入输入#wifi,ssid# 和密码（此时串口软件有提示并且8266小灯常亮），第一次连接后自动存储到flash
后将D1引脚接入低电平(GND)，即可正常使用。 后续需要更改wifi，重复上述步骤。


## STM32F103 C8T6 部分

### 血氧模块接线

- VCC -> 3.3V
- GND -> GND
- SCL  -> PB6
- SDA -> PB7

### DS18B20 温度模块接线

VCC -> 3.3V
GND -> GND
DQ -> PA7

## ESP8266 部分

D8 接GND， 如果需要烧录程序，D3也要接GND。
D1接3.3V 可以从串口配网并存储， D1接 GND 复位后可以从flash区读取存储好的wifi信息自动连接。


## 最终连接 

STM32 部分连接完成后， 将VCC GND 与ESP8266连接起来， STM32 PA9 接 ESP8266 RX， PA10 接 ESP8266 TX。

## 最终注意

血氧模块是通过对模块读到的红外数据进行采样分析得出数据，它的误差受如下影响：

- 手指在模块上贴合程度、模块自身的效能、代码滤波算法
- 板子刚上电前几组可能误差较大
- 采样的时候手指一直要保持贴合在模块上，STM32上蓝色小灯闪烁就代表正在采样，如果蓝色小灯熄灭则代表采样数据错误重新开始采样（因此在接触不良好的时候会发送较慢）。采样成功会从串口发送数据，ESP8266接收到串口发送来的数据后解析并封装成JSON格式从MQTT上报。


   	
