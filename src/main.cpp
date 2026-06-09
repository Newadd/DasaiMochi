#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define SDA_PIN 21
#define SCL_PIN 22
#define TOUCH_PIN 27

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ========== KONFIGURASI (GANTI DENGAN PUNYA ANDA) ==========
const char* ssid = "Pedro";       // Ganti dengan WiFi Anda
const char* password = "12345678";   // Ganti password WiFi
const char* apiKey = "79ae53ad2d29c77cadfc22f9ad630dca";
const char* city = "Nganjuk";
const char* country = "ID";
const char* tz = "WIB-7";

// ========== GLOBAL ==========
float temperature = 0;
int humidity = 0;
String weatherMain = "Clear";
int currentPage = 0;
unsigned long lastPageSwitch = 0;
const unsigned long PAGE_INTERVAL = 8000;
unsigned long lastWeatherUpdate = 0;

// ========== PROTOTIPE FUNGSI ==========
void getWeather();
void handleTouch();
void drawEmoPage();
void drawClockPage();
void drawWeatherPage();
void drawWorldClockPage();
void drawForecastPage();

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  Wire.begin(SDA_PIN, SCL_PIN);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED GAGAL!");
    while (1);
  }
  
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi OK!");
  display.println("WiFi OK!");
  display.display();
  delay(1000);
  
  configTime(0, 0, "pool.ntp.org");
  setenv("TZ", tz, 1);
  tzset();
  
  getWeather();
  lastWeatherUpdate = millis();
  lastPageSwitch = millis();
}

// ========== LOOP ==========
void loop() {
  unsigned long now = millis();
  handleTouch();
  
  if (now - lastWeatherUpdate > 600000) {
    getWeather();
    lastWeatherUpdate = now;
  }
  
  if (now - lastPageSwitch > PAGE_INTERVAL) {
    currentPage = (currentPage + 1) % 5;
    lastPageSwitch = now;
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  switch(currentPage) {
    case 0: drawEmoPage(); break;
    case 1: drawClockPage(); break;
    case 2: drawWeatherPage(); break;
    case 3: drawWorldClockPage(); break;
    case 4: drawForecastPage(); break;
  }
  
  display.display();
  delay(50);
}

// ========== TOUCH ==========
void handleTouch() {
  static int lastState = LOW;
  static unsigned long lastTap = 0;
  int state = digitalRead(TOUCH_PIN);
  
  if (state == HIGH && lastState == LOW) {
    if (millis() - lastTap > 200) {
      currentPage = (currentPage + 1) % 5;
      lastPageSwitch = millis();
      Serial.print("Page: ");
      Serial.println(currentPage);
    }
    lastTap = millis();
  }
  lastState = state;
}

// ========== HALAMAN 0 ==========
void drawEmoPage() {
  display.setTextSize(2);
  display.setCursor(35, 20);
  display.println(": )");
  display.setTextSize(1);
  display.setCursor(30, 50);
  display.println("Dasai Mochi");
}

// ========== HALAMAN 1 ==========
void drawClockPage() {
  struct tm t;
  if (!getLocalTime(&t)) {
    display.println("Sync Time...");
    return;
  }
  display.setTextSize(2);
  display.setCursor(10, 20);
  char buf[9];
  sprintf(buf, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
  display.println(buf);
  display.setTextSize(1);
  display.setCursor(10, 50);
  strftime(buf, 20, "%a, %d %b", &t);
  display.println(buf);
}

// ========== HALAMAN 2 ==========
void drawWeatherPage() {
  if (WiFi.status() != WL_CONNECTED) {
    display.println("No WiFi");
    return;
  }
  if (temperature == 0 && weatherMain == "Clear") {
    display.println("Loading...");
    return;
  }
  display.println("Weather:");
  display.print(weatherMain);
  display.println();
  display.print("Temp: ");
  display.print(temperature);
  display.println(" C");
  display.print("Humidity: ");
  display.print(humidity);
  display.println("%");
}

// ========== HALAMAN 3 ==========
void drawWorldClockPage() {
  time_t now = time(nullptr);
  if (now < 100000) {
    display.println("Sync Time...");
    return;
  }
  time_t india = now + 5*3600 + 30*60;
  time_t sydney = now + 11*3600;
  struct tm *itm = gmtime(&india);
  struct tm *stm = gmtime(&sydney);
  
  display.println("World Clock:");
  display.print("INDIA: ");
  display.print(itm->tm_hour);
  display.print(":");
  display.println(itm->tm_min);
  display.print("SYDNEY: ");
  display.print(stm->tm_hour);
  display.print(":");
  display.println(stm->tm_min);
}

// ========== HALAMAN 4 ==========
void drawForecastPage() {
  display.println("Forecast");
  display.println("Coming soon");
  display.println("Tap to refresh");
}

// ========== WEATHER ==========
void getWeather() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "," + String(country) + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  int code = http.GET();
  if (code == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);
    temperature = doc["main"]["temp"];
    humidity = doc["main"]["humidity"];
    weatherMain = doc["weather"][0]["main"].as<String>();
    Serial.println("Weather updated: " + weatherMain + " " + String(temperature) + "C");
  } else {
    Serial.print("Weather error: ");
    Serial.println(code);
  }
  http.end();
}
