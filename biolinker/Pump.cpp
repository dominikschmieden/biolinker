#include "Pump.h"

Pump::Pump(const int deviceAddress, int defaultDirection, int defaultSpeed): deviceAddress(deviceAddress), defaultDirection(defaultDirection), defaultSpeed(defaultSpeed) {
}

int Pump::getDeviceAddress() const {
  return this->deviceAddress;
}

void Pump::setDefaultSpeed(int speed) {
  if (speed > 255) {
    speed = 255;
  }
  this->defaultSpeed = speed;
}

int Pump::getDefaultSpeed() {
  return this->defaultSpeed;
}

void Pump::setDefaultDirection(int direction) {
  if (direction > 0) {
    direction = 1;
  }
  this->defaultDirection = direction;
}

int Pump::getDefaultDirection() {
  return this->defaultDirection;
}

