#pragma once

#include <Arduino.h>
#include <cstdint>

class WiFiManager;
class WiFiClient;

class UpdateController
{
private:
	WiFiManager *mWiFiManagerP = nullptr;
	WiFiClient *mWiFiClientP = nullptr;
	uint32_t mRetryCounter = 0;
	bool mEnabled = false;
	String mId;

	// Variáveis especiais de espera não bloqueante
	uint32_t mWaitStartTime = 0;
	uint32_t mTimeToWait = 0;

public:
	UpdateController();
	~UpdateController();

	/*
	 * Função que deve ser chamada apenas uma vez na inicialização do objeto
	 */
	void setup();

	/*
	 * Função que deve ser chamada em todo loop após setup ter sido chamado
	 */
	void loop();

	/*
	 * Função que deve ser chamada quando este objeto não for mais necessário no setup
	 */
	void tearDown();

	/*
	 * Função que apaga os SSIDs conhecidos e suas respectivas senhas da memória persistente
	 */
	void resetSettings();

	/*
	 * Função que cria uma cópia da string ID e retorna-a para o chamador
	 */
	String copyId();

protected:
	/*
	 * Função que se conecta a uma rede já conhecida ou faz fallback para página web de configuração caso não encontre uma conhecida (bloqueante)
	 */
	void initWiFi();

	/*
	 * Função de inicialização do OTA e permite a carga de um novo firmware via WiFi mediante autenticação (bloqueante)
	 */
	void initOTA();

	/*
	 * Função que garante que há conexão WiFi. Caso não esteja conectado, aguarda 60 segundos antes de abrir chamar "initwiFi"
	 *
	 * Retorna se o WiFi está conectado ou não.
	 */
	bool ensureWiFi();
};
