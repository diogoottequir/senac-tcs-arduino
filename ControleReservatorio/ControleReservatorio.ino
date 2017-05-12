/*
==================================================
Programa : Controle de Reservatorio
Autor : Diogo Ottequir
Data : 10/09/2016
===================================================
*/

/*Bibliotecas*/
#include <ArduinoJson.h>
#include <ESP8266.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <U8glib.h>
#include <SD.h>

/*Declarações*/
ESP8266 wifi(Serial1);
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

#define Versao "A.01" 
#define S1 22 
#define S2 24
#define S3 26
#define S4 28
#define S5 30
#define S6 32
#define SVazao 2
#define chipSelect 4
#define HOST_NAME   ""
#define HOST_PORT   (8080)


String SSID = "";
String PASSWORD = "";

byte tempoRequisicao = 0;
byte segundo = 0;
byte minuto = 0;
float vazao = 0;
float consumo = 0;
float vazaoTotal = 0;
float mediaTotal = 0;
int contaPulso = 0;

/*
===================================================
Rotinas
===================================================
*/
void setup()
{
	String V = F("-Versao ");
	V += Versao;
	mensagem(F("INICIANDO!"), V, true);
	delay(2000);

	pinMode(S1, INPUT);
	pinMode(S2, INPUT);
	pinMode(S3, INPUT);
	pinMode(S4, INPUT);
	pinMode(S5, INPUT);
	pinMode(S6, INPUT);
	pinMode(SVazao, INPUT);
	
	Serial.begin(9600);
	u8g.begin();
		
	inicializaSD();
	conectaWifi();
	
	Timer1.initialize(1000000);
	Timer1.attachInterrupt(requisicaoAPI);
	attachInterrupt(0, incpulso, RISING);
}

void loop(void)
{	
	u8g.firstPage();
	do
	{
		draw(vazao);		
	} while (u8g.nextPage());
	
	calculaVazao();
}

void draw(float vazao)
{
	String vz = F(" = ");
		   vz += vazao;
           vz += F(" L/s");	
	mostraConsumo(vz);
	mostraNivel();
}

void mostraConsumo(String vz)
{
	u8g.drawBox(0, 0, 12, 2);
	u8g.drawBox(0, 5, 16, 4);
	u8g.drawBox(13, 6, 4, 4);
	u8g.drawBox(14, 7, 4, 4);
	u8g.drawBox(15, 8, 4, 3);
	u8g.drawBox(15, 10, 4, 3);
	u8g.drawVLine(5, 0, 9);

	u8g.setFont(u8g_font_8x13B);
	u8g.drawStr(20, 15, vz.c_str());
}

void mostraNivel() 
{
	float nivel = 0;

	u8g.setFont(u8g_font_fub11r);
	u8g.drawStr(0, 30, F("R1="));
	u8g.drawRFrame(40, 18, 80, 46, 3);

	if (digitalRead(S1) == 1)
	{
		u8g.drawBox(44, 55, 72, 5);
		nivel = 16.67;
	}
	if (digitalRead(S2) == 1)
	{
		u8g.drawBox(44, 49, 72, 5);
		nivel = nivel + 16.67;
	}
	if (digitalRead(S3) == 1)
	{
		u8g.drawBox(44, 43, 72, 5);
		nivel = nivel + 16.67;
	}
	if (digitalRead(S4) == 1)
	{
		u8g.drawBox(44, 37, 72, 5);
		nivel = nivel + 16.67;
	}
	if (digitalRead(S5) == 1)
	{
		u8g.drawBox(44, 25, 72, 5);
		nivel = nivel + 16.67;
	}
	if (digitalRead(S6) == 1)
	{
		u8g.drawBox(44, 25, 72, 5);
		nivel = nivel + 16.67;
	}

	//PERCENTUAL
	String n = F("");
	       n += nivel;
	       n += F("%");

	u8g.setFont(u8g_font_6x10r);
	u8g.drawStr(0, 45, n.c_str());
}

/*
Interrupções
*/
void requisicaoAPI()
{
	tempoRequisicao++;
	if (tempoRequisicao == 600)
	{
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
