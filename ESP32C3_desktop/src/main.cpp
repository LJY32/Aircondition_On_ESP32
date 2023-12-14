#include <Arduino.h>
#include <sstream>
#include<string>
#include<cstring>
#include <U8g2lib.h>
#include <Wire.h>
#include<WiFi.h>
#include <ArduinoWebsockets.h>
#include <DHT.h>
#define DHTPIN 3
#define DHTTYPE DHT11

const char* ssid = "your_ssid";
const char* passwd = "your_passwd";
//IPAddress staticIP(192,168,0,10);
IPAddress staticIP(your_ip);
IPAddress gateway(your_gateway);
IPAddress subnet(your_subnet);
IPAddress dns(your_dns);
using namespace websockets;
WebsocketsServer server;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE,SCL,SDA);
DHT dht(DHTPIN, DHTTYPE);
String acof = "开关未定义",acmode = "模式未定义",actemp = "温度未定义";
float real_temp = -0.5,real_humidity = -0.6;

std::string get_data() {  
    float h = dht.readHumidity();  
    float t = dht.readTemperature(); 
    delay(2000); 
    real_temp = t;real_humidity = h;
    std::stringstream ss;  
    ss << h << "," << t;  
    return ss.str();  
}

void output_to_screen(String of,String acmode,String temp)
{
    u8g2.firstPage();
    do{
        //输出空调状态
        u8g2.setCursor(0,10);
        u8g2.print("空调状态:");
        u8g2.setCursor(56,10);
        u8g2.print(of);
        //输出空调温度
        u8g2.setCursor(0,25);
        u8g2.print("空调温度:");
        u8g2.setCursor(56,25);
        u8g2.print(temp);
        //输出空调模式
        u8g2.setCursor(0,40);
        u8g2.print("空调模式:");
        u8g2.setCursor(56,40);
        u8g2.print(acmode);
        //输出室温
        u8g2.setCursor(0,55);
        u8g2.print("室温:");
        u8g2.setCursor(28,55);
        u8g2.print(real_temp);
        //输出湿度
        u8g2.setCursor(64,55);
        u8g2.print("湿度:");
        u8g2.setCursor(92,55);
        u8g2.print(real_humidity);
    }while(u8g2.nextPage());
}

void setup(void) {
    dht.begin();
    if(WiFi.config(staticIP, gateway, subnet, dns, dns) == false);
    WiFi.begin(ssid, passwd);
    while (WiFi.status() != WL_CONNECTED)delay(500);
    server.listen(81);
    u8g2.setBusClock(9000000);
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312b);
    u8g2.setContrast(200);
    u8g2.begin();
    output_to_screen(acof,acmode,actemp);
}

void loop(){
    auto client = server.accept();
    while(client.available()){
        const char *thdata = get_data().c_str();
        client.send(thdata);
        auto msg = client.readBlocking();
        char readydata[msg.data().length() + 1];
        std::strcpy(readydata, msg.data().c_str());
        char *ptr = strtok(readydata, ",");
        acof = ptr;
        ptr = strtok(NULL, ",");
        acmode = ptr;
        ptr = strtok(NULL, ",");
        actemp = ptr;
        output_to_screen(acof,acmode,actemp);
        delay(3000);
        continue;
    }
    client.close();
    delay(100);
}