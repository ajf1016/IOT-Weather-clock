#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "AJFKAR";
const char* password =  "AJFKAR1016";
const String apiKey = "818e3dc850b8488b1cabebea7286b59f";

String url = "https://api.openweathermap.org/data/2.5/weather?q=chelari&appid=" + apiKey;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print(">>>>Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print(">>>>Connected to ");
  Serial.println(ssid);
  Serial.print(">>>>IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
client.setInsecure();
    
    HTTPClient https;
    Serial.println(">>>>Requesting " + url);
    if (https.begin(client, url)) {
      int httpCode = https.GET();
      Serial.println(">>>> Response code: " + String(httpCode));
      if (httpCode > 0) {
        Serial.println(https.getString());
      }
      https.end();
    } else {
      Serial.printf(">>>>[HTTPS] Unable to connect\n");
    }
  }
  delay(5000);
}