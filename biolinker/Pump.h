#ifndef Pump_H
#define Pump_H
 
#include <Arduino.h>
 
class Pump {
  private:
    const int deviceAddress;
    int defaultSpeed = 0;
    int defaultDirection = 0;
  public:
    Pump(const int deviceAddress, int defaultDirection, int defaultSpeed);
    int getDeviceAddress() const;
    void setDefaultSpeed(int speed);
    int getDefaultSpeed();
    void setDefaultDirection(int direction);
    int getDefaultDirection();
};
 
#endif

