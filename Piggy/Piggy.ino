#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <ESP32Servo.h>

#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Config Wifi
#define WIFI_SSID "Ohm"
#define WIFI_PASSWORD "0877444232"

// Config Firebase
#define API_KEY "AIzaSyATMWWYyKi-G_v-r8B1IQW7UgKOEBZ9lbA"
#define DATABASE_URL "https://piggypal-679da-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Data
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

// Withdraw
int withdraw_state;
#define SERVOTEN_PIN 17
#define SERVOFIVE_PIN 16
#define SERVOTWO_PIN 5
#define SERVOONE_PIN 18
Servo servoTen;
Servo servoFive;
Servo servoTwo;
Servo servoOne;

// IR Sensor
#define PIN_ten 4
#define PIN_five 36
#define PIN_two 35
#define PIN_one 34

// Coin count
int money; // Money In piggy
int tenCoin;
int fiveCoin;
int twoCoin;
int oneCoin;
int deposit_state = 0;
int lastDepositTime;

int allCoin;

// Debug
int lastTime;
int prevTime = 1000;

// SetUp KeyPad and OLED
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
int amountWithdraw = 0;
int digits = 1;

int lastKeyPadTime;
byte loopcheck[] = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  // WiFi setup
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  setUpFirebase();
  pinMode(PIN_ten, INPUT);
  pinMode(PIN_five, INPUT);
  pinMode(PIN_two, INPUT);
  pinMode(PIN_one, INPUT);
  setUpKeyPad();
  setUpServo();
}

void loop(){
  if(millis() - lastDepositTime > 500)
  if(!deposit_state){
    if(analogRead(PIN_ten) > 2000){
      tenCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
      calculateMoney();
    }else if(digitalRead(PIN_five)){
      fiveCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
      calculateMoney();
    }else if(digitalRead(PIN_two)){
      twoCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
      calculateMoney();
    }else if(digitalRead(PIN_one)){
      oneCoin++;
      deposit_state = 1; 
      lastDepositTime = millis();
      calculateMoney();
    }
  }
  if(!digitalRead(PIN_ten) && !digitalRead(PIN_five) && !digitalRead(PIN_two) && !digitalRead(PIN_one)){
    deposit_state = 0;
  }
  if(millis() - lastTime > prevTime || lastTime == 0){
    lastTime = millis();
    Serial.print("10 coin = ");
    Serial.println(tenCoin);
    Serial.print("5 coin = ");
    Serial.println(fiveCoin);
    Serial.print("2 coin = ");
    Serial.println(twoCoin);
    Serial.print("1 coin = ");
    Serial.println(oneCoin);
    Serial.println(digitalRead(PIN_ten));
    Serial.println(digitalRead(PIN_five));
    Serial.println(digitalRead(PIN_two));
    Serial.println(digitalRead(PIN_one));
    Serial.println("===================");
    Serial.print("digital read 10 = ");
    Serial.print(digitalRead(PIN_ten));
    Serial.print(", analog read 10 = ");
    Serial.println(analogRead(PIN_ten));
  }
  // if (signupOK && (millis() - sendDataPrevMillis > 500 || sendDataPrevMillis == 0)){
  //   sendDataPrevMillis = millis();
  //   writeData(money);
  // }
  // if(millis() - lastKeyPadTime > 5){
  //   lastKeyPadTime = millis();
  //   Numpad();
  //   OLEDdisplay();
  // }
  //readData();
}

void setUpIR(){
  pinMode(PIN_ten, INPUT);
  pinMode(PIN_five, INPUT);
  pinMode(PIN_two, INPUT);
  pinMode(PIN_one, INPUT);
}

void withDraw(int amount){
  if(amount > 0){
    withdraw_state = 1;
    while(withdraw_state){
      while(amount >= 10 && tenCoin){
        amount -= 10;
        tenCoin--;
        drop_tenCoin();
      }
      while(amount >= 5 && fiveCoin){
        amount -= 5;
        fiveCoin--;
        drop_fiveCoin();
      }
      while(amount >= 2 && twoCoin){
        amount -= 2;
        twoCoin--;
        drop_twoCoin();
      }
      while(amount >= 1 && oneCoin){
        amount -= 1;
        oneCoin--;
        drop_oneCoin();
      }
      calculateMoney();
      //Firebase.RTDB.setInt(&fbdo, "money", money);
      withdraw_state = 0;
      amountWithdraw = 0;
      if (Firebase.RTDB.setInt(&fbdo, "withdraw/amount", 0)) {
        Serial.println("WithDraw Success");
      }
    }
  }
}

void writeData(int amount) {
  Firebase.RTDB.setInt(&fbdo, "money", amount);
  Firebase.RTDB.setInt(&fbdo, "coin/allcoin", allCoin);
  Firebase.RTDB.setInt(&fbdo, "coin/tencoin", tenCoin);
  Firebase.RTDB.setInt(&fbdo, "coin/fivecoin", fiveCoin);
  Firebase.RTDB.setInt(&fbdo, "coin/twocoin", twoCoin);
  Firebase.RTDB.setInt(&fbdo, "coin/onecoin", oneCoin);
}

//flutter withdraw
void readData() {
  if (Firebase.RTDB.getInt(&fbdo, "withdraw/amount")) {
    if (fbdo.dataType() == "int"){
      int amount = fbdo.intData();
      if(amount > 0){
        amountWithdraw = amount;
        withdraw_state = 1;
        digits = 1;
        OLEDdisplay();
        withDraw(amountWithdraw);
      }
    }
  }
}

void setUpFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if(Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signUp OK");
    signupOK = true;
  }else{
    Serial.println("Failed");
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void setUpKeyPad() {
  // OLED SetUp
  display.begin(i2c_Address, true);
  // KeyPad SetUp
  pinMode(OUT4, INPUT);
  pinMode(OUT3, INPUT);
  pinMode(OUT2, INPUT);
  pinMode(OUT1, INPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN1, OUTPUT);
}

void OLEDdisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  if(withdraw_state){
    display.setCursor(1, 25);
    display.println("Process...");
  }else if(amountWithdraw == 0){
    display.setCursor(1, 25);
    display.print("Balance:");
    display.println(money);
  }
  else{
    display.setCursor(110-(digits * 9), 25);
    display.println(amountWithdraw);
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
        if(amountWithdraw == 0){
          Serial.println("cancel");
        }else{
          amountWithdraw /= 10;
          if(digits > 1){
            digits--;
          }
        }
      }
      if(i == 11 && amountWithdraw > 0){
        Serial.println("Withdraw");
        withdraw_state = 1;
        digits = 1;
        OLEDdisplay();
        withDraw(amountWithdraw);
      }
      if(amountWithdraw < 1000 && i < 10){
        if(amountWithdraw > 0){
          amountWithdraw *= 10;
          digits++;
        }
        //amount = amount * pow(10,digits);
        amountWithdraw += i;
      }else if(i < 10){
        Serial.println("not enough money");
      }
      if(i < 11 && amountWithdraw > 0){
        Serial.println(amountWithdraw);
      }
      buttonState = HIGH;
    }
  }
}

void deposit(){
  if(millis() - lastDepositTime > 500)
  if(!deposit_state){
    if(analogRead(PIN_ten) > 2000){
      tenCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
      calculateMoney();
    }else if(digitalRead(PIN_five)){
      fiveCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
      calculateMoney();
    }else if(digitalRead(PIN_two)){
      twoCoin++;
      deposit_state = 1;
      lastDepositTime = millis();
      calculateMoney();
    }else if(digitalRead(PIN_one)){
      oneCoin++;
      deposit_state = 1; 
      lastDepositTime = millis();
      calculateMoney();
    }
  }
  if(!digitalRead(PIN_ten) && !digitalRead(PIN_five) && !digitalRead(PIN_two) && !digitalRead(PIN_one)){
    deposit_state = 0;
  }
  if(millis() - lastTime > prevTime || lastTime == 0){
    lastTime = millis();
    Serial.print("10 coin = ");
    Serial.println(tenCoin);
    Serial.print("5 coin = ");
    Serial.println(fiveCoin);
    Serial.print("2 coin = ");
    Serial.println(twoCoin);
    Serial.print("1 coin = ");
    Serial.println(oneCoin);
    Serial.println(digitalRead(PIN_ten));
    Serial.println(digitalRead(PIN_five));
    Serial.println(digitalRead(PIN_two));
    Serial.println(digitalRead(PIN_one));
    Serial.println("===================");
    Serial.print("digital read 10 = ");
    Serial.print(digitalRead(PIN_ten));
    Serial.print(", analog read 10 = ");
    Serial.println(analogRead(PIN_ten));
  }
}

void calculateMoney() {
  money = (tenCoin*10) + (fiveCoin*5) + (twoCoin*2) + oneCoin;
  allCoin = tenCoin + fiveCoin + twoCoin + oneCoin;
}

void setUpServo() {
  servoTen.attach(SERVOTEN_PIN);
  servoFive.attach(SERVOFIVE_PIN);
  servoTwo.attach(SERVOTWO_PIN);
  servoOne.attach(SERVOONE_PIN);
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
  servoTwo.write(50);
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