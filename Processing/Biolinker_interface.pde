import g4p_controls.*;
import processing.serial.*;

// communication with arduino
// code from http://www.instructables.com/id/Arduino-to-Processing-Serial-Communication-withou
int endline = 10; // ASCII code for linefeed
String serial;
Serial port;

// GUI - all coordinates in mm
// printer coordinates
int printerX = 0;
int printerY = 0;
int printerZ = 0;
int printerMaxX = 100;
int printerMaxY = 100;
int printerMaxZ = 50;
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

PFont font;
int time = 0;
int lastTime = 0;

void setup() {
  port = new Serial(this, Serial.list()[0], 9600);
  port.clear();
  serial = port.readStringUntil(endline);
  serial = null;
  
  printerX = 60;
  printerY = 45;
  printerZ = 20;
  
  targetX = 60;
  targetY = 45;
  targetZ = 20;
  
  size(1200, 900);
  stroke(0);
  background(255);
  font = createFont("Arial", 14, true);
  textFont(font, 30);
  fill(0);
  text("Biolinker control software", 10, 30);
  textFont(font, 14);  
  
  text("X", 10, 100);
  inputX = new GTextField(this, 30, 86, 100, 20);
  text("Y", 10, 130);
  inputY = new GTextField(this, 30, 116, 100, 20);
  text("Z", 10, 160);
  inputZ = new GTextField(this, 30, 146, 100, 20);
  
  moveWithExtrusion = new GCheckbox(this, 30, 176, 100, 20, "Extrude ink");
  inkSelector = new GToggleGroup();
  ink1 = new GOption(this, 30, 206, 100, 20, "Ink 1");
  ink1.setSelected(true);
  ink2 = new GOption(this, 30, 236, 100, 20, "Ink 2");
  inkSelector.addControl(ink1);
  inkSelector.addControl(ink2);
  targetButton = new GButton(this, 30, 266, 100, 20, "Move");
  targetButton.fireAllEvents(true);
  drawScreen();
}

void draw() {
  
  while (port.available() > 0) {
    serial = port.readStringUntil(endline);
  }
  
  if (serial != null) {
    String[] commands = split(serial, ' ');
    println(commands[0]);
    println(commands[1]);
  }
  
  time = millis();
  if (time-lastTime > 500) {
    drawScreen();
    lastTime = time;
  }
}

public void drawScreen() {
  // clearing rectangle
  stroke(255);
  rect(xyScreenX-100, xyScreenY-200, xyScreenWidth+400, xyScreenHeight+400);
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

public void handleTextEvents(GEditableTextControl textcontrol, GEvent event) { /* code */ }

public void handleButtonEvents(GButton button, GEvent event) {
  
  // set new target
  if (button == targetButton && event == GEvent.CLICKED ) {
    int newX = printerX;
    int newY = printerY;
    int newZ = printerZ;
    try {
      newX = Integer.parseInt(inputX.getText());
      newY = Integer.parseInt(inputY.getText());
      newZ = Integer.parseInt(inputZ.getText());
    } catch(NumberFormatException e) {
      G4P.showMessage(this, "Please enter a valid integer value", "Wrong input type", G4P.ERROR);
    }
    if (newX > printerMaxX) {
      newX = printerMaxX;
      println("X out of bounds");
    }
    if (newX < 0) {
      newX = 0;
      println("X out of bounds");      
    }
    if (newY > printerMaxY) {
      newY = printerMaxY;
      println("Y out of bounds");      
    }
    if (newY < 0) {
      newY = 0;
      println("Y out of bounds");      
    }
    if (newZ > printerMaxZ) {
      newZ = printerMaxZ;
      println("Z out of bounds");      
    }
    if (newZ < 0) {
      newZ = 0;
      println("Z out of bounds");      
    }
    targetX = newX;
    targetY = newY;
    targetZ = newZ;
  }
}