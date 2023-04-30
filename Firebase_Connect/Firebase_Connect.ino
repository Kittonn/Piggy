#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include <ESPDateTime.h>

// Config Wifi
#define WIFI_SSID "Ohm"
#define WIFI_PASSWORD "0877444232"

#define API_KEY "AIzaSyATMWWYyKi-G_v-r8B1IQW7UgKOEBZ9lbA"
#define DATABASE_URL "https://piggypal-679da-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

FirebaseJson json;
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int money = 1000;

void setup() {
  Serial.begin(9600);
  WiFi.disconnect();
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
  setupDateTime();
}

void loop(){
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    writeData(money);
    writeData_transaction();
    readData();
  }
}

void writeData_transaction() {
  json.set("/type", "withdraw");
  json.set("/amount", 70);
  json.set("/time", DateTime.now());
  String parentPath = "transaction/" + String(DateTime.now());
  if (Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json)){
    Serial.println(parentPath);
    Serial.println("Set JSON PASSED");
  }
}
void writeData(int amount) {
  if (Firebase.RTDB.setInt(&fbdo, "money", amount)) {
    Serial.println("Set PASSED");
  }
}

void readData() {
  if (Firebase.RTDB.getInt(&fbdo, "withdraw/amount")) {
    if (fbdo.dataType() == "int"){
      int amountWithdraw = fbdo.intData();
      Serial.print("Withdraw => ");
      Serial.println(amountWithdraw);
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

void setupDateTime() {
  DateTime.setTimeZone("KRAT");
  DateTime.begin(10);
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
  }
}
