#include "BluetoothController.h"

#include "UpdateController.h"
#include "SensorController.h"
#include "LedController.h"

#include <NimBLEDevice.h>
#include <Arduino.h>

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
		BLEAdvertising *advertising = mParent->mServerP->getAdvertising();
		mParent->mDeviceConnected = true;
		advertising->stop();
	}

	void onDisconnect(NimBLEServer *server) override
	{
		mParent->mDeviceConnected = false;
	}
};

/*
 * Classe responsável por lidar com eventos de escrita na característica BLE "linha de comando"
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
				mParent->mLedControllerP->set(true);
				break;
			case 'B':
				// Desliga o LED
				mParent->mLedControllerP->set(false);
				break;
			case 'U':
				// Habilita o WiFi
				mParent->mUpdateControllerP->setup();
				break;
			case 'D':
				// Desabilita o WiFi
				mParent->mUpdateControllerP->tearDown();
				break;
			case 'R':
				// Reseta as configurações de WiFi
				mParent->mUpdateControllerP->resetSettings();
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
	mUpdateControllerP = new UpdateController; // Cria o objeto mUpdateControllerP
	mSensorControllerP = new SensorController; // Cria o objeto mSensorControllerP
	mLedControllerP = new LedController;	   // Cria o objeto mLedControllerP
	mId = mUpdateControllerP->copyId();		   // Copia o identificador único

	// Cria os objetos de callbacks
	mCommandCharCallbacksP = new CharacteristicCallbacks(this);
	mServerCallbacksP = new ServerCallbacks(this);
}

BluetoothController::~BluetoothController()
{
	delete mUpdateControllerP;
	delete mSensorControllerP;
	delete mLedControllerP;
	delete mCommandCharCallbacksP;
	delete mServerCallbacksP;
}

void BluetoothController::setup()
{
	mSensorControllerP->setup();
	mLedControllerP->setup();

	/*
	 * Código criado a partir dos exemplos BLE_notify.ino e BLE_write.ino
	 * De ESP32 Dev Module/ESP32 BLE Arduino
	 */

	Serial.println(F("1- Baixe e instale um app de scanner BLE no seu celular"));
	Serial.println(F("2- Busque por dispositivos BLE no app"));
	Serial.println(F("3- Conecte ao ESP32"));
	Serial.println(F("4- Navegue até CUSTOM SERVICE e interaja com as características"));
	Serial.println(F("5- Pronto! =)"));

	// Cria o dispositivo BLE
	BLEDevice::init(mId.c_str());

	// Cria o servidor BLE
	mServerP = NimBLEDevice::createServer();
	mServerP->setCallbacks(mServerCallbacksP);
	mServerP->advertiseOnDisconnect(false);

	// Cria o serviço BLE
	char buffer[64];
	strcpy_P((char *)&buffer, SERVICE_UUID);
	BLEService *service = mServerP->createService((const char *)&buffer);

	// https://www.bluetooth.com/specifications/assigned-numbers/
	// Cria a característica BLE "sensor de humidade"
	mSensorCharP = service->createCharacteristic(
		NimBLEUUID(uint16_t(0x2A6F)),	 // UUID número: 10863 (Humidity)
		NIMBLE_PROPERTY::READ			 // READ permite leituras esporádicas
			| NIMBLE_PROPERTY::NOTIFY	 // NOTIFY não requer ACK
		/*| NIMBLE_PROPERTY::INDICATE*/, // INDICATE requer ACK
		2);								 // Tamanho máximo do valor: 2 bytes

	// https://www.bluetooth.com/specifications/assigned-numbers/
	// Cria o descritor BLE "dados do sensor" - necessário para o cliente interpretar o valor lido
	NimBLE2904 *sensorBle2904Descriptor = new NimBLE2904();
	sensorBle2904Descriptor->setFormat(BLE2904::FORMAT_UINT16);
	//sensorBle2904Descriptor->setUnit(uint16_t(0x27AD)); // GATT Unit: 10157 (percentage)
	mSensorCharP->addDescriptor(sensorBle2904Descriptor); // Sem memory leak

	// https://www.bluetooth.com/specifications/assigned-numbers/
	// Cria a característica BLE "linha de comando"
	mCommandCharP = service->createCharacteristic(
		NimBLEUUID(uint16_t(0x2B26)),	 // UUID número: 11046 (IDD Command Data)
		NIMBLE_PROPERTY::WRITE			 // WRITE permite escrita (requer ACK)
		/*| NIMBLE_PROPERTY::WRITE_NR*/, // WRITE_NR permite escrita (não requer ACK)
		1);								 // Tamanho máximo do valor: 1 byte

	// Define as "callbacks" a serem chamadas quando houver escritas
	mCommandCharP->setCallbacks(mCommandCharCallbacksP);

	// Inicia o serviço
	service->start();

	// Inicia a propagar o serviço
	BLEAdvertising *advertising = mServerP->getAdvertising();
	advertising->addServiceUUID(service->getUUID());

	/*
	 * Se o dispositivo for energizado por bateria, vale a pena configurar
	 * a resposta de scanner para "false". Isso deve extender a vida da bateria
	 * ao custo de enviar menos dados quando escaneado.
	 */
	advertising->setScanResponse(true);
	advertising->setMinPreferred(0x20); // Coloque 0x00 para não propagar os parâmetros
	advertising->start();

	Serial.println(F("Aguardando um cliente se conectar..."));
}

void BluetoothController::loop()
{
	mUpdateControllerP->loop();
	mSensorControllerP->loop();
	mLedControllerP->loop();

	// Bluetooth está "dormindo"
	if (millis() < mWaitStartTime + mTimeToWait)
	{
		return;
	}

	/*
	 * Código criado a partir dos exemplos BLE_notify.ino e BLE_write.ino
	 * De ESP32 Dev Module/ESP32 BLE Arduino
	 */

	// Notifica que o valor foi alterado
	if (mDeviceConnected)
	{
		const uint16_t sensorValue = mSensorControllerP->value();
		if (sensorValue != mSensorValue)
		{
			mSensorValue = sensorValue;
			mSensorCharP->setValue(sensorValue);
			mSensorCharP->notify();
		}

		// A stack do bluetooth fica cogestionada se muitos pacotes forem enviados
		// Em 6 horas de testes foi constatado 3 ms de atraso, certa vez
		mWaitStartTime = millis();
		mTimeToWait = 3;
	}
	else // Desconectando
	{
		if (mOldDeviceConnected)
		{
			mWaitStartTime = millis();
			mTimeToWait = 500; // Dá tempo à stack bluetooth para terminar de processar
			mOldDeviceConnected = false;
		}
		else if (!mServerP->getAdvertising()->isAdvertising())
		{
			mServerP->startAdvertising(); // Recomeça a propaganda
			Serial.println(F("Aguardando um cliente se conectar..."));
		}
	}
	// Conectando
	if (mDeviceConnected && !mOldDeviceConnected)
	{
		// Código executado sempre que um novo dispositivo se conecta
		mOldDeviceConnected = true;
	}
}
