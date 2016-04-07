#include "Command.h"

bool Command::isStarted() const {
  return this->started;
}

bool Command::isFinished() const {
  return this->finished;
}

void Command::setStarted() {
  this->started = true;
}

void Command::setFinished() {
  this->finished = true;
}

