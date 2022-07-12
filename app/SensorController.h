#pragma once

#include <cstdint>

class SensorController
{
	uint16_t mValue = 0;

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
	uint16_t value();
};
