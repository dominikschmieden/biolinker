#ifndef PumpCommand_H
#define PumpCommand_H
 
#include <Arduino.h>
#include <Wire.h>
#include "Command.h"
#include "Pump.h"
 
class PumpCommand: public Command {
  private:
    bool counterClockwise = 0; // direction
    int speed = 0;
    Pump * const pump;
  public:
    PumpCommand(Pump * const pump);
    PumpCommand(Pump * const pump, bool counterClockwise, int speed);    
    void setCounterClockwise(bool counterClockwise) ;
    bool getCounterClockwise() const;
    void setSpeed(int speed);
    int getSpeed() const;
    Pump * const getPump() const;
    void startCommand();
    void loop();
    void endCommand();   
    String toString() const; 
};
 
#endif

