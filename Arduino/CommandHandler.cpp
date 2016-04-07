#include "CommandHandler.h"

CommandHandler::CommandHandler(
  DistanceSensor * const XSensor,
  DistanceSensor * const YSensor,
  DistanceSensor * const ZSensor,      
  Pump * const  pump1,
  Motor * const XMotor,
  Motor * const YMotor,
  Motor * const ZMotor
): 
  XSensor(XSensor), 
  YSensor(YSensor), 
  ZSensor(ZSensor), 
  pump1(pump1),
  XMotor(XMotor),
  YMotor(YMotor),
  ZMotor(ZMotor)
{};

// executes the command given as parameter
void CommandHandler::parseCommand(String command) {
  command.trim();
  // read the parameters of the current command
  int parameters[4] = {0};
  String bufferString;
  int j = 0;
  for (int i = 1, j = 0; i < command.length() && j < 4; i++) {
    if (command.charAt(i) != ' ') {
      bufferString.concat(command.charAt(i));
    }
    if (command.charAt(i) == ' ' || i == command.length() -1) {
      parameters[j] = bufferString.toInt();
      bufferString = "";
      j++;
    }
  }  
  
  if (command.startsWith("G")) {
    switch (parameters[0]) {
      case 0: // move to a co-ordinate position without turning on the pumps
      {
        int targetX = parameters[1];
        int targetY = parameters[2];
        int targetZ = parameters[3];
        std::queue <Command*> commands1;

        MotorCommand* moveX = new MotorCommand(XMotor);
        moveX->moveTo(targetX);
        commands1.push(moveX);
       
        MotorCommand* moveY = new MotorCommand(YMotor);
        moveY->moveTo(targetY);
        commands1.push(moveY);

        //MotorCommand* moveZ = new MotorCommand(ZMotor);
        //moveZ->moveTo(targetZ);
        //commands1.push(moveZ);
                       
        this->waitingCommandGroups.push(commands1);
        break;
      }
      case 1: // move to a co-ordinate position AND have pumps on on the way
      {
        int targetX = parameters[1];
        int targetY = parameters[2];
        int targetZ = parameters[3];
                
        std::queue <Command*> commands1;
        commands1.push(new PumpCommand(this->pump1)); // pump on
        this->waitingCommandGroups.push(commands1);
        
        std::queue <Command*> commands2;
        MotorCommand* moveX = new MotorCommand(XMotor);
        moveX->moveTo(targetX);
        commands2.push(moveX);

        MotorCommand* moveY = new MotorCommand(YMotor);
        moveY->moveTo(targetY);
        commands2.push(moveY);

        //MotorCommand* moveZ = new MotorCommand(ZMotor);
        //moveZ->moveTo(targetZ);
        //commands2.push(moveZ);        

        this->waitingCommandGroups.push(commands2);        

        std::queue <Command*> commands3;
        commands3.push(new PumpCommand(this->pump1, 0, 0)); // pump off
        this->waitingCommandGroups.push(commands3);
        break;
      }
      case 4: // wait for a given number of milliseconds
      {
        std::queue <Command*> commands1;
        commands1.push(new PauseCommand(parameters[1]));
        this->waitingCommandGroups.push(commands1);        
      break;
      }
      default:
        Serial.print("Unknown command: ");
        Serial.println(command);
    }
  } else if (command.startsWith("M")) {
    switch (parameters[0]) {
      case 114: // report x y z in mm
        Serial.println("xyz " + String(XSensor->getDistance()) + " " + String(YSensor->getDistance()) + " " + String(ZSensor->getDistance()));
      break;
      case 115: // report calibration distances in mm 
        Serial.println("cal " + String(XMotor->getMaxDistance()) + " " + String(YMotor->getMaxDistance()) + " " + ZMotor->getMaxDistance());
      break;
      default:
        Serial.print("Unknown command: ");
        Serial.println(command);
    }
  } else if (command.startsWith("T")) {
    switch (parameters[0]) {
      case 1: // turn on pump 1 (parameter 1 = direction, parameter 2 = speed)
      {
        std::queue <Command*> commands;
        commands.push(new PumpCommand(this->pump1, (bool) parameters[1], parameters[2]));
        this->waitingCommandGroups.push(commands);
        break;        
      }
      case 2:
        Serial.println("Starting pump 2 - not implemented"); 
      break;
      case 3: // direct motor control (parameter 1 = which motor, parameter 2 = direction, parameter 3 = speed)
      {
        Motor * motors[] = {this->XMotor, this->YMotor, this->ZMotor};
        Motor * motor = motors[parameters[1]];
        int dir   = parameters[2];
        int speed = parameters[3];
        MotorCommand* useMotor = new MotorCommand(motor);    
        useMotor->setSpeed(speed);        
        if (dir == 0) {
          useMotor->setDirection(FORWARD);
        } else {
          useMotor->setDirection(BACKWARD);
        }
        useMotor->turnOn();
        std::queue <Command*> commands;
        commands.push(useMotor);
        this->waitingCommandGroups.push(commands);
        break;   
      }
      case 10: // set current position to zero
      {
        
        // overwrite reads in sensor memory
        for (int i = 0; i <= MEDIAN_READS; i++) {
          this->XSensor->readUnfiltered();
          this->YSensor->readUnfiltered();
          this->ZSensor->readUnfiltered();
          delay(20);
        }

        int newX = this->XSensor->getDistance();
        int newY = this->YSensor->getDistance();
        int newZ = this->ZSensor->getDistance();

        this->XSensor->setZeroDistance(newX); 
        this->YSensor->setZeroDistance(newY);
        this->ZSensor->setZeroDistance(newZ);

        EEPROM.write(ADDR_ZEROX, newX);
        EEPROM.write(ADDR_ZEROY, newY);
        EEPROM.write(ADDR_ZEROZ, newZ);

        delay(100);

        // overwrite reads in sensor memory with filtered values
        for (int i = 0; i <= MEDIAN_READS; i++) {
          this->XSensor->read();
          this->YSensor->read();
          this->ZSensor->read();
          delay(20);
        }           

        Serial.println("Zero calibration performed: " + String(newX) + " " + String(newY) + " " + String(newZ));
        break;
      }
      case 11: // set current position to MAX
      {
        // overwrite reads in sensor memory
        for (int i = 0; i <= MEDIAN_READS; i++) {
          this->XSensor->readUnfiltered();
          this->YSensor->readUnfiltered();
          this->ZSensor->readUnfiltered();
          delay(20);
        }

        int newX = this->XSensor->getDistance() - this->XSensor->getZeroDistance();
        int newY = this->YSensor->getDistance() - this->YSensor->getZeroDistance();
        int newZ = this->ZSensor->getDistance() - this->ZSensor->getZeroDistance();

        this->XMotor->setMaxDistance(newX); 
        this->YMotor->setMaxDistance(newY);
        this->ZMotor->setMaxDistance(newZ);      

        EEPROM.write(ADDR_MAXX, newX);
        EEPROM.write(ADDR_MAXY, newY);
        EEPROM.write(ADDR_MAXZ, newZ);

        // overwrite reads in sensor memory with filtered values
        for (int i = 0; i <= MEDIAN_READS; i++) {
          this->XSensor->read();
          this->YSensor->read();
          this->ZSensor->read();
          delay(20);
        }        

        delay(100);

        Serial.println("Maximum calibration performed: " + String(newX) + " " + String(newY) + " " + String(newZ));
        break;
      }
      case 12: // set pump 1 default speed and direction
      {
        int direction = parameters[1];
        int speed = parameters[2];
        this->pump1->setDefaultDirection(direction);
        this->pump1->setDefaultSpeed(speed);
        EEPROM.write(ADDR_PUMP1_DIRECTION, this->pump1->getDefaultDirection());
        EEPROM.write(ADDR_PUMP1_SPEED, this->pump1->getDefaultSpeed());
        
        break;
      }
      case 13: // set pump 2 default speed and direction
      {
//        int direction = parameters[1];
//        int speed = parameters[2];
//        this->pump1->setDefaultDirection(direction);
//        this->pump1->setDefaultSpeed(speed);
//        EEPROM.write(ADDR_PUMP1_DIRECTION, this->pump1->getDefaultDirection());
//        EEPROM.write(ADDR_PUMP1_SPEED, this->pump1->getDefaultSpeed());
        // pump 2 not implemented yet
        break;
      }      
      case 14: // set default speeds
      {
        int speedX = parameters[1];
        int speedY = parameters[2];
        this->XMotor->setDefaultSpeed(speedX);
        this->YMotor->setDefaultSpeed(speedY);
        this->XMotor->setSpeed(speedX);
        this->YMotor->setSpeed(speedY);        
        EEPROM.write(ADDR_XMOTOR_SPEED, this->XMotor->getDefaultSpeed());
        EEPROM.write(ADDR_YMOTOR_SPEED, this->YMotor->getDefaultSpeed());
        Serial.println("speedx" + String(speedX));
        Serial.println("speedy" + String(speedY));
        break;
      }
      default: // stop pumps and motor
      {
        // stop pumps
        Wire.beginTransmission(this->pump1->getDeviceAddress());
        String command = "0 0";
        Wire.write(command.c_str());
        Wire.endTransmission();
        this->XMotor->turnOff();
        this->YMotor->turnOff();
        this->ZMotor->turnOff();

        // remove waiting commands
        while (!this->waitingCommandGroups.empty()) {
          std::queue<Command*> group = this->waitingCommandGroups.front();
          this->waitingCommandGroups.pop();
          while (!group.empty()) {
            Command * cmd = group.front();
            delete cmd;
            group.pop();
          }
        }

        // stop other commands and empty command queue
        if (!this->runningCommands.empty()) {
          std::vector<Command*>::iterator it;
          for (it = runningCommands.begin(); it != runningCommands.end(); it++) {
            (*it)->endCommand();
            delete (*it);           
          } 
          std::vector<Command*>().swap(runningCommands);
        }
      }     
    } 
  } else {
    Serial.print("Unknown command: ");
    Serial.println(command);
  }
}

// this function is called in the loop() function of the main script
// it manages the commands by checking for completion, removing completed ones and
// adding new ones from the waiting queue
// commands are grouped in command groups, so that several can run at the same time
// each command group is brought to completion before the next one is started
void CommandHandler::executeCommands() {
  // if no commands are running, take a new command group from the waiting queue
  if (this->runningCommands.empty()) {
    std::queue<Command*> nextCommandGroup;
    if (!this->waitingCommandGroups.empty()) {
      nextCommandGroup = this->waitingCommandGroups.front();
      this->waitingCommandGroups.pop();
      while (!nextCommandGroup.empty()) {
        this->runningCommands.push_back(nextCommandGroup.front());
        nextCommandGroup.pop();
      }
    }
  }

  // run the currently active commands
  if (!this->runningCommands.empty()) {
    std::vector<Command*>::iterator it;
    for (it = runningCommands.begin(); it != runningCommands.end(); ++it) {
      //Serial.println("Found command: " + (*it)->toString());
      if (!(*it)->isStarted()) {
        (*it)->startCommand();
      }
      if (!(*it)->isFinished()) {
        //Serial.println((*it)->toString());
        (*it)->loop();
      }
    }

    // remove commands that have run to completion
    it = runningCommands.begin();
    while (it != runningCommands.end()) {
      if ((*it)->isFinished()) {
        delete (*it);
        it = runningCommands.erase(it);
      } else {
        ++it;
      }
    } 
  }
}


