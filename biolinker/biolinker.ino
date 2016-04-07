#include <StandardCplusplus.h>
#include "config.cpp"
#include <NewPing.h>
#include <Wire.h>
#include <AFMotor.h>
#include <EEPROM.h>
#include "CommandHandler.h"

unsigned long timer1 = 0;
unsigned long timer2 = 0;
unsigned long timer3 = 0;
unsigned long timer4 = 0;

Pump           pump1   = Pump(PUMP1_ADDRESS, 0, 0);
DistanceSensor XSensor = DistanceSensor("Sensor X", TRIGGER_X, ECHO_X, MAX_SENSING_DISTANCE, 0);
DistanceSensor YSensor = DistanceSensor("Sensor Y", TRIGGER_Y, ECHO_Y, MAX_SENSING_DISTANCE, 0);
DistanceSensor ZSensor = DistanceSensor("Sensor Z", TRIGGER_Z, ECHO_Z, MAX_SENSING_DISTANCE, 0);
Motor          XMotor  = Motor("Motor X", 255, 0, XMOTOR_PORT, &XSensor);
Motor          YMotor  = Motor("Motor Y", 255, 0, YMOTOR_PORT, &YSensor);
Motor          ZMotor  = Motor("Motor Z", 255, 0, ZMOTOR_PORT, &ZSensor);

CommandHandler commandHandler(&XSensor, &YSensor, &ZSensor, &pump1, &XMotor, &YMotor, &ZMotor);

void setup() {
  Serial.begin(BAUD); // serial communication to PC
  Wire.begin();       // serial communication between arduino boards

  // read calibrations from memory and update corresponding objects
  XSensor.setZeroDistance(EEPROM.read(ADDR_ZEROX));
  YSensor.setZeroDistance(EEPROM.read(ADDR_ZEROY));
  ZSensor.setZeroDistance(EEPROM.read(ADDR_ZEROZ));
  XMotor.setMaxDistance(EEPROM.read(ADDR_MAXX));
  YMotor.setMaxDistance(EEPROM.read(ADDR_MAXY));  
  ZMotor.setMaxDistance(EEPROM.read(ADDR_MAXZ));  
  XMotor.setDefaultSpeed(EEPROM.read(ADDR_XMOTOR_SPEED));
  YMotor.setDefaultSpeed(EEPROM.read(ADDR_YMOTOR_SPEED));
  XMotor.setSpeed(XMotor.getDefaultSpeed());
  YMotor.setSpeed(YMotor.getDefaultSpeed());
  pump1.setDefaultDirection(EEPROM.read(ADDR_PUMP1_DIRECTION));
  pump1.setDefaultSpeed(EEPROM.read(ADDR_PUMP1_SPEED));
  
  Serial.println("Biolinker ready...");
  commandHandler.parseCommand("M115"); // send calibrations to computer interface
}

// read commands from serial
int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\r': // Ignore CR
        break;
      case '\n': // Return on nl
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len-1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}

void loop() {
  static char buffer[64];
  while (readline(Serial.read(), buffer, 64) > 0) {
    commandHandler.parseCommand(buffer);
  }    
  if ((millis() - timer1) > 200) {
    commandHandler.parseCommand("M114");
    timer1 = millis();
  }
  if ((millis() - timer2) > 20) {
    XSensor.read();
    YSensor.read();
    ZSensor.read();    
    commandHandler.executeCommands();
    timer2 = millis();
  }
}

