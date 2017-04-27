//===================================================
// Programa : Controle de Reservatorio
// Autor : Diogo Ottequir
// Data : 10/09/2016
//===================================================
//Bibliotecas
#include <ArduinoJson.h>
#include <ESP8266.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <U8glib.h>

//===================================================
//Configuracao
#define SSID "DLINK"
#define PASSWORD "19891992"

SoftwareSerial mSerial(0, 1); // RX - TX
ESP8266 wifi(mSerial);
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

//===================================================
//Constantes
const String Versao = "A.01"; //Versao de firmware
const char* host = "192.168.0.197";
const int httpPort = 8080;
int tempoRequisicao = 0;

//Vazao
float vazao = 0;
float vazaoTotal = 0;
float media = 0;
float mediaTotal = 0;
int contaPulso = 0;
int segundo = 0;
int minuto = 0;

//===================================================
// Rotinas
//===================================================
void setup()
{
	// Inicializa Monitor Serial;
	Serial.begin(9600);
	
	// Inicializa display OLED;
	u8g.begin();
	
	// Inicializa o Timer1 a cada 1 segundo;
	Timer1.initialize(1000000);
	Timer1.attachInterrupt(requisicaoAPI);

	//Configura o pino 2(Interrupção 0) para trabalhar como interrupção
	pinMode(2, INPUT);
	attachInterrupt(0, incpulso, RISING);
}

void loop(void)
{
	u8g.firstPage();
	do
	{
		draw();		
	} while (u8g.nextPage());
	calculaVazao();
}

//===================================================
//Funções
void calculaVazao()
{
	contaPulso = 0;
	sei();
	delay(1000);
	cli();

	vazao = contaPulso / 5.5;
	media = media + vazao;
	segundo++;

	if (segundo == 60)
	{
		media = media / 60;
		mediaTotal = mediaTotal + media;
		media = 0;
		segundo = 0;
		minuto++;
	}
	
	if (minuto == 10)
	{
		vazaoTotal = mediaTotal;
		minuto = 0;
		mediaTotal = 0;
	}
}

void draw()
{
	String vz = " = ";
		   vz += vazao;
		   vz += " L/s";
	
	//Linha superior - consumo
	u8g.drawBox(0, 0, 12, 2);
	u8g.drawBox(0, 5, 16, 4);
	u8g.drawBox(13, 6, 4, 4);
	u8g.drawBox(14, 7, 4, 4);
	u8g.drawBox(15, 8, 4, 3);
	u8g.drawBox(15, 10, 4, 3);
	u8g.drawVLine(5, 0, 9);

	u8g.setFont(u8g_font_8x13B);
	u8g.drawStr(20, 15, vz.c_str());
	
	//Nivel 1
	u8g.setFont(u8g_font_fub11);
	u8g.drawStr(0, 30, "R1=");

	u8g.setFont(u8g_font_6x10);
	u8g.drawStr(0, 45, "16.50%");

	u8g.drawRFrame(40, 18, 15, 46, 3);
	u8g.drawBox(43, 55, 9, 5);

	//Nivel 2
	u8g.setFont(u8g_font_fub11);
	u8g.drawStr(70, 30, "R2=");

	u8g.setFont(u8g_font_6x10);
	u8g.drawStr(70, 45, "0.00%");

	u8g.drawRFrame(110, 18, 15, 46, 3);
}

void lerArquivoConfig() {
	/*char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[50.976080,2.302038]}";
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);
	menssagem = "";
	if (!root.success())
	{
		return;
	}

	sensor = root["sensor"];
	time = root["time"];
	latitude = root["data"][0];
	longitude = root["data"][1];

	Serial.println("- - - - - - - -");
	Serial.println("Time: ");
	Serial.println(time);
	Serial.println("Latitude: ");
	Serial.println(latitude);
	Serial.println("Longitude: ");
	Serial.println(longitude);*/

}

//===================================================
//Interrupcoes

void requisicaoAPI()
{
	tempoRequisicao++;
  Serial.println(tempoRequisicao);

  if (tempoRequisicao == 600)
  {
	   //Implementar metodo para efetuar a requisição a api
	   Serial.println("********************");
	   Serial.println("EFETUADA REQUISICAO!");
	   Serial.println("********************"); 
     Serial.println(vazaoTotal);

     tempoRequisicao = 0;
  }
}

void incpulso()
{
	contaPulso++;
}


#pragma region Funcoes
void iniciaConfiguracoes() {
  iniciaConfiguracoes();
  delay(2000);
  defineModoOperacao();
  delay(2000);
  habilitaSingle();
  delay(2000);
  conectaWifi();
  delay(1000);
}

void defineModoOperacao() {
  while (!wifi.setOprToStationSoftAP()) {
    Serial.print(".");
    delay(1000);
  }
}

void habilitaSingle() {
  while (!wifi.setOprToStationSoftAP()) {
    Serial.print(".");
    delay(1000);
  }
}

void conectaWifi() {
  if (!wifi.joinAP(SSID, PASSWORD)) {
    delay(1000);
  }
  else {
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  }
}

void efetuaRequisicao(uint8_t buffer[256]) {
  String cabecalho = getCabecalho();
  Serial.println("Efetuando Requisicao:");
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
  String str = "GET /Arduino/AdicionaTeste?";
  str += "chave=Arduino";
  str += "&nivel=";
  str += "&motor = 1 ";
  str += "HTTP/1.1\r\nHost: ";
  str += host;
  str += "\r\nConnection: close\r\n\r\n";
  return str;
}

#pragma endregion
