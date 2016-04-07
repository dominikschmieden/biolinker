#ifndef CommandHandler_H
#define CommandHandler_H
 
#include <Arduino.h>
#include <StandardCplusplus.h>
#include <vector>
#include <queue>
#include <iterator>
#include <AFMotor.h>
#include <EEPROM.h>
#include "config.cpp"
#include "Command.h"
#include "Pump.h"
#include "PumpCommand.h"
#include "PauseCommand.h"
#include "MotorCommand.h"
#include "DistanceSensor.h"
 
class CommandHandler {
  private:
    int bufferEnd = 0;  
    char inputBuffer[64];   
    DistanceSensor * const XSensor;
    DistanceSensor * const YSensor;
    DistanceSensor * const ZSensor;
    Pump  * const pump1;
    Motor * const XMotor;
    Motor * const YMotor;
    Motor * const ZMotor;
    std::queue< std::queue<Command*> > waitingCommandGroups;
    std::vector<Command*> runningCommands;
  public:
    CommandHandler(
      DistanceSensor * const XSensor,
      DistanceSensor * const YSensor,
      DistanceSensor * const ZSensor,      
      Pump  * const pump1,
      Motor * const XMotor,
      Motor * const YMotor,
      Motor * const ZMotor
    );
    void readCommandsFromSerial();
    void executeCommands();
    bool cmdFinished(Command *command);
    void parseCommand(String command);    
};
 
#endif

