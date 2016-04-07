#include "Motor.h"

Motor::Motor(
  const String name,
  const int defaultSpeed, 
  const int maxDistance,
  const int port, 
  DistanceSensor * const sensor
) :
  lastPosition(-100),
  name(name),
  defaultSpeed(defaultSpeed),
  speed(defaultSpeed),
  maxDistance(maxDistance),
  port(port),
  sensor(sensor),
  motor(AF_DCMotor(port, MOTOR12_64KHZ))
{
  motor.setSpeed(defaultSpeed);
  this->direction = FORWARD;
}

void Motor::setSpeed(int speed) {
  this->speed = speed;
  this->motor.setSpeed(speed);
}

int Motor::getSpeed() const {
  return this->speed;
}

void Motor::setDirection(int direction) {
  this->direction = direction;
}

int Motor::getDirection() const {
  return this->direction;
}

int Motor::getMaxDistance() const {
  return this->maxDistance;
}

void Motor::setMaxDistance(int distance) {
  this->maxDistance = distance;
}

void Motor::turnOn() {
  this->motor.run(this->direction);
  this->running = true;
}

void Motor::turnOff() {
  this->motor.run(RELEASE);
  this->running = false;
}

DistanceSensor * const Motor::getSensor() const {
  return this->sensor;
}

bool Motor::isRunning() const {
  return this->running;
}

String Motor::getName() const {
  return this->name;
}

int Motor::getLastPosition() const {
  return this->lastPosition;
}

void Motor::setLastPosition(int position) {
  this->lastPosition = position;
}

void Motor::setDefaultSpeed(int speed) {
  this->defaultSpeed = speed;
}

int Motor::getDefaultSpeed() {
  return this->defaultSpeed;
}
