#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define OUT4 12 // 12 For esp32 
#define OUT3 14 // 14  For esp32
#define OUT2 27 // 27  For esp32
#define OUT1 26 // 26  For esp32
#define IN3 25 // 25 For esp32
#define IN2 33 // 33  For esp32
#define IN1 32 // 32  For esp32

int keypad[] = {IN1, IN2, IN3, OUT1, OUT2, OUT3, OUT4};
int number[] = {66, 9, 10, 12, 17, 18, 20, 33, 34, 36, 65, 68}; // 65 DEL, 68, ENTER
byte x = B00000000;
int buttonState = LOW;
int count = 0;
int amount = 0;
int digits = 1;
int Withdraw = 0; //Withdraw State
int withdrawTime = 0;
int processTime = 5000;

int lastTime; //For keypad
int delayTime = 10;
byte loopcheck[] = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  display.begin(i2c_Address, true);
  delay(1000);
  pinMode(OUT4, INPUT);
  pinMode(OUT3, INPUT);
  pinMode(OUT2, INPUT);
  pinMode(OUT1, INPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN1, OUTPUT);
}
void loop() {
  if(millis() - lastTime > delayTime){
    Numpad();
    OLEDdisplay();
    lastTime = millis();
  }
}

void OLEDdisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  if(Withdraw){
    display.setCursor(1, 25);
    display.println("Process...");
    if(millis() - withdrawTime > processTime){
      Withdraw = 0;
    }
  }else{
    display.setCursor(110-(digits * 9), 25);
    display.println(amount);
  }

  display.display();
}

void Numpad() {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(keypad[count],HIGH);
  if(count == 0){
  digitalWrite(IN2,LOW);
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
          if(digits > 1){
            digits--;
          }
        }
      }
      if(i == 11 && amount > 0){
        Serial.println("Withdraw");
        withdrawTime = millis();
        Withdraw = 1;
        amount = 0;
        digits = 1;
      }
      if(amount < 1000 && i < 10){
        if(amount > 0){
          amount *= 10;
          digits++;
        }
        //amount = amount * pow(10,digits);
        amount += i;
      }else if(i < 10){
        Serial.println("not enough money");
      }
      if(i < 11 && amount > 0){
        Serial.print("money = ");
        Serial.println(amount);
      }
      buttonState = HIGH;
    }
  }
}