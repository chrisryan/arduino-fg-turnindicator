/*
 * Arduino Turn Coordinator for Flight Gear
 *
 * This program will communicate with FlightGear with the Virtual Serial port
 * Reading the values sent from the program to display the Turn Coordinator
 * using a servo for the miniature airplane and an led for the failure indicator.
 * This program also supports some additional components that allow for adjusting
 * the server level and standard turn offset.
 */

#include <Servo.h>

const int failurePin = 13; // Digital
const int servoPin = 9; // Digital
const int levelPin = A0; // Analog
const int turnPin = A1; // Analog

Servo turnIndicator;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
boolean inop = false; // should instrument indicate failure
double turn = 0.0; // the rate of turn value

void setup() {
  pinMode(failurePin, OUTPUT);
  turnIndicator.attach(servoPin);

  test();

  inputString.reserve(200);
  Serial.begin(9600);
}

void loop() {
  if (stringComplete) {
    parseInput();
    setFailure(inop);
    turnTo(turn);

    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
 * Run through some of the functions so that they can
 * be verified and any adjustments can be made with the pots.
 */
void test() {
  setFailure(true);

  double testVals[7] = {0.0, 1.0, -1.0, 2.5, -2.5, -0.5, 0.0};
  for (int i = 0; i < 7; i++) {
    turnTo(testVals[i]);
    delay(1000);
  }
}

void setFailure(bool enable) {
  digitalWrite(failurePin, (enable ? HIGH : LOW));
}

void turnTo(double rate) {
  int angle = getCenter() - (getTurnOffset() * rate);
  turnIndicator.write(constrain(angle, 0, 179));
}

int getCenter() {
  return map(analogRead(levelPin), 0, 1023, 0, 179);
}

int getTurnOffset() {
  return map(analogRead(turnPin), 0, 1023, 0, 89);
}

void parseInput() {
  inop = false;
  turn = 0.0;

  int split = inputString.indexOf(',');
  String substr = inputString.substring(0, split);
  char buf[20];
  substr.toCharArray(buf, 20);
  turn = atof(buf);

  if (inputString[split + 1] == '0') {
    inop = true;
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      return;
    }
  }
}
