#include "LedController.h"
#include <Arduino.h>

// defines - mapeamento de pinos do NodeMCU
#define D5 5 // Outputs PWM signal at boot

LedController::LedController()
{
    /* Criação da variavel de liga e desliga */
    led = D5;
}

LedController::~LedController()
{
}

void LedController::setup()
{
    // enviar HIGH para o output faz o Led acender / enviar LOW faz o Led apagar)
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
}

void LedController::loop()
{
}

void LedController::set(bool turnedOn)
{
    /* Funcao que chama o led para ligar ou desligar */
    if (turnedOn)
        digitalWrite(led, HIGH);
    else
        digitalWrite(led, LOW);
}
