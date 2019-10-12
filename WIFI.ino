void  WIFIinit() {                                                            // Попытка подключения к точке доступа
  WiFi.mode(WIFI_STA);
  byte tries = 11;
  WiFi.begin(_ssid.c_str(), _password.c_str());
  while (--tries && WiFi.status() != WL_CONNECTED) {                         // Делаем проверку подключения до тех пор пока счетчик tries   не станет равен нулю или не получим подключение
    Serial.print(".");
    delay(1000);
   }
  if (WiFi.status() != WL_CONNECTED) {                                       // Если не удалось подключиться запускаем в режиме AP
    Serial.println("");
    Serial.println("WiFi up AP");
    StartAPMode();
  }
  else {                                                                     // Иначе удалось подключиться отправляем сообщение   о подключении и выводим адрес IP
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

bool StartAPMode(){                                                          // Включаем WIFI в режиме точки доступа с именем и паролем хронящихся в переменных _ssidAP _passwordAP
  WiFi.disconnect();                                                         // Отключаем WIFI
  WiFi.mode(WIFI_AP);                                                        // Меняем режим на режим точки доступа
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));                // Задаем настройки сети
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());    
  return true;
}

void WIFIAP_Client() {                                                      // Включаем WIFI в режиме репитера
  WiFi.disconnect();
  WiFi.mode(WIFI_AP_STA);
  Serial.print(_ssid);
  WiFi.begin(_ssid.c_str(), _password.c_str());
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < 50) {
    delay(500);
    i++;
    Serial.print(".");
  }
}
