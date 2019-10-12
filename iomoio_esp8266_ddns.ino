#include <ESP8266WiFi.h>                                                                                     // Библиотека для создания Wi-Fi подключения (клиент или точка доступа)
#include <ESP8266SSDP.h> 
#include <WiFiClient.h>                                                                                      // Библиотека для связи с сетевыми хостами (локальными и интернет)
#include <ESP8266WebServer.h>                                                                                // Библиотека для поднятия веб-сервера
#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>                                                                         //Содержится в пакете.  Видео с уроком http://esp8266-arduinoide.ru/step8-timeupdate/
#include <FS.h>                                                                                              // Библиотека для работы с файловой системой
#include <ESP8266FtpServer.h>   // Библиотека для работы с SPIFFS по FTP
#include <ArduinoJson.h>        //Установить из менеджера библиотек.

static const char xyz[] PROGMEM = "Это строка, сохраненная на flash ";
const PROGMEM uint32_t pig[] = {0 ,1 ,2 , 3};
String _ssid = "Xiaomi";
String _password = "956843287";                         
String _ssidAP = "WiFi";   // SSID AP точки доступа
String _passwordAP = ""; // пароль точки доступа
//const char* update_path = "/firmware";
const char* update_username = "admin";
const char* update_password = "admin";
const char* FTP_username = "admin";
const char* FTP_password = "admin";
String SSDP_Name = "FSWebServer";                                                                         // Имя SSDP
const char* ip_update_host = "hldns.ru";                                                                     // Указываем хост (адрес сайта) службы DynDNS
const char* ip_update_get = "/update/LZE1X3ZII5FWJP28XK374IYECRW3D2";                                        // Указываем GET-запрос (набор параметров) адреса идентификации в службе DynDNS
const int   portt = 10200;
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
byte dis = 2 ;
int timezone = 3;               // часовой пояс GTM
                                                                                           // Создаём объект для работы с FTP

IPAddress apIP(192, 168, 4, 1);
ESP8266WebServer HTTP (portt);                                                                               // Web интерфейс для устройства
ESP8266HTTPUpdateServer httpUpdater;                                                                         // Создаём объект для работы с удалёнными хостами
WiFiClient client; 

FtpServer ftpSrv; 


File fsUploadFile;// Для файловой системы
String jsonConfig = "{}";
void CAPS_LOCK (){  Serial.print("\n");  };

void setup(void){
  Serial.begin(115200); // Инициализируем вывод данных на серийный порт со скоростью 9600 бод
  Serial.println("");
  Serial.println("Start 1-WIFI");
 // dis = pgm_read_word_near(pig + 2);
 Serial. println(FPSTR(xyz));
  //  Serial. println(FPSTR(xyz+1));
  
  WIFIinit();
  SPIFFS_begin();    // Инициализируем работу с файловой системой
  ftp_begin ();     // Инициализируем FTP-сервер (на 21-й порт)               
    Serial.println("Step7-FileConfig");
  loadConfig();
  server_begin();   // Инициализируем работу веб-сервера
  ip_update();      // Вызываем функцию, обновляющую IP маршрутизатора (роутера) в службе динамических DNS                                         
  SSDP_init();      //Настраиваем и запускаем SSDP интерфейс
  HTTP_OTA ();      // Start ElegantOTA
} 
void loop(void){
 /* v = millis(); 
  if (DIMMLED2 == 1){
  if(v - j >= 1000){
      j = millis();
if (dimmSTATE == 50){
 DIMMLED2 = 0;
  Serial.println("DIM == 0 ");
}else{
       dimmSTATE++;
        Serial.println(v); 
        Serial.println(dimmSTATE); 
        }
      /*
      if (l[0] == 3)  { l[0] = 0; }
      if (l[2] == 255){ l[1]++; }
      if (l[1] == 0)  {l[2]++; w ();}
      if (l[1] == 1)  {l[2]--; w ();}
      if (l[2] == 0)  {l[0]++; };
      if (l[2] == 0)  {l[1]--; };
      
    }
  }; */
 HTTP.handleClient();                                      // Ожидаем обращение к веб-серверу
 ArduinoOTA.handle();
 ftpSrv.handleFTP();                     // Отслеживаем обращения по FTP 
 //delay(1);
}
