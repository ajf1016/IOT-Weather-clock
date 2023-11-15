#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

String dateString;
String timeString;
String receivedData;

String temperatures[8] = {};
String humidity[8] = {};
String feelsLike[8] = {};

// TEMP
String d1T;
String d2T;
String d3T;
String d4T;
// HUMIDITY
String d1H;
String d2H;
String d3H;
String d4H;
// FEELS LIKE D1
String d1D;
String d1N;
String d1E;
String d1M;
// FEELS LIKE D2
String d2D;
String d2N;
String d2E;
String d2M;
// FEELS LIKE D3
String d3D;
String d3N;
String d3E;
String d3M;
// FEELS LIKE D4
String d4D;
String d4N;
String d4E;
String d4M;

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();

  for (int i = 0; i < 8; i++) {
    temperatures[i] = "0";
    humidity[i] = "0";
    feelsLike[i] = "0";
  }

  // Display initial messages
  lcd.setCursor(0, 0);
  lcd.print("IOT WeatherClock");
  lcd.setCursor(0, 1);
  lcd.print("Connecting WIFI....");
  delay(2000);
}

void loop() {
  if (Serial.available()) {
    receivedData = Serial.readStringUntil('\n');
    // receivedData = "Time:2023-11-15 00:10:38Weather:D1:26C,09%,Day=26C,Night=18C,Eve=22C,Mrng=15C|D2:26C,09%,Day=25C,Night=17C,Eve=23C,Mrng=15C|D3:26C,07%,Day=25C,Night=16C,Eve=21C,Mrng=14C|D4:26C,06%,Day=24C,Night=16C,Eve=22C,Mrng=14C|";
    dateString = receivedData.substring(5, 15);
    timeString = receivedData.substring(16, 24);

    Serial.println(receivedData);

    // // Extract temperature, humidity, and feels_like data for each day
    // for (int i = 0; i < 8; i++) {
    //   String tempSubstring = receivedData.substring(47 + (35 * i), 61 + (35 * i));
    //   temperatures[i] = tempSubstring.substring(7, 11);
    //   humidity[i] = tempSubstring.substring(24, 26);
    //   feelsLike[i] = tempSubstring.substring(44, 48);
    // }
    int gap = 63;
    d1T = receivedData.substring(35 + gap, 37 + gap);
    d1H = receivedData.substring(39 + gap, 41 + gap);
    d1D = receivedData.substring(47 + gap, 49 + gap);
    d1N = receivedData.substring(57 + gap, 59 + gap);
    d1E = receivedData.substring(65 + gap, 66 + gap);
    d1M = receivedData.substring(74 + gap, 76 + gap);

    d2T = receivedData.substring(81 + gap, 83 + gap);
    d2H = receivedData.substring(85 + gap, 87 + gap);
    d2D = receivedData.substring(93 + gap, 95 + gap);
    d2N = receivedData.substring(103 + gap, 105 + gap);
    d2E = receivedData.substring(111 + gap, 113 + gap);
    d2M = receivedData.substring(120 + gap, 122 + gap);

    d3T = receivedData.substring(127 + gap, 129 + gap);
    d3H = receivedData.substring(131 + gap, 133 + gap);
    d3D = receivedData.substring(139 + gap, 141 + gap);
    d3N = receivedData.substring(149 + gap, 151 + gap);
    d3E = receivedData.substring(157 + gap, 159 + gap);
    d3M = receivedData.substring(166 + gap, 168 + gap);

    d4T = receivedData.substring(173 + gap, 175 + gap);
    d4H = receivedData.substring(177 + gap, 179 + gap);
    d4D = receivedData.substring(185 + gap, 187 + gap);
    d4N = receivedData.substring(195 + gap, 197 + gap);
    d4E = receivedData.substring(203 + gap, 205 + gap);
    d4M = receivedData.substring(212 + gap, 214 + gap);

  }

  // Display time and date
  displayTime(dateString, timeString);

  // Display DHT sensor data
  displayDHT();

  // Display weather data for days 1 to 4
  displayWeather();
  

  delay(1000); // Update every second
}

void displayWeather() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DAY1:");
  lcd.print(d1T);
  lcd.print("C,");
  lcd.print("HUM:");
  lcd.print(d1H);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("MNG:");
  lcd.print(d1M);
  lcd.print("C,NIT:");
  lcd.print(d1N);
  lcd.print("C");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DAY2:");
  lcd.print(d2T);
  lcd.print("C,");
  lcd.print("HUM:");
  lcd.print(d2H);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("MNG:");
  lcd.print(d2M);
  lcd.print("C,NIT:");
  lcd.print(d2N);
  lcd.print("C");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DAY3:");
  lcd.print(d3T);
  lcd.print("C,");
  lcd.print("HUM:");
  lcd.print(d3H);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("MNG:");
  lcd.print(d3M);
  lcd.print("C,NIT:");
  lcd.print(d3N);
  lcd.print("C");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DAY4:");
  lcd.print(d4T);
  lcd.print("C,");
  lcd.print("HUM:");
  lcd.print(d4H);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("MNG:");
  lcd.print(d4M);
  lcd.print("C,NIT:");
  lcd.print(d4N);
  lcd.print("C");
  delay(2000);
}

void displayDHT() {
  float humidityValue = dht.readHumidity();
  float temperatureValue = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidityValue) || isnan(temperatureValue)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TEMP : ");
  lcd.setCursor(6, 0);
  lcd.print(temperatureValue);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("HUMID : ");
  lcd.setCursor(6, 1);
  lcd.print(humidityValue);
  lcd.print("%");
  delay(2000);
}

void displayTime(String dateString, String timeString) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DATE:");
  lcd.setCursor(6, 0);
  lcd.print(dateString);

  lcd.setCursor(0, 1);
  lcd.print("TIME:");
  lcd.setCursor(6, 1);
  lcd.print(timeString);
  delay(2000);
}


