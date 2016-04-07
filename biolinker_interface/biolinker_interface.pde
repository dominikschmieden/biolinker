import g4p_controls.*;
import processing.serial.*;
import java.util.List;
import java.util.ArrayList;
import java.awt.Font;
import java.awt.*;

// communication with arduino
// code from http://www.instructables.com/id/Arduino-to-Processing-Serial-Communication-withou
int endline = 10; // ASCII code for linefeed
String serial;
List<String> log = new ArrayList<String>();
List<String> positionLog = new ArrayList<String>();
boolean logFileSaved = false;
long logTimer = 0;
Serial port;
boolean logPosition = false;
boolean calibrationReceived = false;
boolean foundPrinter = false;

// GUI - all coordinates in mm
// printer coordinates
int printerX = 0;
int printerY = 0;
int printerZ = 0;
int printerMaxX = 0;
int printerMaxY = 0;
int printerMaxZ = 30;
int printerMinX = 0;
int printerMinY = 0;
int printerMinZ = 0;
int targetX = 0;
int targetY = 0;
int targetZ = 0;

// position screen
int xyScreenX = 500;
int xyScreenY = 100;
int xyScreenWidth = 500;
int xyScreenHeight = 500;

int zScreenX = xyScreenX + xyScreenWidth + 60;
int zScreenY = 100;
int zScreenHeight = 500;
int zScreenWidth = 20;

int markerSize = 20;

// control elements
GTextField inputX;
GTextField inputY;
GTextField inputZ;

GButton targetButton;
GCheckbox moveWithExtrusion;
GToggleGroup inkSelector;
GOption ink1;
GOption ink2;
GTextArea programInput;
GButton programSubmit;
GButton cancelAll;
GButton zeroStage;
GButton maxStage;
GButton centerStage;
GCheckbox logPositionData;
GButton saveToLogfiles;

GTextField pump1speed;
GToggleGroup pump1directionSelector;
GOption pump1forward;
GOption pump1backward;
GButton pump1start;

GTextField pump2speed;
GToggleGroup pump2directionSelector;
GOption pump2forward;
GOption pump2backward;
GButton pump2start;

GTextField pump1defaultSpeed;
GToggleGroup pump1defaultDirectionSelector;
GOption pump1defaultForward;
GOption pump1defaultBackward;
GButton pump1defaultsSubmit;

GTextField xMotorDefaultSpeed;  
GTextField yMotorDefaultSpeed;  
GButton motorDefaultsSubmit;

GButton calibrateZero;
GButton calibrateMax;

PFont font;
int time = 0;
int lastTime = 0;

void serialEvent(Serial p) {
  try {
    String response = p.readString();
    response = response.trim();
    
    if (foundPrinter == false && response.startsWith("xyz")) {
      foundPrinter = true;
    }
    
    if (!response.startsWith("xyz") && !response.startsWith("cal")) {
      print(response+"\n");
      log.add(response);
    }
    
    String[] params = split(response, ' '); 
    if (params[0].equals("xyz")) {
      setPrinterX(Integer.parseInt(params[1]));
      setPrinterY(Integer.parseInt(params[2]));
      setPrinterZ(Integer.parseInt(params[3]));
      if (logPosition == true) {
        positionLog.add((millis()-logTimer) + "\t" + params[1] + "\t" + params[2] + "\t" + params[3] + "\n");
      } 
    } else if (params[0].equals("cal")) {
      printerMaxX = Integer.parseInt(params[1]);
      printerMaxY = Integer.parseInt(params[2]);
      printerMaxZ = Integer.parseInt(params[3]);
      calibrationReceived = true;
      print("Calibration settings from printer received.\n");
    }
  } catch (Exception e) {
    print(e.toString());
  }
}

void setup() {
  // find the printer
  try {
    String[] ports = Serial.list();
    if (ports.length == 0) {
      throw new Exception("The printer could not be detected (no devices were found on any COM port).");
    }
    for (int i = 0; i < ports.length; i++) {
      port = new Serial(this, ports[i], 115200);
      port.bufferUntil(endline);
      port.clear();
      int start = millis();
      while (millis() - start < 2000) {
        // wait for serial event to detect the printer on the current port
        if (foundPrinter == true) {
          break;
        }
      }
      
      if (foundPrinter) {
        port.clear();
        break;
      } else {
        port.stop();
      }        
    }
    if (!foundPrinter) {
      StringBuilder message = new StringBuilder();
      message.append("The printer could not be detected, I tried devices on the following port(s): ");
      for(String s : Serial.list()) {
        message.append(s);
        message.append(" ");
      }
      throw new Exception(message.toString());
    }
  } catch (Exception e) {
    G4P.showMessage(this, e.getMessage(), "Printer not found", G4P.ERROR);
  }
  
  size(1200, 900);
  stroke(0);
  background(255);
  font = createFont("Arial", 14, true);
  textFont(font, 30);
  fill(0);
  text("Biolinker Control", 20, 50);
  if (!foundPrinter) {
    textFont(font, 20);
    fill(0);
    text("Can\'t detect the printer. Connect and restart program to proceed.", 420, 400);
  }
  textFont(font, 14);  
  
  stroke(87);
  fill(209);
  
  rect(20, 80, 160, 210);
  rect(200, 80, 180, 210);
  rect(20, 300, 360, 350);
  rect(20, 670, 170, 150);
  rect(210, 670, 170, 150);
  rect(400, 670, 170, 150);
  rect(590, 670, 170, 150);
  rect(780, 670, 170, 150);    
  
  stroke(0);
  fill(0);
  
  text("X", 30, 100);
  inputX = new GTextField(this, 50, 86, 100, 20);
  text("Y", 30, 130);
  inputY = new GTextField(this, 50, 116, 100, 20);
  text("Z", 30, 160);
  inputZ = new GTextField(this, 50, 146, 100, 20);
  
  moveWithExtrusion = new GCheckbox(this, 30, 176, 100, 20, "Extrude ink");
  inkSelector = new GToggleGroup();
  ink1 = new GOption(this, 30, 206, 100, 20, "Ink 1");
  ink2 = new GOption(this, 30, 236, 100, 20, "Ink 2");
  inkSelector.addControl(ink1);
  inkSelector.addControl(ink2);
  ink1.setSelected(true);
  targetButton = new GButton(this, 30, 266, 140, 20, "Go");
  
  programInput = new GTextArea(this, 30, 310, 340, 300);
  programInput.setPromptText("Enter movement commands here...");
  programSubmit = new GButton(this, 30, 620, 340, 20, "Start");
  
  cancelAll = new GButton(this, 210, 90, 160, 40, "Stop all"); 
  zeroStage = new GButton(this, 210, 140, 160, 20, "Zero Stage");
  maxStage = new GButton(this, 210, 170, 160, 20, "Max Stage");
  centerStage = new GButton(this, 210, 200, 160, 20, "Center Stage");  
  logPositionData = new GCheckbox(this, 210, 230, 160, 20, "Log Position");
  saveToLogfiles = new GButton(this, 210, 250, 160, 20, "Save to log");  

  textFont(font, 14); 
  text("Pump 1", 30, 694);
  text("Speed", 30, 726);
  pump1speed = new GTextField(this, 80, 710, 100, 20);  
  pump1directionSelector = new GToggleGroup();
  pump1forward  = new GOption(this, 30, 740, 100, 20, "Forward");
  pump1backward = new GOption(this, 30, 766, 100, 20, "Backward");
  pump1directionSelector.addControl(pump1forward);
  pump1directionSelector.addControl(pump1backward);
  pump1forward.setSelected(true);  
  pump1start = new GButton(this, 30, 790, 150, 20, "Send");
  
  text("Pump 2", 220, 694);
  text("Speed", 220, 726);
  pump2speed = new GTextField(this, 270, 710, 100, 20);  
  pump2directionSelector = new GToggleGroup();
  pump2forward  = new GOption(this, 220, 740, 100, 20, "Forward");
  pump2backward = new GOption(this, 220, 766, 100, 20, "Backward");
  pump2directionSelector.addControl(pump2forward);
  pump2directionSelector.addControl(pump2backward);
  pump2forward.setSelected(true);  
  pump2start = new GButton(this, 220, 790, 150, 20, "Send");
  
  text("Stage calibration", 410, 694);
  calibrateZero = new GButton(this, 410, 720, 150, 40, "Set Zero");
  calibrateMax = new GButton(this, 410, 770, 150, 40, "Set Maximum");
  
  text("Pump 1 defaults", 600, 694);
  text("Speed", 600, 726);
  pump1defaultSpeed = new GTextField(this, 650, 710, 100, 20);  
  pump1defaultDirectionSelector = new GToggleGroup();
  pump1defaultForward  = new GOption(this, 600, 740, 100, 20, "Forward");
  pump1defaultBackward = new GOption(this, 600, 766, 100, 20, "Backward");
  pump1defaultDirectionSelector.addControl(pump1defaultForward);
  pump1defaultDirectionSelector.addControl(pump1defaultBackward);
  pump1defaultForward.setSelected(true);  
  pump1defaultsSubmit = new GButton(this, 600, 790, 150, 20, "Save");
  
  text("Motor Speeds", 790, 694);
  text("X", 790, 726);
  xMotorDefaultSpeed = new GTextField(this, 810, 710, 130, 20);  
  text("Y", 790, 756);
  yMotorDefaultSpeed = new GTextField(this, 810, 740, 130, 20);    
  motorDefaultsSubmit = new GButton(this, 790, 790, 150, 20, "Save");
}

void draw() {  
  // do every 0.1 s:
  time = millis();
  if (time-lastTime > 100) {
    if (calibrationReceived) {
      drawScreen();
    }
    lastTime = time;
  }
}

public void drawScreen() {
  // clearing rectangle
  fill(255);
  stroke(255);
  rect(xyScreenX-100, xyScreenY-200, xyScreenWidth+230, xyScreenHeight+230);
  stroke(0);
  
  // determine positions of markers on screen
  float markerX = ((float) printerX/printerMaxX)*xyScreenWidth  + xyScreenX;
  float markerY = ((float) printerY/printerMaxY)*xyScreenHeight + xyScreenY;
  float markerZ = ((float) printerZ/printerMaxZ)*zScreenHeight  + zScreenY;
  
  float targetMarkerX = ((float) targetX/printerMaxX)*xyScreenWidth  + xyScreenX;
  float targetMarkerY = ((float) targetY/printerMaxY)*xyScreenHeight + xyScreenY;
  float targetMarkerZ = ((float) targetZ/printerMaxZ)*zScreenHeight  + zScreenY;
  
  // print control screen
  rect(xyScreenX, xyScreenY, xyScreenWidth, xyScreenHeight); // xy screen
  rect(zScreenX, zScreenY, zScreenWidth, zScreenHeight); // z screen
  
  // print target markers
  stroke(180);
  // xy target marker
  line(targetMarkerX, xyScreenY, targetMarkerX, xyScreenY + xyScreenHeight);
  line(xyScreenX, targetMarkerY, xyScreenX + xyScreenWidth, targetMarkerY);
  // z target marker
  line(zScreenX, targetMarkerZ, zScreenX + zScreenWidth, targetMarkerZ);
  
  // print markers
  stroke(0);
  // xy marker
  line(markerX-markerSize/2, markerY, markerX+markerSize/2, markerY);
  line(markerX, markerY-markerSize/2, markerX, markerY+markerSize/2);
  // z marker
  line(zScreenX, markerZ, zScreenX + zScreenWidth, markerZ);
  
  
  fill(0);
  
  text(printerX+" mm", markerX - 10, xyScreenY - 6);
  text(printerY+" mm", xyScreenX - 50, markerY + 6);
  
  text("0 mm", zScreenX, zScreenY - 6);
  text(printerMaxZ+" mm", zScreenX, zScreenY + zScreenHeight + 16);
  text(printerZ+" mm", zScreenX + zScreenWidth + 10, markerZ + 6);
  fill(255);
}

public void setPrinterX(int newX) {
  if (newX > printerMaxX) {
    newX = printerMaxX;
  }
  if (newX < 0) {
    newX = 0;   
  }
  printerX = newX;
}

public void setPrinterY(int newY) {
  if (newY > printerMaxY) {
    newY = printerMaxY;     
  }
  if (newY < 0) {
    newY = 0;    
  }
  printerY = newY;
}

public void setPrinterZ(int newZ) {
  if (newZ > printerMaxZ) {
    newZ = printerMaxZ;     
  }
  if (newZ < 0) {
    newZ = 0;     
  }
  printerZ = newZ;
}

public void handleTextEvents(GEditableTextControl textcontrol, GEvent event) { /* code */ }
public void handleToggleControlEvents(GToggleControl checkbox, GEvent event) { /* code */ }

public void handleButtonEvents(GButton button, GEvent event) {
  
  if (logPositionData.isSelected() == true) {
    logPosition = true;
    logTimer = millis();
  } else {
    logPosition = false;
  }
  
  // move printhead
  if (button == targetButton && event == GEvent.CLICKED ) {
    int newX = printerX;
    int newY = printerY;
    int newZ = printerZ;
    try {
      newX = Integer.parseInt(inputX.getText());
      newY = Integer.parseInt(inputY.getText());
      newZ = Integer.parseInt(inputZ.getText());
      if (newX > printerMaxX) {
        newX = printerMaxX;
      }
      if (newX < 0) {
        newX = 0;  
      }
      if (newY > printerMaxY) {
        newY = printerMaxY;  
      }
      if (newY < 0) {
        newY = 0;   
      }
      if (newZ > printerMaxZ) {
        newZ = printerMaxZ;  
      }
      if (newZ < 0) {
        newZ = 0; 
      }
      targetX = newX;
      targetY = newY;
      targetZ = newZ;
      
      // send commands to printer
      
      // should ink be extruded? not implemented yet
      if (moveWithExtrusion.isSelected()) {
        if (ink2.isSelected()) {
          port.write("G1 " + newX + " " + newY + " " + newZ + "\n"); // use ink 2
        } else {
          port.write("G1 " + newX + " " + newY + " " + newZ + "\n"); // use ink 1 (default ink)
        }
      } else {
        port.write("G0 " + newX + " " + newY + " " + newZ + "\n"); // turn off ink
      }      
    } catch(NumberFormatException e) {
      G4P.showMessage(this, "Please enter a valid integer value", "Wrong input type", G4P.ERROR);
    }
  } else if (button == programSubmit && event == GEvent.CLICKED ) {
    int line = 0;
    while (programInput.getText(line) != "") {
      port.write(programInput.getText(line)+"\n");
      delay(30);
      line++;
    }
  } else if (button == cancelAll && event == GEvent.CLICKED ) {
    port.write("T5"+"\n");
  } else if (button == pump1start && event == GEvent.CLICKED) {
    try {
      int speed = Integer.parseInt(pump1speed.getText());
      if (speed < 0 || speed > 255) {
        throw new NumberFormatException(); 
      }
      if (pump1forward.isSelected()) {
        port.write("T1 " + "0 " + speed + "\n");
      } else {
        port.write("T1 " + "1 " + speed + "\n");
      }
    } catch (NumberFormatException e) {
      G4P.showMessage(this, "Please enter a valid integer value (0-255).", "Wrong input type", G4P.ERROR);
    }
  } else if (button == pump1defaultsSubmit && event == GEvent.CLICKED) {
    try {
      int speed = Integer.parseInt(pump1defaultSpeed.getText());
      if (speed < 0 || speed > 255) {
        throw new NumberFormatException(); 
      }
      if (pump1defaultForward.isSelected()) {
        port.write("T12 " + "0 " + speed + "\n");
      } else {
        port.write("T12 " + "1 " + speed + "\n");
      }
      G4P.showMessage(this, "New default speed and direction saved.", "Saved", G4P.INFO);
    } catch (NumberFormatException e) {
      G4P.showMessage(this, "Please enter a valid integer value (0-255).", "Wrong input type", G4P.ERROR);
    }
  } else if (button == motorDefaultsSubmit && event == GEvent.CLICKED) {
    try {
      int speedX = Integer.parseInt(xMotorDefaultSpeed.getText());
      int speedY = Integer.parseInt(xMotorDefaultSpeed.getText());      
      if (speedX < 0 || speedX > 255 || speedY < 0 || speedY > 255) {
        throw new NumberFormatException(); 
      }
      port.write("T14 " + speedX + " " + speedY + "\n");
      G4P.showMessage(this, "New default speeds saved.", "Saved", G4P.INFO);
    } catch (NumberFormatException e) {
      G4P.showMessage(this, "Please enter a valid integer value (0-255).", "Wrong input type", G4P.ERROR);
    }    
  } else if (button == pump2start && event == GEvent.CLICKED) {   
    G4P.showMessage(this, "Sorry, only one pump working at this point ;)", "Woops", G4P.ERROR);
  } else if (button == zeroStage && event == GEvent.CLICKED) {
    targetX = 0;
    targetY = 0;
    targetZ = 0;
    port.write("G0 0 0 0\n");
  } else if (button == maxStage && event == GEvent.CLICKED) {
    targetX = printerMaxX;
    targetY = printerMaxY;
    targetZ = printerMaxZ;    
    port.write("G0 " + printerMaxX + " " + printerMaxY + " " + printerMaxZ + "\n");
  } else if (button == centerStage && event == GEvent.CLICKED) {
    int newX = floor(printerMaxX/2);
    int newY = floor(printerMaxY/2);
    int newZ = floor(printerMaxZ/2);
    targetX = newX;
    targetY = newY;
    targetZ = newZ;    
    port.write("G0 " + newX + " " + newY + " " + newZ + "\n");
  } else if (button == saveToLogfiles && event == GEvent.CLICKED ) {
    saveStrings("log.txt", log.toArray(new String[log.size()]));
    saveStrings("positions.txt", positionLog.toArray(new String[positionLog.size()]));
    println("logfiles saved");
  } else if (button == calibrateZero && event == GEvent.CLICKED) {
    int reply = G4P.selectOption(this, "Disconnect the stage from the motors, then set it to minimum X and Y manually. Press OK to perform calibration.", "Calibration", G4P.INFO, G4P.OK_CANCEL );
    if (reply == G4P.OK) {
      port.write("T10\n");
      delay(500);
      port.write("M115\n");
      G4P.showMessage(this, "Zero calibration performed", "Calibration", G4P.INFO);
    }
  } else if (button == calibrateMax && event == GEvent.CLICKED) {
    int reply = G4P.selectOption(this, "Disconnect the stage from the motors, then set it to maximum X and Y manually. Press OK to perform calibration.", "Calibration", G4P.INFO, G4P.OK_CANCEL );
    if (reply == G4P.OK) {
      port.write("T11\n");
      delay(500);
      port.write("M115\n");
      G4P.showMessage(this, "Maximum calibration performed", "Calibration", G4P.INFO);
    }
  }
}