#include<string.h>
#include<stdlib.h>
#include<cstring>
#include<FFat.h>
#include<SPIFFS.h>
#include <ArduinoWebsockets.h>
#define LENTH 5

byte code[LENTH]; //存有红外编码的字节数组，一共LENTH位16进制数
String ofh = "开", mdh = "制冷",real_temp = "-1",real_humidity = "-2"; //定义接下来发送JSON的数据
int of=1,temp=8,mode=2,wind=1; //初始开关，温度，风速，自动模式  
const char* websockets_server_host ="yout_ESP32C3_ip";
const uint16_t websockets_server_port = 81;
using namespace websockets;
WebsocketsServer server;
WebsocketsClient client1;
String *ofh2=&ofh,*mdh2=&mdh;
int *temp2=&temp;

void turn_to_byte(char a[], byte b[]) //定义一个把存有16进制字符串的数据转化为byte数组的函数
{
  char *ptr = strtok(a, ",");  //根据","分割
  for (int i = 0; i < LENTH; i++) {
    b[i] = (int)(unsigned char)strtol(ptr, NULL, 16); // 将字符串转换为十六进制数，并存储到数组b中
    ptr = strtok(NULL, ",");
  }
}

void get_lr_str(char key[]) //根据键（key）搜索配置文件中的红外码并存入数组
{
  String str;
  char *ptr, rn[1800];
  int cnum = -1;
  File configFile = SPIFFS.open("/codes.conf", "r"); //打开文件
  while (configFile.available())
  {
    cnum = 1;
    String line = configFile.readStringUntil('\n'); //逐行读取（读取到\n,也就是换行符时停止）
    for (int i = 0; i < 7; i++)if (key[i] != line[i]) {
        cnum = 0;  //进行搜索
        break;
      }
    if (cnum == 0)continue;
    else if (cnum == 1)
    {
      char lineArray[line.length() + 1];  // 创建一个 char 数组来存储 line 的内容
      std::strcpy(lineArray, line.c_str());  // 将 line 的内容复制到 lineArray
      ptr = strtok(lineArray, ":");  // 使用 strtok 分割 lineArray
      ptr = strtok(NULL, "\n");  //循环使用 strtok 分割数组
      strcpy(rn, ptr);
      turn_to_byte(rn, code);
      break;
    }
  }
  configFile.close();
}

void send_code(int temp, int mode, int wind)
{
  if (of == 1) { //开关状态优先级最高，在关的状态下，无法改变空调其他状态
    char key[] = "off0000";
    get_lr_str(key);
    Serial.write(code, LENTH);
  }
  else {
    if (mode == 0) {  //生成制冷状态的key并且发射红外编码
      char key[] = "coldxxx";
      if (temp >= 10) {
        key[4] = '1';
        key[5] = temp % 10 + 48;
        key[6] = wind + 48;
      }
      else {
        key[4] = '0';
        key[5] = temp + 48;
        key[6] = wind + 48;
      }
      get_lr_str(key);
      Serial.write(code, LENTH);
    }
    else if (mode == 1) {  //生成制热状态的key并且发射红外编码
      char key[] = "warmxxx";
      if (temp >= 10) {
        key[4] = '1';
        key[5] = temp % 10 + 48;
        key[6] = wind + 48;
      }
      else {
        key[4] = '0';
        key[5] = temp + 48;
        key[6] = wind + 48;
      }
      get_lr_str(key);
      Serial.write(code, LENTH);
    }
    else if (mode == 2) { //自动模式
      char key[] = "auto000";
      get_lr_str(key);
      Serial.write(code, LENTH);
    }
  }
}

//以下为杂七杂八函数们

void change_of(){of=!of;}

void change_mode()
{
  if (mode == 2)mode = 0;
  else mode += 1;
}

void change_temp(int command)
{
  if (!((temp == 0 && command == 0) | (temp == 14 && command == 1)))
  {//限制温度最大值为30，最小值为16
    if (command == 1)temp += 1;
    else temp -= 1;
  }
  else temp = temp;
}

void aircon(const String command)
{
  if (command == "1") {
    change_of();
    send_code(temp, mode, wind);
  }
  if (command == "2") {
    change_mode();
    send_code(temp, mode, wind);
  }
  if(mode!=2) {
    if (command == "3") {
      change_temp(1);
      send_code(temp, mode, wind);
    }
    else if (command == "4") {
      change_temp(0);
      send_code(temp, mode, wind);
    }
  }
}

String json_send()
{
  if (of == 1)ofh = "关";
  else ofh = "开";
  if (mode == 0)mdh = "制冷";
  else if (mode == 1)mdh = "制热";
  else mdh = "自动";
  return ("{\"of\":\"" + ofh + "\",\"mode\":\"" + mdh + "\",\"temp\":\"" + (temp + 16) + "\",\"real_temp\":\""+real_temp+"\",\"real_humidity\":\""+real_humidity+"\"}"); //没错，手搓一串json
}

void loop2(void *loop2)
{
    while(!client1.connect(websockets_server_host, websockets_server_port, "/"))continue;
    Serial.println("成功连接");
    client1.onMessage([&](WebsocketsMessage message){
        char readydata[message.data().length() + 1];
        std::strcpy(readydata, message.data().c_str());
        char *ptr = strtok(readydata, ",");
        real_humidity = ptr;
        ptr = strtok(NULL, ",");
        real_temp = ptr;
    });
    while(true)
    {
        if(client1.available())client1.poll();
        client1.send(*ofh2+","+*mdh2+","+(*temp2+16));
    }
}