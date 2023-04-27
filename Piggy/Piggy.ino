#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <ESP32Servo.h>

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
int money = 1000;

#define SERVO_PIN 23
Servo servo;

int withdraw_state;

// Coin count
int tenCoin = 10;
int fiveCoin = 10;
int twoCoin = 10;
int oneCoin = 10;

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
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
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    writeData(money);
    Serial.print("Withdraw -> ");
    Serial.println(readData());
    withDraw(readData());
  }
}

void withDraw(int amount){
  if(amount > 0){
    withdraw_state = 1;
    while(withdraw_state){
      while(amount > 10 && tenCoin){
        amount -= 10;
        tenCoin--;
      }
      while(amount > 5 && fiveCoin){
        amount -= 5;
        fiveCoin--;
      }
      while(amount > 2 && twoCoin){
        amount -= 2;
        twoCoin--;
      }
      while(amount > 1 && oneCoin){
        amount -= 1;
        oneCoin--;
      }
      withdraw_state = 0;
      if (Firebase.RTDB.setInt(&fbdo, "withdraw/amount", 0)) {
        Serial.println("Set PASSED");
      }
    }
  }
}

void writeData(int amount) {
  if (Firebase.RTDB.setInt(&fbdo, "money", amount)) {
    Serial.println("Set PASSED");
  }
}

int readData() {
  if (Firebase.RTDB.getInt(&fbdo, "withdraw/amount")) {
    if (fbdo.dataType() == "int"){
      int amountWithdraw = fbdo.intData();
      return amountWithdraw;
    }
  }
}
void wifisetUp() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
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

