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

String inputString = ""; // a string to hold incoming data
boolean stringComplete = false; // whether the string is complete
boolean inop = true; // should instrument indicate failure
double turn = 0.0; // the rate of turn value

/*
 * Setup
 * Initial our LED as output
 * Create our servo controller
 * Run the test routine
 * Start our serial communication
 */
void setup() {
  pinMode(failurePin, OUTPUT);
  turnIndicator.attach(servoPin);

  test();

  inputString.reserve(200);
  Serial.begin(9600);
}

/*
 * Main Loop
 * Check if we have a new string to parse then update our
 * outputs accordingly. Updating on each loop allows for
 * the trim pots to be adjusted and see the results
 * immediately.
 */
void loop() {
  if (stringComplete) {
    parseInput();
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  setFailure(inop);
  turnTo(turn);
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

/*
 * Activate or deactivate the failure indicator
 * In this case it is an led
 */
void setFailure(bool enable) {
  digitalWrite(failurePin, (enable ? HIGH : LOW));
}

/*
 * Given a rate of turn with 0 being no turn and 1.0 and -1.0 being
 * a standard rate turn to the right and left respectively, calculate
 * the angle to set the servo motor to.
 */
void turnTo(double rate) {
  int angle = getCenter() - (getTurnOffset() * rate);
  turnIndicator.write(constrain(angle, 0, 179));
}

/*
 * Determine the center position for the servo by reading the trim pot
 */
int getCenter() {
  return map(analogRead(levelPin), 0, 1023, 0, 179);
}

/*
 * Determine the angle of a standard rate turn by reading the trim pot
 */
int getTurnOffset() {
  return map(analogRead(turnPin), 0, 1023, 0, 89);
}

/*
 * Parse the serial input string and set the values we need.
 * The input string is determined by the arduinofgturnindicator.xml
 * file. The expected format is: %0.3d,%d\n
 * with the first parameter the rate of turn
 * and the second a bool 0 or 1 if the indicator is functioning
 */
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

/*
 * This function is called when serial data is available in between calls to loop.
 * This keeps us from having to have this functionality in the loop function.
 */
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
