#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Config Wifi
#define WIFI_SSID "Ohm"
#define WIFI_PASSWORD "0877444232"

// Config Firebase
#define FIREBASE_HOST "piggyapp-de095-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "vNTWIn7zH8eq1tGDKZWbHe3vxpkeV77UIKiFzdeA"

#define API_KEY "AIzaSyDcwiX5CQcMsAbpk_MXjeH2YyicqldmxOI"
#define USER_EMAIL "kantinan@gmail.com"
#define USER_PASSWORD "123456"

FirebaseData fbdo;

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connect : ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
 
void loop() {
  if(Firebase.ready()){
    Serial.println("connected");
    Serial.println(Firebase.get(fbdo, "/money"));
    Serial.println(fbdo.to<int>());
  }
  delay(500);
}
