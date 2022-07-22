#include "SensorController.h"

#include "DHT.h"

/*
 * Código criado a partir do exemplo DHTtester.ino
 * Da Biblioteca Personalizadas/DHT Sensor Library
 */

// defines - mapeamento de pinos do NodeMCU
#define D4 4

// Descomente de acordo com o tipo de Sensor DHT utilizado!
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

SensorController::SensorController()
{
  mDHTP = new DHT(D4, DHTTYPE);
}

SensorController::~SensorController()
{
  delete mDHTP;
}

void SensorController::setup()
{
  mDHTP->begin();
}

void SensorController::loop()
{
  // Sensor está "dormindo"
  if (millis() < mWaitStartTime + mTimeToWait)
  {
    return;
  }

  // Programa-se para dormir por 2 segundos após a leitura
  mWaitStartTime = millis();
  mTimeToWait = 2000;

  // Ler temperatura e humidade leva cerca de 250 ms!
  // As leituras podem estar ainda desatualizadas em 2 segundos...
  float humidity = mDHTP->readHumidity();

  // Checa se falhou ao ler o valor do sensor e retorna (para tentar novamente)
  if (isnan(humidity))
  {
    Serial.println(F("Falha ao ler dados do sensor!"));
  }
  else
  {
    // Salva o valor lido com sucesso mais recente
    mHumidity = humidity;

    Serial.print(F("Humidade: "));
    Serial.println(mHumidity);
  }
}

float SensorController::value()
{
  return mHumidity;
}
