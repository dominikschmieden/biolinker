#include "PauseCommand.h"

PauseCommand::PauseCommand(unsigned long duration) : duration(duration) {};

void PauseCommand::startCommand() {
  this->timer = millis();
  this->setStarted();
}

String PauseCommand::toString() const {
  return String("Pausing, duration: ") + this->duration + String(" ms");
}

void PauseCommand::loop() {
  if ((millis() - this->timer) > this->duration) {
    this->setFinished();
  }
}

void PauseCommand::endCommand() {
  // nothing
}

