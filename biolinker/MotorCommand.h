#ifndef MotorCommand_H
#define MotorCommand_H
 
#include <Arduino.h>
#include "Command.h"
#include "DistanceSensor.h"
#include "Motor.h"
#include <AFMotor.h>
 
class MotorCommand: public Command {
  private:
    unsigned long timer;
    Motor * const motor;
    int targetDistance;
    const int NOCOMMAND = 0;
    const int MOVE = 1;
    const int TURNON = 2;
    const int TURNOFF = 3;
    const int REVERSE = 4;
    int command = NOCOMMAND;
  public:
    MotorCommand(Motor * const motor);
    void turnOn();
    void turnOff();
    void moveTo(int targetDistance);
    void setDirection(int direction);
    void setSpeed(int speed);
    int  getDirection() const;
    int  getSpeed() const;
    void reverseDirection();
    void startCommand();
    void loop();
    void endCommand();   
    String toString() const; 
};
 
#endif

