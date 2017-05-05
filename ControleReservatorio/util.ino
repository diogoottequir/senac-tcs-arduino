void setModoOperacao() {
  String msg = "";
  if (!wifi.setOprToStationSoftAP())
  {
    msg = F("Erro ao setar modo de operacao!");
  }
  else
  {
    msg = F("Modo de operacao ok!");
  }
  u8g.firstPage();
  do
  {
    u8g.setFont(u8g_font_8x13B);
    u8g.drawStr(0, 15, F("MODO OPERACAO!"));
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(0, 25, msg.c_str());
  } while (u8g.nextPage());
}

void conectaWifi(String _ssid, String _password) {
  u8g.firstPage();
  do
  {
    u8g.setFont(u8g_font_8x13B);
    u8g.drawStr(0, 15, F("CONECTANDO WIFI!"));
  } while (u8g.nextPage());
  if (!wifi.joinAP(_ssid, _password)) {
    while (1)
    {
      u8g.firstPage();
      do
      {
        u8g.setFont(u8g_font_8x13B);
        u8g.drawStr(0, 15, F("Erro ao conectar!"));
        u8g.drawStr(0, 30, F("WIFI!"));
      } while (u8g.nextPage());
      delay(800);

      u8g.firstPage();
      do
      {
      } while (u8g.nextPage());
      delay(800);
    }
  }
}

void lerArquivoConfigWifi() {
  String ssid = "";
  String password = "";
  String js = " ";
  
  File dataFile = SD.open(F("wifi.txt"), FILE_READ);
  if (dataFile)
  {
    while (dataFile.available()) {
      char linha = dataFile.read();
      js += linha;
    }
    dataFile.close();
  }

  Serial.println(js);
  //char json[] = "";
  
  //StaticJsonBuffer<200> jsonBuffer;
  //JsonObject& root = jsonBuffer.parseObject(json);

  conectaWifi(ssid, password);
  
  /*menssagem = "";
  if (!root.success())
  {
    return;
  }

  sensor = root["sensor"];
  time = root["time"];
  latitude = root["data"][0];
  longitude = root["data"][1];
  
  File dataFile = SD.open("wifi.txt", FILE_READ);
  char json;
  char ssid;
  char password;
  if (dataFile)
  {
  while (dataFile.available()) {
  json = dataFile.read();
  }
  dataFile.close();

  Serial.print(json);

  StaticJsonBuffer<200> jsonBuffer;
  //JsonObject& root = jsonBuffer.parseObject(json);
  //ssid = root["ssid"];
  //password = root["password"];

  Serial.println(ssid);
  Serial.println(password);

  }else {
  Serial.println(F("Erro ao abrir arquivo"));
  }
  
  */
}

void efetuaRequisicao(uint8_t buffer[256]) {
  String cabecalho = getCabecalho();
  Serial.println(F("Efetuando Requisicao:"));
  Serial.println(cabecalho);
  wifi.send((const uint8_t*)cabecalho.c_str(), strlen(cabecalho.c_str()));

  // http://labdegaragem.com/profiles/blogs/arduino-json
  /*if (wifi.send((const uint8_t*)cabecalho.c_str(), strlen(cabecalho.c_str()))) {
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if (len > 0) {
  Serial.print("RESPOSTA:[");
  for (uint32_t i = 0; i < len; i++) {
  Serial.print((char)buffer[i]);
  }
  Serial.print("]\r\n");
  }
  }
  else {
  Serial.println("Erro ao efetuar requisicao!");
  }*/
}

String getCabecalho() {
  //String str = "GET https://senac-tcs-api.herokuapp.com/users/sign_in";
  //str += "HTTP/1.1\r\nHost: ";
  //str += host;
  //str += "\r\nConnection: close\r\n\r\n";
  return "";//str;
}

float calculaVazao()
{
	contaPulso = 0;
	sei();
	delay(1000);
	cli();
	return contaPulso / 5.5;

}



