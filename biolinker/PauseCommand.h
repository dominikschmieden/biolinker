#ifndef PauseCommand_H
#define PauseCommand_H
 
#include <Arduino.h>
#include "Command.h"
 
class PauseCommand: public Command {
  private:
    unsigned long duration;
    unsigned long timer;
  public:
    PauseCommand(unsigned long duration);
    void startCommand();
    void loop();
    void endCommand();   
    String toString() const; 
};
 
#endif

