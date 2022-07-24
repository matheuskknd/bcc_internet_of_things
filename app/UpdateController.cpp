#include "UpdateController.h"

#include <ArduinoOTA.h>
#include <WiFiManager.h>

// Defines
#define MAX_ATTEMPT_CONNECTION 10

// Configurações OEM do WIFI e do OTA
static const auto HOSTNAME = PSTR("Esp32HostName");	 // Device recognizable name
static const auto SSID = PSTR("Rede ESP32");		 // SSID (Wi-Fi network name)
static const auto PASSWORD = PSTR("protecao123");	 // Wi-Fi password
static const auto OTAPASSWORD = PSTR("protecaoota"); // OTA password

UpdateController::UpdateController()
{
	mWiFiManagerP = new WiFiManager; // Cria o objeto mWiFiManagerP
	mWiFiClientP = new WiFiClient;	 // Cria o objeto mWiFiClientP

	// Cria o nome do ID único do dispositivo
	// mId = String("ESP ") + WiFi.macAddress();
	mId = "ESP32_IOT";
}

UpdateController::~UpdateController()
{
	delete mWiFiClientP;
	delete mWiFiManagerP;
}

void UpdateController::setup()
{
	// Fixa o host name
	WiFi.hostname(mId);

	// Inicia o WiFiManager
	initWiFi();

	// Inicialização do OTA
	initOTA();

	// Marca como habilitado
	mEnabled = true;
}

void UpdateController::loop()
{
	// Updater está "dormindo"
	if (millis() < mWaitStartTime + mTimeToWait)
	{
		return;
	}

	// Garante que, se houver conexão WiFi, também há o serviço OTA
	if (mEnabled && ensureWiFi())
	{
		ArduinoOTA.handle(); // Keep-alive da comunicação OTA
	}
}

void UpdateController::tearDown()
{
	// Desfaz a inicialização da biblioteca OTA
	ArduinoOTA.end();
	ArduinoOTA.onError(nullptr);
	ArduinoOTA.onProgress(nullptr);
	ArduinoOTA.onEnd(nullptr);
	ArduinoOTA.onStart(nullptr);
	ArduinoOTA.setPassword(nullptr);
	ArduinoOTA.setHostname(nullptr);

	// Desabilita a conexão WiFi
	mWiFiManagerP->disconnect();

	// Marca como desabilitado
	mEnabled = false;
}

void UpdateController::resetSettings()
{
	mWiFiManagerP->resetSettings();
}

String UpdateController::copyId()
{
	return mId;
}

void UpdateController::initWiFi()
{
	// Nome da rede e senha para acessar o servidor de configuração em fallback
	char bufferSsid[32];
	char bufferPassword[32];
	strcpy_P((char *)&bufferSsid, SSID);
	strcpy_P((char *)&bufferPassword, PASSWORD);
	mWiFiManagerP->autoConnect((const char *)&bufferSsid, (const char *)&bufferPassword);

	Serial.print(F("Conectado com sucesso na rede via WifiManager na rede: "));
	Serial.println(WiFi.SSID());
	Serial.println();
	Serial.print(F("IP obtido: "));
	Serial.println(WiFi.localIP()); // Mostra o endereço IP obtido via DHCP
	Serial.println();
	Serial.print(F("Endereço MAC: "));
	Serial.println(WiFi.macAddress()); // Mostra o endereço MAC do ESP32
	Serial.println();
}

void UpdateController::initOTA()
{
	Serial.println(F("Iniciando OTA..."));
	ArduinoOTA.setHostname(mId.c_str()); // Define o nome da porta na IDE

	// Configura uma senha para inserir a atualização via OTA
	char buffer[32];
	strcpy_P((char *)&buffer, OTAPASSWORD);
	ArduinoOTA.setPassword((const char *)&buffer);

	ArduinoOTA.onStart([]()
					   { Serial.println(F("OTA started.")); });

	ArduinoOTA.onEnd([]()
					 { Serial.println(F("\nOTA ended.")); });

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
						  {
		Serial.printf("Progress: %u %%", (progress / (total / 100)));
		Serial.println(); });

	ArduinoOTA.onError([](ota_error_t error)
					   {
		Serial.printf("Error[%u]: ", error);
		switch (error)
		{
		case OTA_AUTH_ERROR:
			Serial.println(F("Auth error"));
			break;
		case OTA_BEGIN_ERROR:
			Serial.println(F("Begin error"));
			break;
		case OTA_CONNECT_ERROR:
			Serial.println(F("Connect error"));
			break;
		case OTA_RECEIVE_ERROR:
			Serial.println(F("Receive error"));
			break;
		case OTA_END_ERROR:
			Serial.println(F("End error"));
			break;
		default:
			break;
		} });

	// Starts listening
	ArduinoOTA.begin();
}

bool UpdateController::ensureWiFi()
{
	// Se já está conectado a rede WI-FI, nada é feito.
	auto status = WiFi.status();
	if (status == WL_CONNECTED)
	{
		if (mRetryCounter != 0)
		{
			Serial.println();
			Serial.print(F("Conectado com sucesso na rede: "));
			Serial.println(WiFi.SSID());
			Serial.println();
			Serial.print(F("IP obtido: "));
			Serial.print(WiFi.localIP()); // Mostra o endereço IP obtido via DHCP
			Serial.println();
			Serial.print(F("Endereço MAC: "));
			Serial.print(WiFi.macAddress()); // Mostra o endereço MAC do ESP32
			mRetryCounter = 0;
		}

		return true;
	}

	// Caso contrário, são efetuadas tentativas de reconexão MAX_ATTEMPT_CONNECTION vezes
	if (mRetryCounter == 0)
	{
		mRetryCounter = MAX_ATTEMPT_CONNECTION + 1;
		Serial.println(F("Tentando reconexão WiFi..."));
	}
	else if (mRetryCounter == 1)
	{
		// Caso nenhuma tentativa funcione, o WiFiManager é utilizado como fallback
		mRetryCounter = 0;
		initWiFi();
	}
	else
	{
		// Senão apenas decrementa o contador de retentativas
		--mRetryCounter;
	}

	// Dá tempo para Wi-Fi se reconectar
	mWaitStartTime = millis();
	mTimeToWait = 3000;
	return false;
}
