#include<string.h>
#include<stdlib.h>
#include<cstring>
#include<FFat.h>
#include<SPIFFS.h>

byte code[289];
String ofh="开",mdh="制冷";
int of=1,temp=8,mode=2,wind=1;

void turn_to_byte(char a[],byte b[]) //定义一个把存有16进制字符串的数据转化为byte数组的函数
{
    char *ptr = strtok(a, ",");
    for (int i = 0; i < 289; i++) {
        b[i] = (int)(unsigned char)strtol(ptr, NULL, 16); // 将字符串转换为十六进制数，并存储到数组b中
        ptr = strtok(NULL, ",");
    }
}

void get_lr_str(char key[])
{
    String str;
    char *ptr,rn[1800];
    int cnum=-1;
    File configFile = SPIFFS.open("/codes.conf", "r"); //打开文件
    while (configFile.available())
    {
        cnum = 1;
        String line = configFile.readStringUntil('\n'); //逐行读取（读取到\n,也就是换行符时停止）
        for(int i=0;i<7;i++)if(key[i]!=line[i]){cnum=0;break;}//进行搜索
        if(cnum==0)continue;
        else if(cnum==1)
        {
            char lineArray[line.length() + 1];  // 创建一个 char 数组来存储 line 的内容  
            std::strcpy(lineArray, line.c_str());  // 将 line 的内容复制到 lineArray  
            ptr = strtok(lineArray, ":");  // 使用 strtok 分割 lineArray
            ptr = strtok(NULL, "\n");  //循环使用 strtok 分割数组
            strcpy(rn,ptr);
            turn_to_byte(rn,code);
            break;
        }
    } 
    configFile.close();
}

void send_code(int temp,int mode,int wind)
{
    if(of==1){
        char key[]="off0000";
        get_lr_str(key);
        Serial.write(code,289);
    }
    else{
    if(mode==0){
        char key[]="coldxxx";
        if(temp>=10){key[4]='1';key[5]=temp%10+48;key[6]=wind+48;}
        else{key[4]='0';key[5]=temp+48;key[6]=wind+48;}
        get_lr_str(key);
        Serial.write(code,289);
    }
    else if(mode==1) {
        char key[]="warmxxx";
        if(temp>=10){key[4]='1';key[5]=temp%10+48;key[6]=wind+48;}
        else{key[4]='0';key[5]=temp+48;key[6]=wind+48;}
        get_lr_str(key);
        Serial.write(code,289);
    }
    else if(mode==2){
        char key[]="auto000";
        get_lr_str(key);
        Serial.write(code,289);
        }
    }
}

//以下为杂七杂八函数们

void change_of()
{
    if(of==1)of=0;
    else of=1;
}

void change_mode()
{
    if(mode==2)mode=0;
    else mode+=1;
}

void change_temp(int command)
{
    if(!((temp==0&&command==0)|(temp==14&&command==1)))
        {
            if(command==1)temp+=1;
            else temp-=1; 
        }
    else temp=temp;
}

void aircon(const String command)
{
    if(command=="1"){
        change_of();
        send_code(temp,mode,wind);
    }
    if(command=="2"){
        change_mode();
        send_code(temp,mode,wind);
    }
    if(mode==2){temp=temp;wind=wind;}
    else{
        if(command=="3"){
            change_temp(1);
            send_code(temp,mode,wind);
        }
        else if(command=="4"){
            change_temp(0);
            send_code(temp,mode,wind);
        }
    }
}

String json_send()
{
    if(of==1)ofh="关";
    else ofh="开";
    if(mode==0)mdh="制冷";
    else if(mode==1)mdh="制热";
    else mdh="自动";
    return("{\"of\":\""+ofh+"\",\"mode\":\""+mdh+"\",\"temp\":\""+(temp+16)+"\"}"); //没错，手搓一串json
}
