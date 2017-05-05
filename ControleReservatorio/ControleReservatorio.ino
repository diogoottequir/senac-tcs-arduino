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
#include <SD.h>

//===================================================
//Configuracao
SoftwareSerial mSerial(6, 7); // RX - TX
ESP8266 wifi(mSerial);
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Sd2Card SDcard;
SdVolume volume;

//===================================================
//Variaveis Globais

//Versao do firmware
#define Versao "A.01" 

// Sensor nÍvel pinos 8, 9, 10, A1, A2 e A3
#define S1 8 
#define S2 9
#define S3 10
#define S4 15
#define S5 16
#define S6 17

// Pino ligado ao CS do modulo
#define chipSelect 4

// Sensor vazão pino 2
#define SVazao 2

//Internet
byte tempoRequisicao = 0;

//Vazao
float vazaoTotal = 0;
float mediaTotal = 0;
int contaPulso = 0;

//===================================================
// Rotinas
//===================================================
void setup()
{
	// Seta sensores de nível;
	pinMode(S1, INPUT);
	pinMode(S2, INPUT);
	pinMode(S3, INPUT);
	pinMode(S4, INPUT);
	pinMode(S5, INPUT);
	pinMode(S6, INPUT);

	// Inicializa Monitor Serial;
	Serial.begin(9600);

	// Inicializa display OLED;
	u8g.begin();
	
	String firmware = F("-Versao ESP8266: ");
	firmware += wifi.getVersion();

	String V = F("-Versao ");
	V += Versao;
	u8g.firstPage();
	do
	{
		u8g.setFont(u8g_font_8x13B);
		u8g.drawStr(0, 15, F("INICIANDO"));
		u8g.setFont(u8g_font_6x10);
		u8g.drawStr(0, 25, V.c_str());
		u8g.drawStr(0, 35, firmware.c_str());
	} while (u8g.nextPage());
	delay(2000);

	//Inicia Cartão SD
	inicializaSD();

	// Configura WIFI;
	inicializaESP8266();
	
	// Inicializa o Timer1 a cada 1 segundo;
	Timer1.initialize(1000000);
	Timer1.attachInterrupt(requisicaoAPI);

	//Configura o pino 2(Interrupção 0) para trabalhar como interrupção;
	pinMode(SVazao, INPUT);
	attachInterrupt(0, incpulso, RISING);
}

void loop(void)
{
	float vazao = 0;
	float media = 0;
	byte segundo = 0;
	byte minuto = 0;

	u8g.firstPage();
	do
	{
		draw(vazao);		
	} while (u8g.nextPage());
	vazao = calculaVazao();

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

//Funções
void inicializaSD() {
	if (!SD.begin(chipSelect))
	{
		while (1)
		{
			u8g.firstPage();
			do
			{
				u8g.setFont(u8g_font_8x13B);
				u8g.drawStr(0, 15, F("Erro ao acessar"));
				u8g.drawStr(0, 30, F("CARTAO!"));
			} while (u8g.nextPage());
			delay(800);

			u8g.firstPage();
			do
			{
			} while (u8g.nextPage());
			delay(800);
		}
		return;
	}
	
	u8g.firstPage();
	do
	{
		u8g.setFont(u8g_font_8x13B);
		u8g.drawStr(0, 15, F("CARTAO SD!"));
		u8g.setFont(u8g_font_6x10);
		u8g.drawStr(0, 25, F("Cartao iniciado OK!"));
	} while (u8g.nextPage());
	delay(2000);  
}

void inicializaESP8266() {
	// Seta modo de operação
	setModoOperacao();
	delay(2000);

	// Conecta WIFI
  lerArquivoConfigWifi();	
}

void draw(float vazao)
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
	u8g.drawStr(0, 30, F("R1="));

	u8g.setFont(u8g_font_6x10);
	u8g.drawStr(0, 45, F("16.50%"));

	u8g.drawRFrame(40, 18, 80, 46, 3);
	u8g.drawBox(44, 55, 72, 5);
}

//Interrupções
void requisicaoAPI()
{
	tempoRequisicao++;
	Serial.println(tempoRequisicao);

	if (tempoRequisicao == 600)
	{
		//Implementar metodo para efetuar a requisição a api
		Serial.println(F("********************"));
		Serial.println(F("EFETUADA REQUISICAO!"));
		Serial.println(F("********************")); 
		Serial.println(vazaoTotal);

		tempoRequisicao = 0;
	}
}

void incpulso()
{
	contaPulso++;
}
