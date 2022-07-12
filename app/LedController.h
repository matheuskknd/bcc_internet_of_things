#pragma once

class LedController
{
public:
	LedController();
	~LedController();

	/*
	 * Função que deve ser chamada apenas uma vez na inicialização do objeto
	 */
	void setup();

	/*
	 * Função que deve ser chamada em todo loop após setup ter sido chamado
	 */
	void loop();

	/*
	 * Função que define se o LED deve estar ligado ou desligado
	 */
	void set(bool turnedOn);

protected:
};
