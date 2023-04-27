#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Config Wifi
#define WIFI_SSID "Ohm"
#define WIFI_PASSWORD "0877444232"

#define API_KEY "AIzaSyATMWWYyKi-G_v-r8B1IQW7UgKOEBZ9lbA"
#define DATABASE_URL "https://piggypal-679da-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int money = 1000;

void setup() {
  Serial.begin(9600);
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
    readData();
  }
}

void writeData(int amount) {
  if (Firebase.RTDB.setInt(&fbdo, "money", amount)) {
    Serial.println("Set PASSED");
  }
}

void readData() {
  if (Firebase.RTDB.getInt(&fbdo, "withdraw")) {
    if (fbdo.dataType() == "int"){
      Serial.print("Withdraw => ");
      Serial.println(fbdo.intData());
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

