#define BLYNK_TEMPLATE_ID "TMPL6cXIzueBs"
#define BLYNK_TEMPLATE_NAME "weather"
#include <typeinfo>
#include <LiquidCrystal_I2C.h>


#define BLYNK_TEMPLATE_NAME "weather"
#define BLYNK_AUTH_TOKEN "p_koBbc-SbNlfFkNTtEVhcONyBp_o5Ce"
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <BlynkSimpleEsp8266.h>

#define DHTPIN D3
#define DHTTYPE DHT22

const char* ntpServerName = "pool.ntp.org";
const int utcOffsetInSeconds = 19800; // UTC offset in seconds (IST: UTC+5:30)

const long gmtOffset_sec = 19800; // GMT offset in seconds (IST: UTC+5:30)
const int daylightOffset_sec = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiUDP udp;
NTPClient timeClient(udp, ntpServerName, gmtOffset_sec, daylightOffset_sec);

const char* ssid = "A";
const char* password = "manimani";
const String apiKey = "818e3dc850b8488b1cabebea7286b59f";
bool isFetched = false;
String finalData;

String timeAndDate = "";
String weatherDataToSend;
String serializedWeatherData;

// Blynk authentication token
char auth[] = "p_koBbc-SbNlfFkNTtEVhcONyBp_o5Ce";

// location coords
const float lon = 75.702904;
const float lat = 31.251240;

const int maxDays = 7;
double dailyTemperatures[maxDays];

unsigned long prevTime = 0;
const unsigned long interval = 1000; // Adjust the interval as needed (in milliseconds)

String temperatures[20];
String humidity[20];
String feelsLike[20];
String feelsLikeDay[20];
String feelsLikeNight[20];
String feelsLikeMorn[20];
String feelsLikeEve[20];
int dayIndex = 0;
int blynk_day_index = 1;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear(); 
  lcd.setCursor(0, 0); 
  lcd.print("IOT WEATHERCLOCK");
  lcd.setCursor(0, 1); 
  lcd.print("WIFI CONNECTING.."); 
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, weatherDataToSend);
  

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println(">Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();
  timeClient.update();

  Blynk.begin(auth, ssid, password);

  Serial.println("");
  Serial.println(">Connected to ");
  Serial.println(ssid);
  Serial.print(">IP address: ");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WIFI CONNECTED");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  Serial.println(WiFi.localIP());
}

BLYNK_WRITE(V3) {
    int value = param.asInt();
    blynk_day_index = value;
    Serial.print("Received value: ");
    Serial.println(value);
}

void loop() {
  Blynk.run(); 

  timeAndDate = "";
  finalData = "";

  float dht_humidity = dht.readHumidity();
  float dht_temperature = dht.readTemperature();

  if (!isFetched) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClientSecure client;
      client.setInsecure();

      HTTPClient https;
      Serial.println("> Requesting data");
      if (https.begin(client, "https://api.openweathermap.org/data/2.5/onecall?lat=" + String(lat) + "&lon=" + String(lon) + "&exclude=minutely,hourly,current&appid=" + apiKey)) {
        int httpCode = https.GET();
        Serial.println("> Response code: " + String(httpCode));
        if (String(httpCode) == "200") {
          isFetched = true;
        }
        if (httpCode > 0) {
          String weatherData = https.getString();
          weatherDataToSend = weatherData;

          // string manipulation
          const char* response = weatherDataToSend.c_str();
          const char* searchStart = response;

          while (dayIndex < 8 && (searchStart = strstr(searchStart, "\"temp\":{\"day\":")) != nullptr) {
            searchStart += 14; // Move to the beginning of the temperature value
            char* endPtr;
            float tempDayKelvin = strtof(searchStart, &endPtr);
            if (endPtr != searchStart) {
              temperatures[dayIndex] = static_cast<int>(tempDayKelvin - 273.15);

              searchStart = strstr(searchStart, "\"humidity\":");
              if (searchStart != nullptr) {
                searchStart += 12; // Move to the beginning of the humidity value
                int humidityValue = strtol(searchStart, &endPtr, 10);
                if (endPtr != searchStart) {
                  // Check if humidity is below 10 and add a leading zero if needed
                  humidity[dayIndex] = (humidityValue < 10) ? "0" + String(humidityValue) : String(humidityValue);
                }
              }

              // Extract "feels_like" values for different times of the day
              const char* feelsLikeKey = "\"feels_like\":{\"day\":";
              searchStart = strstr(searchStart, feelsLikeKey);
              if (searchStart != nullptr) {
                searchStart += strlen(feelsLikeKey); // Move to the beginning of the "feels_like" value for the day
                float feelsLikeDayKelvin = strtof(searchStart, &endPtr);
                if (endPtr != searchStart) {
                  feelsLikeDay[dayIndex] = static_cast<int>(feelsLikeDayKelvin - 273.15);
                }
              }

              const char* feelsLikeNightKey = "\"night\":";
              searchStart = strstr(searchStart, feelsLikeNightKey);
              if (searchStart != nullptr) {
                searchStart += strlen(feelsLikeNightKey); // Move to the beginning of the "feels_like" value for the night
                float feelsLikeNightKelvin = strtof(searchStart, &endPtr);
                if (endPtr != searchStart) {
                  feelsLikeNight[dayIndex] = static_cast<int>(feelsLikeNightKelvin - 273.15);
                }
              }

              const char* feelsLikeEveKey = "\"eve\":";
              searchStart = strstr(searchStart, feelsLikeEveKey);
              if (searchStart != nullptr) {
                searchStart += strlen(feelsLikeEveKey); // Move to the beginning of the "feels_like" value for the evening
                float feelsLikeEveKelvin = strtof(searchStart, &endPtr);
                if (endPtr != searchStart) {
                  feelsLikeEve[dayIndex] = static_cast<int>(feelsLikeEveKelvin - 273.15);
                }
              }

              const char* feelsLikeMornKey = "\"morn\":";
              searchStart = strstr(searchStart, feelsLikeMornKey);
              if (searchStart != nullptr) {
                searchStart += strlen(feelsLikeMornKey); // Move to the beginning of the "feels_like" value for the morning
                float feelsLikeMornKelvin = strtof(searchStart, &endPtr);
                if (endPtr != searchStart) {
                  feelsLikeMorn[dayIndex] = static_cast<int>(feelsLikeMornKelvin - 273.15);
                }
              }

              dayIndex++;
            }
          }
        }
        https.end();
      } else {
        Serial.printf(">[HTTPS] Unable to connect\n");
      }
    }
  }

  int tempArraySize = sizeof(temperatures) / sizeof(temperatures[0]);
  serializedWeatherData = "Weather:";

  for (int i = 0; i < 6; i++) {
    String temp = String(temperatures[i]);
    String humidityValue = humidity[i];
    String feelsLikeDayValue = String(feelsLikeDay[i]);
    String feelsLikeNightValue = String(feelsLikeNight[i]);
    String feelsLikeEveValue = String(feelsLikeEve[i]);
    String feelsLikeMornValue = String(feelsLikeMorn[i]);

    serializedWeatherData += "D" + String(i + 1) + ":" + String(temp) + "C," + String(humidityValue) + "%,";
    serializedWeatherData += "Day=" + String(feelsLikeDayValue) + "C,";
    serializedWeatherData += "Night=" + String(feelsLikeNightValue) + "C,";
    serializedWeatherData += "Eve=" + String(feelsLikeEveValue) + "C,";
    serializedWeatherData += "Mrng=" + String(feelsLikeMornValue) + "C|";
  }

  // Time
  if (millis() - prevTime > interval) {
    prevTime = millis();
    timeAndDate = "Time:" + getFormattedTime();
  }
  finalData = timeAndDate + serializedWeatherData;
  timeClient.update();
  
  
  int dht_temp_int = static_cast<int>(dht_temperature);
  int dht_hum_int = static_cast<int>(dht_humidity);

  
  
  if(blynk_day_index == 1){
    String DHT_DATA_FOR_LCD = "TEMP:" + static_cast<String>(dht_temp_int) + "C,HUM:" + static_cast<String>(dht_hum_int) + "%";
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print(getFormattedTime());
    lcd.setCursor(0, 1); 
    lcd.print(DHT_DATA_FOR_LCD);
  }else{
    String FULL_DAY = "DAY " + static_cast<String>(blynk_day_index);
    String API_DATA_FOR_LCD_ROW_0 = FULL_DAY + " TEMP:" + temperatures[blynk_day_index - 1]+"C";
    String API_DATA_FOR_LCD_ROW_1 = FULL_DAY + " HUM:" + humidity[blynk_day_index - 1]+"%";
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print(API_DATA_FOR_LCD_ROW_0);
    lcd.setCursor(0, 1);
    lcd.print(API_DATA_FOR_LCD_ROW_1);
  }

  
  Blynk.virtualWrite(V0, dht_temp_int);
  Blynk.virtualWrite(V1, dht_hum_int);
  Blynk.virtualWrite(V2, timeAndDate);

  String day = "D" + static_cast<String>(blynk_day_index);
  String blynk_api_data = day + " Temperature : " + String(temperatures[blynk_day_index - 1]) + "C, " + day + " Humidity : " + String(humidity[blynk_day_index - 1]) + "%, " + day + " Feelslike morning : " + String(feelsLikeMorn[blynk_day_index - 1]) + "C, " + day + " Feelslike evening : " + String(feelsLikeEve[blynk_day_index - 1]) + "C, " + day + " Feelslike night : " + String(feelsLikeNight[blynk_day_index - 1]) + "C";
  Blynk.virtualWrite(V4, blynk_api_data);
 
  delay(1000);
}

time_t getNtpTime() {
  // Send an NTP request to the time server
  byte packetBuffer[48];
  udp.beginPacket(ntpServerName, 123); // NTP requests are to port 123
  memset(packetBuffer, 0, 48);
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0; // Stratum
  packetBuffer[2] = 6; // Polling Interval
  packetBuffer[3] = 0xEC; // Peer Clock Precision
  udp.write(packetBuffer, 48);
  udp.endPacket();

  // Wait for response
  delay(1000);

  // Read the NTP response
  if (udp.parsePacket()) {
    udp.read(packetBuffer, 48);
    // The timestamp starts at byte 43 for 4 bytes.
    unsigned long highWord = word(packetBuffer[43], packetBuffer[42]);
    unsigned long lowWord = word(packetBuffer[41], packetBuffer[40]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // Unix time starts on January 1, 1970, so remove the 70 years
    // (1970-1900 = 70) and adjust for the time zone
    return secsSince1900 - 2208988800UL + utcOffsetInSeconds;
  }

  return 0; // If no response or an error occurred
}

String getFormattedTime() {
  time_t now = timeClient.getEpochTime();
  struct tm timeInfo;
  char output[25];

  // Check if the time has been set properly
  if (now < 0) {
    return "Invalid timestamp";
  }

  gmtime_r(&now, &timeInfo);
  strftime(output, 25, "%Y-%m-%d %H:%M:%S", &timeInfo);

  return output;
}