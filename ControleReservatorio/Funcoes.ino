void inicializaSD() {
	mensagem(F("CARTAO SD!"), F(""), true);
	delay(1000);
	if (!SD.begin(chipSelect))
	{
		mensagem(F("ERRO!"), F("Sem Cartao SD."), false);
	}
	mensagem(F("SUCESSO!"), F("Cartao OK."), true);
}

void conectaWifi() {
	buscaAutenticacaoWifi(lerArquivoConfigWifi());
	setModoOperacao();
	mensagem(F("CONECTANDO!"), "", true);
	if (!wifi.joinAP(SSID, PASSWORD)) {
		mensagem(F("ERRO!"), F("WIFI Desconectada."), false);   
	}
	disableMUX();
}

void setModoOperacao() {
	mensagem(F("MODO OPR.!"), F(""), true);
	delay(1000);
	if (!wifi.setOprToStationSoftAP()) {
		mensagem(F("ERRO!"), F("Erro no Modo de Opr."), false);
	} 
	mensagem(F("SUCESSO.!"), F("Modo operacao OK."), true);
}

void disableMUX() {
	mensagem(F("SINGLE!"), F(""), true);
	delay(1000);
	if (!wifi.disableMUX()) {
		mensagem(F("ERRO!"), F("Single erro."), false);
	}
	mensagem(F("SUCESSO.!"), F("Single OK."), true);
}

String lerArquivoConfigWifi() {
	String json = "";
	File dataFile = SD.open(F("wifi.txt"), FILE_READ);
	if (dataFile)
	{
		while (dataFile.available()) {
			char linha = dataFile.read();
			json += linha;
		}
		dataFile.close();
	}
	else
	{
		mensagem(F("ERRO!"), F("wifi.txt nao encontrado."), false);
	}
	dataFile.close();
	Serial.println(json);
	return json;
}

void buscaAutenticacaoWifi(String json) 
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);
	if (root.success())
	{
		SSID = root["wifi"]["ssid"].asString();
		PASSWORD = root["wifi"]["password"].asString();
	 return;
	}
	SSID = "";
	PASSWORD = "";
}

void efetuaRequisicao() {
	uint8_t buffer[1024] = { 0 };

	if (!wifi.createTCP(HOST_NAME, HOST_PORT)) {
		mostraDisplay(F("ERRO!"), F("Falha no TCP."), false);
	}

	String requisicao = getLogin();
	wifi.send((const uint8_t*)requisicao.c_str(), strlen(requisicao.c_str()));

	//char *hello = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: close\r\n\r\n";
	//wifi.send((const uint8_t*)hello, strlen(hello));

	uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
	if (len > 0) {
		Serial.print("RESPOSTA:[");
		for (uint32_t i = 0; i < len; i++) {
			Serial.print((char)buffer[i]);
		}
		Serial.print("]\r\n");
	}

	if (!wifi.releaseTCP()) {
		mostraDisplay(F("ERRO!"), F("Falha no TCP."), false);
	}			
}

String getLogin() {
	//String str = "GET https://senac-tcs-api.herokuapp.com/users/sign_in";
	//str += "HTTP/1.1\r\nHost: ";
	//str += host;
	//str += "\r\nConnection: close\r\n\r\n";
	return "";//str;
}

void calculaVazao()
{
	contaPulso = 0;
	sei();
	delay(1000);
	cli();
	
	vazao = contaPulso / 5.5;
	consumo = consumo + vazao;
	segundo++;
	if (segundo == 60)
	{
		segundo = 0;
		minuto++;
	}
	if (minuto == 10)
	{
		vazaoTotal = consumo / (60 * 10);
		minuto = 0;
	}
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
