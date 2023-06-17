// counts the number of pull ups that a person does. reset numPullups: after 1 minute. reset maxPullups: after 7 hours.

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 10);
const int backLight = 13; // pin 13 will control the backlight

const int TRIGGER_PIN = 7; // Trigger Pin of Ultrasonic Sensor
const int ECHO_PIN = 6; // Echo Pin of Ultrasonic Sensor

unsigned long lastTimeUltrasonicTrigger = millis();
unsigned long ultrasonicTriggerDelay = 100;

unsigned long lastTimePullup = millis();

//bool bodyExists = false;
int numPullups = 0;
int maxPullups = 0;

void setup() {
  Serial.begin(9600);
  
  lcd.begin(16,2);
  pinMode(backLight, OUTPUT);
  digitalWrite(backLight, LOW); // turn backlight off. (Replace 'LOW' with 'HIGH' to turn it on)
  lcd.print("setup");
  lcd.setCursor(0,1);
  lcd.print("Pullups:0");

  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  
  lcd.setCursor(7,0);
  lcd.print("Max:");
  lcd.print(maxPullups);
}

void recordPullup() {
  numPullups++;
  lcd.setCursor(8,1);
  lcd.print(numPullups);
  if (numPullups > maxPullups) {
    maxPullups = numPullups;
    lcd.setCursor(11,0);
    lcd.print(maxPullups);
  }
}

void triggerUltrasonicSensor() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
}

double getUltrasonicDistance() {
  double durationMicros = pulseIn(ECHO_PIN, HIGH); // microseconds is casted to double
  double distance = durationMicros / 58.0; // cm
  // distance = duration * speed
  // 340 m/s --> 0.034 cm/µs
  // duration * (0.034 / 2)
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return distance;
}

void loop() {
  unsigned long timeNow = millis();
  double distanceToBody;

  if (timeNow - lastTimeUltrasonicTrigger > ultrasonicTriggerDelay) {
    lastTimeUltrasonicTrigger += ultrasonicTriggerDelay;
    triggerUltrasonicSensor();
    lcd.setCursor(0,0);
    distanceToBody = getUltrasonicDistance();
    int distanceInt = int(round(distanceToBody));
    lcd.print("cm:");
    lcd.print(distanceInt);
    lcd.print(" ");
  }

  if (distanceToBody < 40.0) { // body pulled itself up (distanceToBody = distance between sensor on wall and forehead)
    digitalWrite(backLight, HIGH);
    recordPullup();
    delay(1000); // 1 second
    lastTimePullup = millis();
  }

  timeNow = millis();

  if (timeNow - lastTimePullup > (60000)) { // 60 seconds
    // save energy
    digitalWrite(backLight, LOW);
    numPullups = 0;
  }

  // if more than 7 hrs pass (meaning people are asleep and its the next day), reset the pullup count. 
  if (timeNow - lastTimePullup > (25200000)) { // 7 hours = 25200000 ms
    // restart pull up count
    maxPullups = 0;
  }
}
