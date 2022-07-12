#include "BluetoothController.h"

/*
 * Variáveis globais
 */
BluetoothController gBluetoothController;

/*
 * Função setup global
 */
void setup()
{
	// Inicialização da saída serial ("stdout")
	Serial.begin(115200);

	// Chama setup em cada objeto global
	gBluetoothController.setup();

	// Fim da inicialização
	Serial.println("Programa iniciado com sucesso!");
}

/*
 * Função loop global
 */
void loop()
{
	// Chama loop em cada objeto global
	gBluetoothController.loop();
}
