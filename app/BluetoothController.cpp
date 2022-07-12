#include "BluetoothController.h"

#include "UpdateController.h"
#include "SensorController.h"
#include "LedController.h"

#include <NimBLEDevice.h>

/*
 * Classe responsável por lidar com eventos no servidor
 */
class ServerCallbacks : public NimBLEServerCallbacks
{
	BluetoothController *mParent;

public:
	ServerCallbacks(BluetoothController *parent) : mParent(parent) {}
	~ServerCallbacks() = default;

	void onConnect(NimBLEServer *server) override
	{
		BLEAdvertising *advertising = mParent->mServerPImpl->getAdvertising();
		mParent->mDeviceConnected = true;
		advertising->stop();
	}

	void onDisconnect(NimBLEServer *server) override
	{
		mParent->mDeviceConnected = false;
	}
};

/*
 * Classe responsável por lidar com eventos de escrita na característica
 */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
	BluetoothController *mParent;

public:
	CharacteristicCallbacks(BluetoothController *parent) : mParent(parent) {}
	~CharacteristicCallbacks() = default;

	void onWrite(NimBLECharacteristic *characteristic) override
	{
		// Esta classe é utlizada apenas pela característica "controle do LED"
		std::string value = characteristic->getValue();

		if (value.length() > 0)
		{
			Serial.println(F("*********"));
			Serial.print(F("New value: "));
			for (int i = 0; i != value.length(); ++i)
				Serial.print(value[i]);

			Serial.println();
			Serial.println(F("*********"));
		}

		if (value.length() == 1)
		{
			switch (value[0])
			{
			case 'A':
				// Liga o LED
				mParent->mLedControllerPImpl->set(true);
				break;
			case 'B':
				// Desliga o LED
				mParent->mLedControllerPImpl->set(false);
				break;
			case 'U':
				// Habilita o WiFi
				mParent->mUpdateControllerPImpl->setup();
				break;
			case 'D':
				// Desabilita o WiFi
				mParent->mUpdateControllerPImpl->tearDown();
				break;
			case 'R':
				// Reseta as configurações de WiFi
				mParent->mUpdateControllerPImpl->resetSettings();
				break;
			default:
				Serial.println(F("Opção inválida"));
				break;
			}
		}
		else
		{
			Serial.println(F("Comando inválido recebido"));
		}
	}
};

/*
 * Implementação das funções membro
 */

BluetoothController::BluetoothController()
{
	mUpdateControllerPImpl = new UpdateController;
	mSensorControllerPImpl = new SensorController;
	mLedControllerPImpl = new LedController;
	mId = mUpdateControllerPImpl->copyId();
}

BluetoothController::~BluetoothController()
{
	delete mUpdateControllerPImpl;
	delete mSensorControllerPImpl;
	delete mLedControllerPImpl;
}

void BluetoothController::setup()
{
	mSensorControllerPImpl->setup();
	mLedControllerPImpl->setup();

	/*
	 * Código criado a partir dos exemplos BLE_notify.ino e BLE_write.ino
	 * De ESP32 Dev Module/ESP32 BLE Arduino
	 */

	Serial.println(F("1- Baixe e instale um app de scanner BLE no seu celular"));
	Serial.println(F("2- Busque por dispositivos BLE no app"));
	Serial.println(F("3- Conecte ao ESP32"));
	Serial.println(F("4- Navegue até CUSTOM CHARACTERISTIC em CUSTOM SERVICE e escreva algo"));
	Serial.println(F("5- Pronto! =)"));

	// Cria o dispositivo BLE
	BLEDevice::init(mId.c_str());

	// Cria o servidor BLE
	mServerPImpl = NimBLEDevice::createServer();
	mServerPImpl->setCallbacks(new ServerCallbacks(this));
	mServerPImpl->advertiseOnDisconnect(true);

	// Cria o serviço BLE
	char buffer[64];
	strcpy_P((char *)&buffer, SERVICE_UUID);
	BLEService *service = mServerPImpl->createService((const char *)&buffer);

	// Cria a característica BLE "dados do sensor"
	strcpy_P((char *)&buffer, SENSOR_CHAR_UUID);
	mSensorCharPImpl = service->createCharacteristic(
		(const char *)&buffer,
		NIMBLE_PROPERTY::READ |
			NIMBLE_PROPERTY::WRITE |
			NIMBLE_PROPERTY::NOTIFY |
			NIMBLE_PROPERTY::INDICATE,
		32);

	// https://www.bluetooth.com/specifications/specs/gatt-specification-supplement-6/
	// Cria o descritor BLE "dados do sensor"
	mSensorCharPImpl->addDescriptor(new NimBLE2904());

	// Cria a característica BLE "controle do LED"
	strcpy_P((char *)&buffer, LED_CHAR_UUID);
	mLedCharPImpl = service->createCharacteristic(
		(const char *)&buffer,
		NIMBLE_PROPERTY::READ |
			NIMBLE_PROPERTY::WRITE,
		32);

	mLedCharPImpl->setCallbacks(new CharacteristicCallbacks(this));
	mLedCharPImpl->setValue(std::string());

	// Inicia o serviço
	service->start();

	// Inicia a propagar o serviço
	strcpy_P((char *)&buffer, SERVICE_UUID);
	BLEAdvertising *advertising = mServerPImpl->getAdvertising();
	advertising->addServiceUUID((const char *)&buffer);
	advertising->setScanResponse(true);
	advertising->setMinPreferred(0x20); // Coloque 0x00 para não propagar este parâmetro
	advertising->start();
	Serial.println(F("Aguardando um cliente para notificar..."));
}

void BluetoothController::loop()
{
	mUpdateControllerPImpl->loop();
	mSensorControllerPImpl->loop();
	mLedControllerPImpl->loop();

	/*
	 * Código criado a partir dos exemplos BLE_notify.ino e BLE_write.ino
	 * De ESP32 Dev Module/ESP32 BLE Arduino
	 */

	// Bluetooth está "dormindo"
	if (millis() < mWaitStartTime + mTimeToWait)
	{
		return;
	}

	// Notifica que o valor foi alterado
	if (mDeviceConnected)
	{
		const uint16_t sensorValue = mSensorControllerPImpl->value();
		if (sensorValue != mSensorValue)
		{
			mSensorValue = sensorValue;
			mSensorCharPImpl->setValue(mSensorValue);
			mSensorCharPImpl->notify();
		}

		// A stack do bluetooth fica cogestionada se muitos pacotes forem enviados
		// Em 6 horas de testes foi constatado 3 ms de atraso, certa vez
		mWaitStartTime = millis();
		mTimeToWait = 3;
	}
	// Desconectando
	if (!mDeviceConnected && mOldDeviceConnected)
	{
		mWaitStartTime = millis();
		mTimeToWait = 500;				  // Dá tempo à stack bluetooth para terminar de processar
		mServerPImpl->startAdvertising(); // Recomeça a propaganda
		Serial.println(F("Aguardando um cliente para notificar..."));
		mOldDeviceConnected = mDeviceConnected;
	}
	// Conectando
	if (mDeviceConnected && !mOldDeviceConnected)
	{
		// Código executado sempre que um novo dispositivo se conecta
		mOldDeviceConnected = mDeviceConnected;
	}
}
