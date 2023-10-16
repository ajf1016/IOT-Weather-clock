#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// LCD initialization
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// LED Pin
const int ledPin = 2; // GPIO 2

// Wi-Fi credentials
const char* ssid = "xjuog";
const char* password = "11111111";

// OpenWeatherMap API settings
const String apiKey = "818e3dc850b8488b1cabebea7286b59f";
const String city = "Jalandhar";

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize LED pin as an output
  pinMode(ledPin, OUTPUT);

  // Initialize LCD display
  lcd.begin(16, 2); // Set the LCD dimensions (16 columns, 2 rows)

  // Connect to Wi-Fi
  connectToWiFi();

  // Fetch and display weather information
  fetchAndDisplayWeather();
}

void loop() {
  // Your main loop code here
}

void connectToWiFi() {
  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void fetchAndDisplayWeather() {
  // Use the WiFiClientSecure class for secure HTTP connection (HTTPS)
  WiFiClientSecure client;

  Serial.print("Connecting to OpenWeatherMap...");

  if (client.connect("api.openweathermap.org", 443)) {
    Serial.println("Connected");

    // Your API endpoint and parameters
    String url = "/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

    // Send HTTP GET request
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: api.openweathermap.org\r\n" +
                 "Connection: close\r\n\r\n");

    // Wait for the response
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\r');
        if (line == "\n") {
          // Headers received, start reading the JSON data
          String payload = client.readStringUntil('\0');

          // Parse JSON data
          DynamicJsonDocument doc(1024);
          deserializeJson(doc, payload);

          // Extract and display weather data on LCD
          float temperature = doc["main"]["temp"];
          String weatherDescription = doc["weather"][0]["description"];
          String weatherData = "Temp: " + String(temperature, 1) + "C\n" + "Weather: " + weatherDescription;
          displayWeatherData(weatherData);

          // Turn the LED on
          digitalWrite(ledPin, HIGH);
          delay(1000);

          // Turn the LED off
          digitalWrite(ledPin, LOW);
          delay(1000);

          break;
        }
      }
    }
    client.stop();
  } else {
    Serial.println("Connection failed");
  }
}

void displayWeatherData(String data) {
  // Clear the LCD display
  lcd.clear();

  // Set cursor to the beginning of the first line
  lcd.setCursor(0, 0);

  // Print weather data on the LCD
  lcd.print("Weather Data:");
  lcd.setCursor(0, 1);
  lcd.print(data);
}
