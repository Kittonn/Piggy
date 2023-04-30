#include <Servo.h>

#define SERVOTEN_PIN 17
#define SERVOFIVE_PIN 16
#define SERVOTWO_PIN 5
#define SERVOONE_PIN 18

Servo servoTen;
Servo servoFive;
Servo servoTwo;
Servo servoOne;

void setup() {
  Serial.begin(9600);
  // attach the servos to the pins
  servoTen.attach(SERVOTEN_PIN);
  servoFive.attach(SERVOFIVE_PIN);
  servoTwo.attach(SERVOTWO_PIN);
  servoOne.attach(SERVOONE_PIN);
}

void loop() {
  drop_tenCoin();
  drop_fiveCoin();
  drop_twoCoin();
  drop_oneCoin();
}

void drop_tenCoin()
{
  servoFive.write(140);
  delay(1000);
  servoFive.write(60);
  delay(500);
}
void drop_fiveCoin()
{
  servoTen.write(50);
  delay(1000);
  servoTen.write(130);
  delay(500);
}
void drop_twoCoin()
{
  servoTwo.write(40);
  delay(1000);
  servoTwo.write(140);
  delay(500);
}
void drop_oneCoin()
{
  servoOne.write(50);
  delay(1000);
  servoOne.write(140);
  delay(500);
}
