#include "DistanceSensor.h"

DistanceSensor::DistanceSensor(
  const String name,
  const int triggerPin, 
  const int echoPin, 
  const int maxDistance, 
  const int zeroDistance
): 
  name(name),
  triggerPin(triggerPin), 
  echoPin(echoPin), 
  maxDistance(maxDistance), // mm
  zeroDistance(zeroDistance), // mm
  device(device) 
{
  this->device = NewPing(triggerPin, echoPin, maxDistance/10);  
}

DistanceSensor::DistanceSensor() :
  name(0),
  echoPin(0),
  triggerPin(0),
  maxDistance(0),
  zeroDistance(0),
  device(0, 0, 0)
{}

// do a distance read
void DistanceSensor::read() {
  float read = (float)this->device.ping() * soundSpeed / 1000.0 - this->zeroDistance;
  //Serial.println("Ping: " + String((float)this->device.ping() * soundSpeed / 1000.0));
  if (read >= -20 && read <= this->maxDistance*1.2) {
    this->lastReads.push(read);
    if (this->lastReads.size() > MEDIAN_READS) {
      this->lastReads.pop();
    } 
  }
}

// do a distance read without filtering (for calibrations
void DistanceSensor::readUnfiltered() {
  float read = (float)this->device.ping() * soundSpeed / 1000.0;
  //Serial.println("Ping: " + String((float)this->device.ping() * soundSpeed / 1000.0));
  this->lastReads.push(read);
  if (this->lastReads.size() > MEDIAN_READS) {
    this->lastReads.pop();
  }
}

int DistanceSensor::getDistance() { // distance in mm
  // calculate the median
  std::vector<float> lastReadsSorted;
  std::queue<float> lastReadsTmp = this->lastReads;
  while (!lastReadsTmp.empty()) {
    float read = lastReadsTmp.front();
    if (read > 0.0f) {
      lastReadsSorted.push_back(read);
    }
    lastReadsTmp.pop();
  }
  if (lastReadsSorted.size() > 0 ) {
    std::sort(lastReadsSorted.begin(), lastReadsSorted.end());
    
//    Serial.print(this->name + " measured: ");
//    for (std::vector<float>::iterator it = lastReadsSorted.begin(); it != lastReadsSorted.end(); ++it) {
//      Serial.print(String(*it) + String("; "));
//    }
//    Serial.println();
    
    if (lastReadsSorted.size() % 2 == 1) {
      int position = (int) floor(lastReadsSorted.size()/2);
      return (int) (lastReadsSorted[position]);     
    } else {
      int position = (int) lastReadsSorted.size()/2;
      return (int) ((lastReadsSorted[position-1] + lastReadsSorted[position])/2);     
    }
  } else {
    return 0;
  }
}

void DistanceSensor::setZeroDistance(int distance) {
  this->zeroDistance = distance;
}

int DistanceSensor::getMaxDistance() {
  return this->maxDistance;
}

int DistanceSensor::getZeroDistance() {
  return this->zeroDistance;
}


