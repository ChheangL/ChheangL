//https://resource.heltec.cn/download/e-ink/e-ink-module-user-manual.pdf
//the documentation
//https://github.com/HelTecAutomation/e-ink
//the libarary
//NTP Server : https://lastminuteengineers.com/esp32-ntp-server-date-time-tutorial/
// Brown Out trigger solution : https://arduino.stackexchange.com/questions/76690/esp32-brownout-detector-was-triggered-upon-wifi-begin

#include "QYEG0213RWS800_BWR.h"
#include "imgData.h"
    #define UNDEFINED -1
    #define RST_PIN         16
    #define DC_PIN          22
    #define CS_PIN          5
    #define BUSY_PIN        4
    #define CLK_PIN         18
    QYEG0213RWS800_BWR epd213bwr(RST_PIN, DC_PIN, CS_PIN, BUSY_PIN, CLK_PIN);//reset_pin, dc_pin, cs_pin, busy_pin, clk_pin
//NTPSERVER    
#include <WiFi.h>
#include <time.h>
#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  3600
const char* ssid       = "MTech_Solution2";
const char* password   = "No88138Jun2011";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7;
const int   daylightOffset_sec = 0;
RTC_DATA_ATTR unsigned int priDate[3] ={0,0,0}; //optional 
RTC_DATA_ATTR unsigned int CurDate[3]= {0,0,0}; //optional
RTC_DATA_ATTR unsigned int count = 0; // RTC count


struct tm tmstruct ;
void setup() {
  Serial.begin(115200);
  pinMode(16,OUTPUT);
  WiFi.begin(ssid, password);
      delay(3000);
  epd213bwr.EPD_HW_Init();
if(count==0){
     //Electronic paper initialization
     //Enter deep sleep 
    epd213bwr.EPD_Black_image(bgBlack);
    epd213bwr.EPD_Red_image(background);
}
  Serial.println(WiFi.status());
  Serial.println(WiFi.localIP());
  setTimeToWifi();
getLocalTime(&tmstruct);
CurDate[0] = tmstruct.tm_mday;
  CurDate[1] = (tmstruct.tm_mon)+1;
  CurDate[2] = (tmstruct.tm_year)+1900;
  if(CurDate[0] != priDate[0]){
    int d1 = CurDate[0]/10;
    int d2 = CurDate[0]%10;
    epd213bwr.EPD_Dis_Part(55,232,digits[d1],MONO,21,24); //first digit
    epd213bwr.EPD_Dis_Part(55,208,digits[d2],MONO,21,24); //second digit
    if(CurDate[1] != priDate[1]){
      d1 = CurDate[1]/10;
      d2 = CurDate[1]%10;
      epd213bwr.EPD_Dis_Part(55,167,digits[d1],MONO,21,24); //third digit
      epd213bwr.EPD_Dis_Part(55,143,digits[d2],MONO,21,24); //fourth digit
      if(CurDate[2] != priDate[2]){
        d1 = CurDate[2]/1000;
        d2 = CurDate[2]/100%10;
        int d3 = CurDate[2]/10%10;
        int d4 = CurDate[2]%10;
        epd213bwr.EPD_Dis_Part(55,103,digits[d1],MONO,21,24); //fifth digit
        epd213bwr.EPD_Dis_Part(55,79,digits[d2],MONO,21,24); //sixth digit
        epd213bwr.EPD_Dis_Part(55,54,digits[d3],MONO,21,24); //seventh digit
        epd213bwr.EPD_Dis_Part(55,30,digits[d4],MONO,21,24); //eigth digit
        priDate[2] = CurDate[2];
      }
      priDate[1] =  CurDate[1];
    }
    priDate[0] = CurDate[0];
  }
  int k = TIME_TO_SLEEP-getHr();
  Serial.printf("The Count is %u\n",count);
  if(count !=0) k += 22*count;
  Serial.printf("\n %d",k);
  count++;
  //epd213bwr.EPD_Update();
  esp_sleep_enable_timer_wakeup((k)*uS_TO_S_FACTOR);
  epd213bwr.EPD_Update();
  epd213bwr.EPD_DeepSleep();
  esp_deep_sleep_start();
  //WiFi.disconnect(false);
  //setTimeToWifi();
  Serial.print(".");
    
}

void loop() {
  
}


int getHr(){
  getLocalTime(&tmstruct);
  int minite = tmstruct.tm_min;
  int sec = tmstruct.tm_sec;
   int s = (minite*60)+sec;
  Serial.print(minite);
  Serial.print(",");
  Serial.print(sec);
  Serial.print(",");
    Dis_time();
   return s;
}

void Dis_time(){
  configTime(gmtOffset_sec*3600, daylightOffset_sec*3600, "pool.ntp.org");
  getLocalTime(&tmstruct);
  int hr = tmstruct.tm_hour;
  //int hr = count;
  Serial.println(hr);


  if(hr >= 12){
    hr = hr - 12;
    if(hr == 0){
      epd213bwr.EPD_Dis_Part(15,183,erRed,RED,118,8);
    }
    epd213bwr.EPD_Dis_Part(15,183-(hr*10),bits,MONO,8,8);
    //Serial.println(183-(hr*10));
  }else{
    if(hr == 0){
      epd213bwr.EPD_Dis_Part(15,183,erBlack,MONO,118,8);
    }
    epd213bwr.EPD_Dis_Part(15,183-((hr)*10),bitsR,RED,8,8);
  }

}

void setTimeToWifi(){
    //connect to WiFi
  //WiFi.mode(WIFI_ON);
  
  //init and get the time
  configTime(gmtOffset_sec*3600, daylightOffset_sec*3600, "pool.ntp.org");
  tmstruct.tm_year = 0;
  //getLocalTime(&tmstruct);  
  //disconnect WiFi as it's no longer needed
  //WiFi.disconnect(true);
  //WiFi.mode(WIFI_OFF);
} 
