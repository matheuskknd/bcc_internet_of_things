#pragma once

#include <Arduino.h>
#include <cstdint>

class UpdateController;
class SensorController;
class LedController;
class NimBLECharacteristic;
class NimBLEServer;

class BluetoothController
{
private:
	UpdateController *mUpdateControllerPImpl = nullptr;
	SensorController *mSensorControllerPImpl = nullptr;
	LedController *mLedControllerPImpl = nullptr;
	NimBLECharacteristic *mSensorCharPImpl = nullptr;
	NimBLECharacteristic *mLedCharPImpl = nullptr;
	NimBLEServer *mServerPImpl = nullptr;
	bool mOldDeviceConnected = false;
	bool mDeviceConnected = false;
	uint16_t mSensorValue = 0;
	String mId = F("");

	// Variáveis especiais de espera não bloqueante
	uint32_t mWaitStartTime = 0;
	uint32_t mTimeToWait = 0;

	// See the following for generating UUIDs:
	// https://www.uuidgenerator.net/

	/*
	 * UUID do serviço prestado por esse dispositivo (todas as funcionalidades incluídas)
	 */
	constexpr static auto SERVICE_UUID = PSTR("8bfb95e0-412a-4a9a-985b-7e291c51f459");

	/*
	 * UUID da característica "dados do sensor"
	 */
	constexpr static auto SENSOR_CHAR_UUID = PSTR("0eab338e-bb33-4e2b-a220-aae9b0528995");

	/*
	 * UUID da característica "controle do LED"
	 */
	constexpr static auto LED_CHAR_UUID = PSTR("65842cd5-b667-4cc0-a54b-1f53eab7e0ca");

public:
	BluetoothController();
	~BluetoothController();

	/*
	 * Função que deve ser chamada apenas uma vez na inicialização do objeto
	 */
	void setup();

	/*
	 * Função que deve ser chamada em todo loop após setup ter sido chamado
	 */
	void loop();

protected:
	friend class CharacteristicCallbacks;
	friend class ServerCallbacks;
};