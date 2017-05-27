void inicializaSD() {
	mensagem(F("CARTAO SD!"), F(""), true);
	delay(1000);
	if (!SD.begin(chipSelect))
	{
		mensagem(F("ERRO!"), F("Sem Cartao SD."), false);
	}
	mensagem(F("SUCESSO!"), F("Cartao OK."), true);
	delay(1000);
}

void conectaWifi() {
	buscaAutenticacaoWifi(lerArquivoConfigWifi());
	setModoOperacao();
	mensagem(F("CONECTANDO!"), "", true);
	
	SSID = "DLINK";
	PASSWORD = "19891992";
	//SSID = "Senac";
	//PASSWORD = "@senac#alunos";
	//SSID = "MHTEC SISTEMAS";
	//PASSWORD = "Wisermh123+";

	while (!wifi.joinAP(SSID, PASSWORD)) {}
	disableMUX();
}

void setModoOperacao() {
	mensagem(F("MODO OPR.!"), F(""), true);
	delay(1000);
	if (!wifi.setOprToStationSoftAP())
	{
		mensagem(F("ERRO!"), F("Erro no Modo de Opr."), false);
	} 
	mensagem(F("SUCESSO.!"), F("Modo operacao OK."), true);
	delay(1000);
}

void disableMUX() {
	mensagem(F("SINGLE!"), F(""), true);
	delay(1000);
	if (!wifi.disableMUX()) 
	{
		mensagem(F("ERRO!"), F("Single erro."), false);
	}
	mensagem(F("SUCESSO.!"), F("Single OK."), true);
	delay(1000);
}

String lerArquivoConfigWifi() {
	String json = "";
	File dataFile = SD.open(F("wifi.txt"), FILE_READ);
	if (dataFile)
	{
		while (dataFile.available()) 
		{
			char linha = dataFile.read();
			json += linha;
		}
		dataFile.close();
	}
	else
	{
		mensagem(F("ERRO!"), F("arquivo wifi nao encontrado."), false);
	}
	dataFile.close();
	return json;
}

void lerArquivoSetings() {
	String json = "";
	EMAIL = "";
	SENHA = "";
	IDS1 = "";
	IDS2 = "";
	IDS3 = "";
	IDS4 = "";
	IDS5 = "";
	IDS6 = "";
	IDSVazao = "";

	File dataFile = SD.open(F("setings.txt"), FILE_READ);
	if (dataFile)
	{
		while (dataFile.available())
		{
			char linha = dataFile.read();
			json += linha;
		}
		dataFile.close();
	}
	else
	{
		mensagem(F("ERRO!"), F("arquivo setings nao encontrado."), false);
	}
	dataFile.close();
	
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);
	if (root.success())
	{
		EMAIL = root["user"]["email"].asString();
		SENHA = root["user"]["password"].asString();
		IDSVazao = root["device"]["flow_sensors"][0]["id"].asString();
		setIdLevelSensor(root);
	}
	root.end();
}

void setIdLevelSensor(JsonObject& root)
{
	for (size_t i = 0; i < root["device"]["rulers"][0]["level_sensors"].size(); i++)
	{
		int pin = root["device"]["rulers"][0]["level_sensors"][i]["pin"];
		switch (pin)
		{
		case 22:
			IDS1 = root["device"]["rulers"][0]["level_sensors"][i]["id"].asString();
			break;
		case 24:
			IDS2 = root["device"]["rulers"][0]["level_sensors"][i]["id"].asString();
			break;
		case 26:
			IDS3 = root["device"]["rulers"][0]["level_sensors"][i]["id"].asString();
			break;
		case 28:
			IDS4 = root["device"]["rulers"][0]["level_sensors"][i]["id"].asString();
			break;
		case 30:
			IDS5 = root["device"]["rulers"][0]["level_sensors"][i]["id"].asString();
			break;
		case 32:
			IDS6 = root["device"]["rulers"][0]["level_sensors"][i]["id"].asString();
			break;
		}
	}
}

void buscaAutenticacaoWifi(String json) 
{
	SSID = "";
	PASSWORD = ""; 
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);
	if (root.success())
	{
		SSID = root["wifi"]["ssid"].asString();
		PASSWORD = root["wifi"]["password"].asString();
	}
	root.end();	
}

void efetuaRequisicao() {
	Serial.println(F("EFETUANDO REQUISICAO"));
	createTCP();	
	char* response = sendRequest(requestAPI("POST", "/users/sign_in", getJsonLogin(), ""));
	
	HttpResponseClass httpResponse;
	httpResponse.begin(response);
	String Authorization = "";
	if (httpResponse.getStatus() == 200)
	{		
		Authorization = httpResponse.getHeader("Authorization");
		Serial.println();
		Serial.print(F("Authorization: "));
		Serial.println(Authorization);		
		if (IDSVazao != "")
		{
			sendFlowSensorsData(Authorization);			
		}
	}
	else
	{
		Serial.print(F("ERRO: "));
		Serial.println(httpResponse.getStatus());
	}
	Serial.println(F("***** FIM DA REQUISICAO *****"));
	while (true){}
}

void createTCP() 
{
	Serial.print(F("CRIANDO TCP"));
	while (!wifi.createTCP(HOST_NAME, HOST_PORT))
	{
		Serial.print(F("."));
	}
	Serial.println(F("."));
}

char* sendRequest(String httpRequest)
{
	char* response = "";
	delay(500);
	if (wifi.send((const uint8_t*)httpRequest.c_str(), strlen(httpRequest.c_str())))
	{
		uint8_t buffer[1024] = { 0 };
		uint32_t len = wifi.recv(buffer, sizeof(buffer), 5000);
		delay(500);
		if (len > 0)
		{
			response = (char*)buffer;
		}		
	}
	else
	{
		Serial.println(F("ERRO AO EFETUAR REQUISICAO!"));
	}
	return response;
}

void sendFlowSensorsData(String Authorization)
{
	Serial.println();
	Serial.print(F("FLOW SENSOR ID: "));
	Serial.println(IDSVazao);
	Serial.print(F("data: "));
	Serial.println(getJsonFlowSensorsData());

	String urlFlowSensorsData = "/flow_sensors/";
	urlFlowSensorsData += IDSVazao;
	urlFlowSensorsData += "/flow_sensors_data";

	char* response = sendRequest(requestAPI("POST", urlFlowSensorsData, getJsonFlowSensorsData(), Authorization));
	httpResponse.begin(response);
	if (httpResponse.getStatus() == 201)
	{
		Serial.println();
		Serial.println(F("OK!"));
	}
	else
	{
		Serial.print(F("ERRO: "));
		Serial.println(httpResponse.getStatus());
	}
}

String requestAPI(String method, String url, String data, String token) { 
	String str = "";
	str = method;
	str += " ";
	str += url;
	str += " HTTP/1.1\r\n";
	str += "Host: ";
	str += HOST_NAME;
	str += "\r\n";
	str += "User-Agent: Arduino/1.0\r\n";
	str += "Content-Type: application/json\r\n";
	if (token != "")
	{
		str += "Authorization: " + token + "\r\n";
	}
	str += "Cache-Control: no-cache\r\n";
	str += "Content-Length: ";
	str += data.length();
	str += "\r\n\r\n";
	str += data;
	return str;
}

String getJsonLogin()
{
	String retorno = "";
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	JsonObject& json = root.createNestedObject("user");
	json["email"] = EMAIL;
	json["password"] = SENHA;
	root.printTo(retorno);
	root.end();
	return retorno;
}

String getJsonFlowSensorsData()
{
	String retorno = "";
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();	
	root["consumption_per_minute"] = vazaoTotal;
	root["flow_sensor_id"] = IDSVazao;
	root.printTo(retorno);
	root.end();
	return retorno;
}

void calculaVazao()
{
	contaPulso = 0;
	sei();
	delay(1000);
	cli();
	
	vazao = 0;
	vazao = contaPulso / 5.5;
	consumo = consumo + vazao;
	segundo++;
	sei();
  
	Serial.println(segundo);
	if (segundo == 5)
	{
		segundo = 0;
		vazaoTotal = consumo / 60;
		efetuaRequisicao();
	}
	cli();   
}

void mensagem(String titulo, String msg, bool status) 
{
	if (status)
	{
		mostraDisplay(titulo, msg, status);
		return;
	}
	while (1)
	{
		mostraDisplay(titulo, msg, status);
		delay(800);
		limpaDisplay();
		delay(800);
	}	
}

void mostraDisplay(String titulo, String msg, bool status)
{
	u8g.firstPage();
	do
	{
		u8g.setFont(u8g_font_fub11r);
		u8g.drawStr(0, 15, titulo.c_str());
		u8g.setFont(u8g_font_6x10r);
		u8g.drawStr(0, 30, msg.c_str());
	} while (u8g.nextPage());
}

void limpaDisplay()
{
	u8g.firstPage();
	do {} while (u8g.nextPage());
}
