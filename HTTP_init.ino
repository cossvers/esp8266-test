void HTTP_OTA () {                                          // Start OTA
  Serial.println("Start OTA"); 
// ArduinoOTA.setPort(8266);                                  // строчка для номера порта по умолчанию,  можно вписать «8266»:
ArduinoOTA.setHostname ("111");                               // строчка для названия хоста по умолчанию;  можно вписать «esp8266-[ID чипа]»:
/*
// ArduinoOTA.setPassword((const char *)"123");               // строчка для аутентификации.  (по умолчанию никакой аутентификации не будет):

ArduinoOTA.onEnd([]() {
  Serial.println("\nEnd");                                    //  "Завершение OTA-апдейта"
});
ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
});
ArduinoOTA.onError([](ota_error_t error) {
  Serial.printf("Error[%u]: ", error);
   if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");                           //  "Ошибка при аутентификации"
   else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");                          //  "Ошибка при начале OTA-апдейта"
   else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");                        //  "Ошибка при подключении"
   else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");                        //  "Ошибка при получении данных"
   else if (error == OTA_END_ERROR)
      Serial.println("End Failed");                            //  "Ошибка при завершении OTA-апдейта"
});
*/
ArduinoOTA.begin();
}

void SSDP_init () {                                                                //Настраиваем и запускаем SSDP интерфейс
  Serial.println("Start SSDP");
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(portt);
  SSDP.setName(SSDP_Name);
  SSDP.setSerialNumber("001788102201");
  SSDP.setURL("/");
  SSDP.setModelName("FSWebServer");
  SSDP.setModelNumber("000000000001");
  SSDP.setModelURL("http://esp8266-arduinoide.ru/step4-fswebserver/");
  SSDP.setManufacturer("Tretyakov Sergey");
  SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
  SSDP.begin();
}

void STARTSERVER(){                                 // Выводим сообщение о том, что устройство соединилось с WiFi-сетью 
  
  Serial.print("Local IP: ");                                 // Выводим на монитор серийного порта сообщение о том, что сейчас будем выводить локальный IP
  Serial.println(WiFi.localIP());                             // Выводим локальный IP-адрес ESP8266
  Serial.print("MAC-address: ");                              // Выводим на монитор серийного порта сообщение о том, что сейчас будем выводить MAC-адрес ESP8266
  Serial.println(WiFi.macAddress());                          // Выводим MAC-адрес ESP8266
  CAPS_LOCK ();
  Serial.println("HTTP server started");                      // Выводим на монитор серийного порта сообщение о том, что веб-сервер запущен
  Serial.print("Server: ");
  Serial.println("http://cossver.hldns.ru:10200");
  Serial.print("Update: ");
  Serial.println("http://cossver.hldns.ru:10200/update");
  Serial.print("Restart: ");
  Serial.println("http://cossver.hldns.ru:10200/restart?device=ok");
  Serial.print("Server: ");
  Serial.println("http://cossver.hldns.ru:10200");
  Serial.print("scan: ");
  Serial.println("http://cossver.hldns.ru:10200/scan");
  Serial.print("Restartt: ");
  Serial.println("http://cossver.hldns.ru:10200/restartt");



 
}

 void handleRoot() {
 char temp[400];

  snprintf(temp, 400,
    "<html>\
  <head>\
  </head>\
    <body>\
    <h1>How to html page really good</h1>\
    <a href =\"/0\">Turn OFF</a>\
    <a href =\"/1\">Turn ON</a>\
    <a href =\"/1\"> <button> Turn ON</button> </a>\
    <a href =\"/0\"> <button> Turn OFF</button> </a>\
  </body>\
</html>");
  HTTP.send(200, "text/html", temp);
//  server.send(200, "text/plain", "I'm an awasome webServer!!!");

}

void lOn() {
 HTTP.send(200, "text/plain", "turn on the LED!!!");
 analogWrite(dis, 1024);
}

void lOff() {
  HTTP.send(200, "text/plain", "Turn off the LED");
analogWrite(dis, 0);
}

void Flash() {
  HTTP.send(200, "text/plain", "LED flashing");
analogWrite(dis, 1024);
  delay(500);
analogWrite(dis, 0);
  delay(500);
analogWrite(dis, 1024);
  delay(500);
analogWrite(dis, 0);
  delay(500);
analogWrite(dis, 1024);
  delay(500);
analogWrite(dis, 0);
  delay(500);
analogWrite(dis, 1024);
  delay(500);
analogWrite(dis, 0);
  delay(500);
}

void LEDdim(){
Serial.println(dis);
WIFIAP_Client();
     /* Serial.println("DIM ON"); 
  for(int j = 0;j < 1; j++){  
  int i = 0;
  for ( ; i < 1023 ;i++){
     analogWrite(dis, i);
     //  Serial.println(i); 
  }
    for ( ; i > 0 ;i--){
     analogWrite(dis, i);
      // Serial.println(i); 
  }
    //Serial.println("DIM OFF"); 
}
}*/
}

void ftp_begin () {                                // Инициализируем FTP-сервер (на 21-й порт)
  Serial.println("WiFi connected");
  ftpSrv.begin(FTP_username,FTP_password);
}

void SPIFFS_begin() {                                     // Инициализируем работу с файловой системой
 SPIFFS.begin();
 /* if (SPIFFS.format())                    // Если удалось отформатировать файловую систему
    Serial.println("File System Formated"); // Выводим уведомление об этом
  else                                    // иначе
    Serial.println("File System Formating error");  // Выводим сообщение об ошибке
*/
}
