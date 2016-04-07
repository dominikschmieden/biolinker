#ifndef DistanceSensor_H
#define DistanceSensor_H

#include "config.cpp"
#include <Arduino.h>
#include <NewPing.h>
#include <StandardCplusplus.h>
#include <queue>
#include <vector>
#include <algorithm>
 
class DistanceSensor {
  friend class MotorCommand;
  private:
    DistanceSensor();
    const int triggerPin;
    const int echoPin;
    const int maxDistance;
    int zeroDistance;
    const float soundSpeed = 331.3 + 0.606 * 22; // m/s accounting for temperature;
    NewPing device;
    std::queue <float> lastReads;
    const String name;
  public:
    DistanceSensor(const String name, const int triggerPin, const int echoPin, const int maxDistance, int zeroDistance);
    void read(); // do a distance read
    void readUnfiltered(); // do a distance read for calibrations 
    int getDistance(); // in mm
    void setZeroDistance(int distance);
    int getMaxDistance();
    int getZeroDistance();
};
 
#endif

