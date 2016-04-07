#include "PumpCommand.h"

PumpCommand::PumpCommand(Pump * const pump) : pump(pump) {
  this->speed = this->pump->getDefaultSpeed();
  this->counterClockwise = this->pump->getDefaultDirection();
}

PumpCommand::PumpCommand(Pump * const pump, bool counterClockwise, int speed) : pump(pump), counterClockwise(counterClockwise), speed(speed) {
}

void PumpCommand::setCounterClockwise(bool counterClockwise) {
  this->counterClockwise = counterClockwise;
}

bool PumpCommand::getCounterClockwise() const {
  return this->counterClockwise;
}

void PumpCommand::setSpeed(int speed) {
  this->speed = speed;
}

int PumpCommand::getSpeed() const {
  return this->speed;
}

Pump * const PumpCommand::getPump() const {
  return this->pump;
}

void PumpCommand::startCommand() {
  this->setStarted();
  Wire.beginTransmission(this->pump->getDeviceAddress());
  String command = String(this->counterClockwise);
  command += " ";
  command += this->speed;
  command += ";";
  Wire.write(command.c_str());
  Wire.endTransmission();
  this->setFinished();
}

String PumpCommand::toString() const {
  return String("Pumping, Direction: ") + this->counterClockwise + String(" Speed: ") + this->speed;
}

void PumpCommand::loop() {
  // nothing, pump commands are instantaneous
}

void PumpCommand::endCommand() {
  // nothing, pump commands are instantaneous
  // to stop a pump, it has to be set to speed 0 in another command
}

