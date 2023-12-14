#include"lr_code.h"
#include<Arduino.h>
#include<WiFi.h>


const char* ssid = "your_ssid";
const char* passwd = "your_passwd";
//IPAddress staticIP(192,168,0,10);
IPAddress staticIP(your_ip);
IPAddress gateway(your_gateway);
IPAddress subnet(your_subnet);
IPAddress dns(your_dns);
using namespace websockets;
WebsocketsServer server;

void setup()
{
    Serial.begin(115200);
    Serial.println("串口打开成功");
    while(!Serial)delay(1000);
    Serial.println("开始连接wifi");
    if(WiFi.config(staticIP, gateway, subnet, dns, dns) == false)Serial.println("Wifi配置失败，我也没啥办法");
    WiFi.begin(ssid, passwd);
    while (WiFi.status() != WL_CONNECTED)delay(500);
    Serial.println("WiFi连接成功！");
    server.listen(12345);
    Serial.println("正在监听12345端口");
    if(SPIFFS.begin())Serial.println("文件系统启动成功");
    xTaskCreatePinnedToCore(loop2, "Loop2", 4096, NULL, 1, NULL, 0); //在0核心上创建任务
}

void loop()
{
    auto client = server.accept();
    while(client.available()) 
    {
      auto msg = client.readBlocking();
      aircon(msg.data());
      client.send(json_send());
      delay(100);
      continue;
    }
    client.close();
    delay(100);
}