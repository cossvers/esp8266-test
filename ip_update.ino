void server_begin(){                                                                         // Инициализируем работу веб-сервера
    Serial.println("WiFi connected"); 
    httpUpdater.setup(&HTTP, update_username, update_password );           // update_path,
    HTTP.begin();                               
    HTTP.on("/restartt",       HTTP_GET,  [](){                                                                 //             http://cossver.hldns.ru:10200/restartt
      HTTP.send(200, "text / plain", "TEST   Reset OK");
      ESP.restart(); 
    });
    HTTP.on("/scan",           scann            );                                                              //             http://cossver.hldns.ru:10200/scan
    HTTP.on("/restart",        HTTP_GET,  [](){                                 
      String restart = HTTP.arg("device");                                                                         // Получаем значение device из запроса
    if (restart == "ok") {                                                                                      // Если значение равно Ок
      HTTP.send(200, "text / plain", "Reset OK");                                                               // Oтправляем ответ Reset OK
      ESP.restart();                                                                                            // перезагружаем модуль
    }
    else {                                                                                                      // иначе
      HTTP.send(200, "text / plain", "No Reset");                                                               // Oтправляем ответ No Reset
    }
  });                                                                                                             //             http://cossver.hldns.ru:10200/restart?device=ok
    HTTP.on("/",               HTTP_GET,  [](){                                                                 //             http://cossver.hldns.ru:10200
      HTTP.sendHeader("Connection", "close");
      HTTP.sendHeader("Access-Control-Allow-Origin", "*");
      HTTP.send(200, "text/html", serverIndex);
    });
    HTTP.on("/update",         HTTP_POST, [](){                                                                 //             http://cossver.hldns.ru:10200/update
      HTTP.sendHeader("Connection", "close");
      HTTP.sendHeader("Access-Control-Allow-Origin", "*");
      HTTP.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    },[](){
      HTTPUpload& upload = HTTP.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
    HTTP.on("/led", handleRoot );
    HTTP.on("/0", lOff );
    HTTP.on("/1", lOn );
    HTTP.on("/2", Flash ); 
    HTTP.on("/3", LEDdim );
    STARTSERVER();      // Выводим сообщение о том, что устройство соединилось с WiFi-сетью 

}

void ip_update(){   // Функция, обновляющая IP маршрутизатора (роутера) в службе динамических DNS   
  Serial.println("Start ip_update");
  if (client.connect(ip_update_host,80)) {                    // Если возможно соединиться с хостом на 80-й порт:
    client.print("GET ");                                     //  Эта и следующие 7 строк отправляют системные заголовки на сервер службы динамических DNS
    client.print(ip_update_get);                              //  Формат заголовков строгий и в нём важны даже переводы строк
    client.println( " HTTP/1.1");                             //
    client.print("Host:");                                    //
    client.println(ip_update_host);                           //
    client.println( "Connection: close" );                    //
    client.println();                                         //
    client.println();                                         //
    delay(200);                                               //  Задержка в 0.2 сек., чтобы дать удалённому серверу возможность обработать и отправить ответ на наш запрос
    Serial.println("\nResponse of DDNS-Server:");             //  Шлём в монитор серийного порта сообщение о том, что далее будет выводиться ответ от сервера службы DynDNS
    while (client.available()) {                              //  Пока сервер на связи
      char a = client.read();                                 //    считываем его ответ по одному символу 
      Serial.print(a);                                        //    и выводим на монитор серийного порта
    }
  }
  Serial.println("ip_update Connected");
  delay(10);
}

void scann() {       // WiFi.scanNetworks will return the number of networks found
  delay(10);
  WiFi.disconnect();
  delay(100);
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i){          // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");
  delay(2000);
  WiFi.begin(_ssid.c_str(), _password.c_str()); 
   while (WiFi.status() != WL_CONNECTED){                       // Пока соединение не установено
    delay(500); //  делаем задержку в пол секунды, пока соединение не установится
    Serial.print(".");
}                                               //  делаем задержку в пол секунды, пока соединение не установится
  CAPS_LOCK (); 
  Serial.println("WiFi connected");                           // Выводим сообщение о том, что устройство соединилось с WiFi-сетью 
}

// Загрузка данных сохраненных в файл  config.json
bool loadConfig() {
  // Открываем файл для чтения
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
  // если файл не найден  
    Serial.println("Failed to open config file");
  //  Создаем файл запиав в него аные по умолчанию
    saveConfig();
    configFile.close();
    return false;
  }
  // Проверяем размер файла, будем использовать файл размером меньше 1024 байта
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    configFile.close();
    return false;
  }

// загружаем файл конфигурации в глобальную переменную
  jsonConfig = configFile.readString();
  configFile.close();
  // Резервируем памяь для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266 
    DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  //  строку возьмем из глобальной переменной String jsonConfig
    JsonObject& root = jsonBuffer.parseObject(jsonConfig);
  // Теперь можно получить значения из root  
    _ssidAP = root["ssidAPName"].as<String>(); // Так получаем строку
    _passwordAP = root["ssidAPPassword"].as<String>();
    timezone = root["timezone"];               // Так получаем число
    SSDP_Name = root["SSDPName"].as<String>();
    _ssid = root["ssidName"].as<String>();
    _password = root["ssidPassword"].as<String>();
    return true;
}

// Запись данных в файл config.json
bool saveConfig() {
  // Резервируем память для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266 
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json 
  json["SSDPName"] = SSDP_Name;
  json["ssidAPName"] = _ssidAP;
  json["ssidAPPassword"] = _passwordAP;
  json["ssidName"] = _ssid;
  json["ssidPassword"] = _password;
  json["timezone"] = timezone;
  // Помещаем созданный json в глобальную переменную json.printTo(jsonConfig);
  json.printTo(jsonConfig);
  // Открываем файл для записи
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    configFile.close();
    return false;
  }
  // Записываем строку json в файл 
  json.printTo(configFile);
  configFile.close();
  return true;
  }
