#include "BluetoothController.h"

#include "UpdateController.h"
#include "SensorController.h"
#include "LedController.h"

#include <NimBLEDevice.h>
#include <Arduino.h>

// Defines
#define ABS(a) (((a) < 0) ? -(a) : (a))

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
		Serial.println(F("Cliente conectado!"));
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
	NimBLECharacteristic *mCharacteristicP = nullptr;
	BluetoothController *mParent;
	TaskHandle_t taskHandler;

public:
	CharacteristicCallbacks(BluetoothController *parent) : mParent(parent) {}
	~CharacteristicCallbacks() = default;

	/*
	 * Este função é executada no núcleo 1, pois a função WiFiManager::autoConnect
	 * requer que o núcleo 0 permaneça em IDLE durante a chamada. Do contrário o ESP32
	 * encontra um estado cujo comportamento é indefinido e reinicia.
	 */
	static void onWriteImpl(void *_self)
	{
		auto self = (CharacteristicCallbacks *)_self;

		// Esta classe é utlizada apenas pela característica "controle do LED"
		std::string value = self->mCharacteristicP->getValue();

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
				self->mParent->mLedControllerP->set(true);
				break;
			case 'B':
				// Desliga o LED
				self->mParent->mLedControllerP->set(false);
				break;
			case 'U':
				// Habilita o WiFi
				self->mParent->mUpdateControllerP->setup();
				break;
			case 'D':
				// Desabilita o WiFi
				self->mParent->mUpdateControllerP->tearDown();
				break;
			case 'R':
				// Reseta as configurações de WiFi
				self->mParent->mUpdateControllerP->resetSettings();
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

		// A tarefa não pode retornar, ao invés disso ela tem que ser deletada do kernel
		vTaskDelete(nullptr);
	}

	/*
	 * Este função é executada no núcleo 0, porém apenas defere a execução para o núcleo 1
	 */
	void onWrite(NimBLECharacteristic *characteristic) override
	{
		// Salva um ponteiro para a característica neste objeto (sempre será a característica BLE "linha de comando")
		mCharacteristicP = characteristic;

		// Cria uma tarefa que executa a função "onWriteImpl" no núcleo 
		// Cria uma tarefa que executa onWriteImpl
		// create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
		xTaskCreatePinnedToCore(
			onWriteImpl,   // Função de tarefa.
			"onWriteImpl", /* nome da tarefa. */
			10000,		   /* Tamanho da pilha da tarefa */
			(void *)this,  /* parâmetro da tarefa */
			1,			   /* prioridade da tarefa */
			&taskHandler,  /* Identificador de tarefas para acompanhar a tarefa criada */
			1);			   /* fixar tarefa no núcleo 0 */
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
		4);								 // Tamanho máximo do valor: 4 bytes

	// É esperado que o tipo float tenha 4 bytes (32 bits) apenas
	static_assert(sizeof(float) == 4, "");

	// https://www.bluetooth.com/specifications/assigned-numbers/
	// Cria o descritor BLE "dados do sensor" - necessário para o cliente interpretar o valor lido
	NimBLE2904 *sensorBle2904Descriptor = new NimBLE2904();
	sensorBle2904Descriptor->setFormat(BLE2904::FORMAT_FLOAT32); // IEEE-754 32-bit floating point
	sensorBle2904Descriptor->setUnit(uint16_t(0x27AD));			 // GATT Unit: 10157 (percentage)
	mSensorCharP->addDescriptor(sensorBle2904Descriptor);		 // Sem memory leak

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
		// Emite notificação se houver mudança significativa (0.5%)
		float sensorValue = mSensorControllerP->value();
		if (ABS(mSensorValue - sensorValue) > 0.5f)
		{
			Serial.print(F("Notificando mudança! Humidade: "));
			Serial.println(sensorValue);
			mSensorValue = sensorValue;

			// Requer multiplicar por 100 para ser mostrado como porcentagem
			// Requerido por: GATT Unit 0x27AD (percentage)
			sensorValue *= 100;

			// Requer conversão para IEEE-754 32-bit floating point
			// Requerido por: BLE2904::FORMAT_FLOAT32
			struct
			{
				// A ordem é importante
				// Ela é: LSB para MSB.
				uint16_t mantissa : 23;
				uint16_t expoente : 8;
				uint16_t sinal : 1;
			} ieee_754_32_float;

			// A variável ieee_754_32_float ocupa 4 bytes (32 bits)
			static_assert(sizeof(ieee_754_32_float) == 4, "");

			// Inicializa
			ieee_754_32_float.sinal = 0;						// +
			ieee_754_32_float.expoente = 0x40;					// 1
			ieee_754_32_float.mantissa = uint16_t(sensorValue); // Valor inteiro

			// Altera o valor e notifica aos clientes
			mSensorCharP->setValue((uint8_t *)&ieee_754_32_float, 4);
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
