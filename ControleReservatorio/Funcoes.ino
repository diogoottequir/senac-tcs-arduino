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
	while (!wifi.joinAP(SSID, PASSWORD)) {}
	disableMUX();
}

void setModoOperacao() {
	mensagem(F("MODO OPR.!"), F(""), true);
	delay(1000);
	if (!wifi.setOprToStationSoftAP()) {
		mensagem(F("ERRO!"), F("Erro no Modo de Opr."), false);
	} 
	mensagem(F("SUCESSO.!"), F("Modo operacao OK."), true);
	delay(1000);
}

void disableMUX() {
	mensagem(F("SINGLE!"), F(""), true);
	delay(1000);
	if (!wifi.disableMUX()) {
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

void lerArquivoSetings() {
	String json = "";
	File dataFile = SD.open(F("setings.txt"), FILE_READ);
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
		mensagem(F("ERRO!"), F("setings.txt nao encontrado."), false);
	}
	dataFile.close();
	Serial.println(json);
	
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);
	if (root.success())
	{
		EMAIL = root["user"]["email"].asString();
		SENHA = root["user"]["password"].asString();
		return;
	}
	EMAIL = "";
	SENHA = "";
	
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
	Serial.println("EFETUANDO REQUISICAO");
	uint8_t buffer[1024] = {0};

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.println("create tcp ok!");
    } else {
        Serial.println("create tcp erro!");
    }

    String hello = getLogin();
    Serial.println(hello);
    delay(500);
    wifi.send((const uint8_t*)hello.c_str(), strlen(hello.c_str()));

	Serial.println("");
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 5000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.println("]");
    }

    if (wifi.releaseTCP()) {
        Serial.println("release tcp ok!");
    } else {
        Serial.println("release tcp erro!");
    }
}

String getLogin() { 
	String data = montaJsonLogin();

	String str = "POST /users/sign_in HTTP/1.1\r\n";
	str += "Host: senac-tcs-api.herokuapp.com\r\n";
	str += "User-Agent: Arduino/1.0\r\n";
	str += "Content-Type: application/json\r\n";
	str += "Cache-Control: no-cache\r\n";
	str += "Content-Length: ";
	str += data.length();
	str += "\r\n";
	str += "\r\n";
	str += data;
	return str;
}

String montaJsonLogin()
{
	String retorno = "";
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	JsonObject& json = root.createNestedObject("user");
	json["email"] = EMAIL;
	json["password"] = SENHA;
	root.printTo(retorno);
	Serial.println(retorno);
	return retorno;
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
	sei();
  
	Serial.println(segundo);
	if (segundo == 60)
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
