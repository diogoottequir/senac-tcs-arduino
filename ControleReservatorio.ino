// Programa : Controle de Reservatorio                              
// Autor : Diogo Ottequir                                           
// Data : 10/09/2016                                                

//Versão de firmware
const String Versao = "A.01";

#pragma region Bibliotecas
	#include <ESP8266.h>
	#include <SoftwareSerial.h>
	#include <LiquidCrystal.h>

#pragma endregion
#pragma region Configuracao
	#define SSID "DLINK"
	#define PASSWORD "19891992"

	//#define SSID "MHTEC SISTEMAS"
	//#define PASSWORD "1307046881"

	SoftwareSerial mSerial(2, 3); // RX - TX
	ESP8266 wifi(mSerial);
	LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

#pragma endregion
#pragma region Variaveis
	const char* host = "192.168.0.197";
	const int httpPort = 8080;
	const int potPin = 0;
	float nivel = 0;
#pragma endregion

// Rotinas
void setup()
{
	iniciaConfiguracoes();
	delay(2000);
	defineModoOperacao();
	delay(2000);
	habilitaSingle();
	delay(2000);  
	conectaWifi();
	delay(1000);  
}

void loop(void)
{	
	lcd.clear();	
	lcd.setCursor(0, 0);
	lcd.print("Conectando em:");
	lcd.setCursor(0, 1);
	lcd.print(host);
	
	delay(1000);
  
	uint8_t buffer[256] = { 0 };
	while (wifi.createTCP(host, httpPort)) {
		mostraDisplay();
		efetuaRequisicao(buffer);		
		wifi.releaseTCP();			    
	}

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("======ERRO======");
	lcd.setCursor(0, 1);
	lcd.print("REINICIANDO.");
	
	delay(1000);
	//finalizaPrograma();
}

#pragma region Funcoes
void iniciaConfiguracoes(){
	Serial.begin(9600);
	lcd.begin(16,2);
  
	lcd.clear();	
	lcd.setCursor(5,0);
	lcd.print("D.D.F.");	
	lcd.setCursor(0, 1);
	lcd.print("TCC SENAC 2017-1");
	
	delay(2000);
  
	lcd.clear();	
	lcd.setCursor(2, 0);
	lcd.print("Iniciando...");	
	lcd.setCursor(2, 1);
	lcd.print("Versao: " + Versao);	
}

void defineModoOperacao(){
	lcd.clear();	
	lcd.setCursor(0, 0);
	lcd.print("Modo de Operacao");
	
	while(!wifi.setOprToStationSoftAP()) {
		Serial.print(".");
		delay(1000);
	}	
	lcd.setCursor(0, 1);
	lcd.print("STATUS OK");
}

void habilitaSingle(){ 
	lcd.clear();	
	lcd.setCursor(0, 0);
	lcd.print("Config. Single");
	
	while(!wifi.setOprToStationSoftAP()) {
		Serial.print(".");
		delay(1000);
	}	
	lcd.setCursor(0, 1);
	lcd.print("STATUS OK");	
}

void conectaWifi(){ 
	lcd.clear();	
	lcd.setCursor(0, 0);
	lcd.print("Conectando WIFI");
	
	if (!wifi.joinAP(SSID, PASSWORD)) {
		lcd.setCursor(0, 1);
		lcd.print("Erro ao Conectar");
		delay(1000);
	} 
	else {
		lcd.setCursor(0, 1);
		lcd.print("Rede Conectada");
		
		Serial.print("IP: ");   
		Serial.println(wifi.getLocalIP().c_str());
	}
}

void mostraDisplay() {
	int valor = analogRead(potPin);
	nivel = valor / 1023.00 * 100.00;

	String str1 = "NIVEL: ";
	str1 += nivel;
	str1 += "%";

	lcd.clear();	
	lcd.setCursor(0, 0);
	lcd.print(str1);
	lcd.setCursor(0, 1);
	lcd.print("MOTOR: DESLIGADO");
}

void efetuaRequisicao(uint8_t buffer[256]) {
	String cabecalho = getCabecalho();
	Serial.println("Efetuando Requisicao:");
	Serial.println(cabecalho);
	wifi.send((const uint8_t*)cabecalho.c_str(), strlen(cabecalho.c_str()));

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

String getCabecalho(){
	String str = "GET /Arduino/AdicionaTeste?";
	str += "chave=Arduino";
	str += "&nivel=";
	str += nivel;
	str += "&motor = 1 ";
	str += "HTTP/1.1\r\nHost: ";
	str += host;
	str += "\r\nConnection: close\r\n\r\n";
	return str;
}

//void finalizaPrograma(){
//	Serial.println("========== Erro ==========");
//	Serial.println("Reinicie o sistema.");
//	Serial.println("==========================");
//	while(1){}
//}

#pragma endregion
