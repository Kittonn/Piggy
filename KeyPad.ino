#include <math.h>
#define LED D4
#define OUT4 D0
#define OUT3 D1
#define OUT2 D2
#define OUT1 D3
#define IN3 D5
#define IN2 D6
#define IN1 D7

int keypad[] = {IN1, IN2, IN3, OUT1, OUT2, OUT3, OUT4};
int number[] = {66, 9, 10, 12, 17, 18, 20, 33, 34, 36, 65, 68}; // 65 DEL, 68, ENTER
byte x = B00000000;
int buttonState = LOW;
int count = 0;
int amount = 0;
int digits = 0;
int current = 0;

byte loopcheck[] = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(OUT4, INPUT);
  pinMode(OUT3, INPUT);
  pinMode(OUT2, INPUT);
  pinMode(OUT1, INPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN1, OUTPUT);
}
void loop() {
  digitalWrite(LED,LOW);
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(keypad[count],HIGH);
  if(count == 0){
    if(loopcheck[0] <= 4 && loopcheck[1] <= 4 && loopcheck[2] <= 4){
      buttonState = LOW;
    }
  }
  count += 1;
  for(int i = 0; i < 7; i++){
    bitWrite(x, i, digitalRead(keypad[i]));
    loopcheck[count-1] = x; 
  }
  if(count > 2){
    count = 0;
  }
  for(int i = 0; i < 12; i++){
    if(x == number[i] && !buttonState){
      if(i == 10){
        if(amount == 0){
          Serial.println("cancel");
        }else{
          amount /= 10;
        }
      }
      if(i == 11 && amount > 0){
        Serial.println("Withdraw");
      }
      if(amount < 1000 && i < 10){
        amount = amount * pow(10,digits);
        amount += i;
        if(digits < 1){
          digits++;
        }
      }else if(i < 10){
        Serial.println("not enough money");
      }
      if(i < 11 && amount > 0){
        Serial.print("money = ");
        Serial.println(amount);
        //Serial.println(i);
      }
      buttonState = HIGH;
    }
  }
  delay(10);
}