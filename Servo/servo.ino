#include <ESP32Servo.h>

const uint8_t SERVO_PIN = 32;
const uint8_t IR_PIN = 34;
const uint8_t BTN_PIN = 33;

Servo servo;

int current_state = 1;
int last_state = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(IR_PIN, INPUT);
  servo.attach(SERVO_PIN);
}

void loop()
{
  current_state = digitalRead(IR_PIN);
  int button_state = digitalRead(BTN_PIN);

  Serial.println(current_state);

  if (button_state == LOW)
  {
    // drop_coin(50, 180); // 1 บาท
    drop_coin(50, 140); // 2 บาท
    // drop_coin(50, 140); // 5 บาท
    // drop_coin(130,50); // 10 บาท
  }
}

void drop_coin(int start_angle, int stop_angle)
{
  servo.write(start_angle);
  delay(1000);
  servo.write(stop_angle);
  delay(500);
}
