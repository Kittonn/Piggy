#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define WIFI_SSID "Ohm"
#define WIFI_PASSWORD "0877444232"

#define serverName "https://firebase.google.com/docs/database/rest/retrieve-data"
int i = 0;
WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connect : ");
  Serial.println(WiFi.localIP());
}
 
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    String reqUrl = "https://piggypal-11dc6-default-rtdb.asia-southeast1.firebasedatabase.app";
    // http.begin(client, reqUrl);
    // int httpCode = http.GET();
    // Serial.println(httpCode);
    // Serial.println(http.getString());
    // http.end();

    if (!client.connect(reqUrl, 443)) {
      Serial.println("connect Failed");
    } else {
      Serial.println("connect Done");
      String url = "/Money.json";
      Serial.print("Request URL : ");
      Serial.println(url);

      client.print(String("GET ") + reqUrl + url);
    }
  }
  delay(500);
}