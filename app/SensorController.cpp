#include "SensorController.h"

SensorController::SensorController()
{
}

SensorController::~SensorController()
{
}

void SensorController::setup()
{
}

void SensorController::loop()
{
    ++mValue;
}

uint16_t SensorController::value()
{
    return mValue;
}
