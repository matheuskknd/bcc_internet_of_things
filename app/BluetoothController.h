#pragma once

#include <Arduino.h>
#include <cstdint>

class UpdateController;
class SensorController;
class LedController;
class NimBLECharacteristic;
class NimBLEServer;
class CharacteristicCallbacks;
class ServerCallbacks;

class BluetoothController
{
private:
	UpdateController *mUpdateControllerP = nullptr;
	SensorController *mSensorControllerP = nullptr;
	LedController *mLedControllerP = nullptr;
	NimBLECharacteristic *mSensorCharP = nullptr;
	NimBLECharacteristic *mCommandCharP = nullptr;
	NimBLEServer *mServerP = nullptr;
	CharacteristicCallbacks *mCommandCharCallbacksP = nullptr;
	ServerCallbacks *mServerCallbacksP = nullptr;
	bool mOldDeviceConnected = false;
	bool mDeviceConnected = false;
	uint16_t mSensorValue = 0;
	String mId;

	// Variáveis especiais de espera não bloqueante
	uint32_t mWaitStartTime = 0;
	uint32_t mTimeToWait = 0;

	// See the following for generating UUIDs:
	// https://www.uuidgenerator.net/

	/*
	 * UUID do serviço prestado pelo dispositivo (todas as funcionalidades incluídas)
	 */
	constexpr static auto SERVICE_UUID = PSTR("8bfb95e0-412a-4a9a-985b-7e291c51f459");

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
