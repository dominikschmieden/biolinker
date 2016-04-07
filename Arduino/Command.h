#ifndef Command_H
#define Command_H
 
#include <Arduino.h>
 
class Command {
  private:
    bool started = false; 
    bool finished = false; // has stopped
  public:
    virtual void startCommand() = 0;
    virtual void loop() = 0;
    virtual void endCommand() = 0;
    virtual String toString() const = 0;
    bool isStarted() const;    
    bool isFinished() const;
    void setStarted();
    void setFinished();
};
 
#endif

