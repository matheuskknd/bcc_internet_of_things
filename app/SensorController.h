#pragma once

#include <cstdint>

class DHT;

class SensorController
{
private:
	DHT *mDHTP = nullptr;
	float mHumidity = 0.0f;

	// Variáveis especiais de espera não bloqueante
	uint32_t mWaitStartTime = 0;
	uint32_t mTimeToWait = 0;

public:
	SensorController();
	~SensorController();

	/*
	 * Função que deve ser chamada apenas uma vez na inicialização do objeto
	 */
	void setup();

	/*
	 * Função que deve ser chamada em todo loop após setup ter sido chamado
	 */
	void loop();

	/*
	 * Função que retorna o valor atual coletado diretamente do sensor
	 */
	float value();
};
