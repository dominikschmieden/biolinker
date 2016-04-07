#ifndef Motor_H
#define Motor_H
 
#include <Arduino.h>
#include <AFMotor.h>
#include "config.cpp"
#include "DistanceSensor.h"
 
class Motor {
  private:
    int lastPosition;
    int speed;
    int direction;
    const int port;
    int defaultSpeed = 255;
    int maxDistance;
    bool running = false;
    DistanceSensor* sensor;
    AF_DCMotor motor;
    String name;
  public:
    Motor(const String name, const int defaultSpeed, const int maxDistance, const int port, DistanceSensor * const sensor);
    void setSpeed(int speed);
    int  getSpeed() const;
    void setDirection(int direction);
    int getDirection() const;
    int getMaxDistance() const;
    void setMaxDistance(int distance);
    int getLastPosition() const;
    void setLastPosition(int position);
    void turnOn();
    void turnOff();
    bool isRunning() const;
    String getName() const;
    DistanceSensor * const getSensor() const;
    void setDefaultSpeed(int speed);
    int getDefaultSpeed();
};
 
#endif

