#include "MotorCommand.h"

MotorCommand::MotorCommand(
  Motor * const motor
) : 
  motor(motor)
{}

void MotorCommand::turnOn() {
  this->motor->turnOn();
  this->command = TURNON;
}

void MotorCommand::turnOff() {
  this->motor->turnOff();
  this->command = TURNOFF;
}

void MotorCommand::moveTo(int targetDistance) {
  this->targetDistance = constrain(targetDistance, 0, this->motor->getMaxDistance());
  this->command = MOVE;
}

void MotorCommand::setDirection(int direction) {
  this->motor->setDirection(direction);
}

void MotorCommand::setSpeed(int speed) {
  this->motor->setSpeed(speed);
}

int  MotorCommand::getDirection() const {
  return this->motor->getDirection();
}

int  MotorCommand::getSpeed() const {
  return this->motor->getSpeed();
}

void MotorCommand::reverseDirection() {
  bool wasRunning = this->motor->isRunning();

  if (wasRunning == true) {
    this->motor->turnOff();
  }
  
  if (this->motor->getDirection() == FORWARD) {
    this->motor->setDirection(BACKWARD);
  } else {
    this->motor->setDirection(FORWARD);
  }

  if (wasRunning == true) {
    this->motor->turnOn();
  }
}

void MotorCommand::startCommand() {
  if (this->command == MOVE) {
    // determine if motor should already be at that position from a previous command
    if (this->motor->getLastPosition() != this->targetDistance) {
      // determine motor direction
      int position = this->motor->getSensor()->getDistance();
      if (position >= this->targetDistance) {
        this->motor->setDirection(BACKWARD);
      } else {
        this->motor->setDirection(FORWARD);
      }
      this->motor->turnOn();
      this->setStarted();
    } else {
      this->setFinished();
      Serial.println(this->motor->getName() + " ignored target distance: " + String(this->targetDistance));
    }
  } else if (this->command == TURNON) {
    this->motor->turnOn();
  } else if (this->command == TURNOFF) {
    this->motor->turnOff();
  } else {
    Serial.println("No command for MotorCommand selected");
  }
}

void MotorCommand::loop() {
  if (this->command == MOVE) {
    int position = this->motor->getSensor()->getDistance();
    if (this->motor->getDirection() == FORWARD && position >= this->targetDistance) {
      this->setFinished();
      this->motor->turnOff();
      this->motor->setLastPosition(position);
      Serial.println("################################################################" + this->motor->getName() + " off at read position: " + String(position));
    } else if (this->motor->getDirection() == BACKWARD && position <= this->targetDistance) {
      Serial.println("################################################################" + this->motor->getName() + " off at read position: " + String(position));
      this->setFinished();
      this->motor->turnOff();
      this->motor->setLastPosition(position);
    }
  } else if (this->command == TURNON || this->command == TURNOFF || this->command == REVERSE) {
    this->setFinished(); // instantaneous
  }
}

void MotorCommand::endCommand() {
  this->motor->turnOff();
}

String MotorCommand::toString() const {
  if (this->command == MOVE) {
    String dir = "forward";
    if (this->motor->getDirection() != FORWARD) {
      dir = "backward";
    }
    int pos = this->motor->getSensor()->getDistance();
    String name = this->motor->getName();
    return name + " running " + dir + " to position " + targetDistance + " mm, now at " + pos + " mm.";
  } else if (this->command == TURNON) {
    return "Switch motor on.";
  } else if (this->command == TURNOFF) {
    return "Switch motor off.";
  } else if (this->command == REVERSE) {
    return "Reverse motor direction";
  } else {
    return "Empty MotorCommand";
  }
}
