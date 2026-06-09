// ==================================================
// DASAI MOCHI PRO - Smart Mochi with Physics Eyes
// Adapted for SSD1306 OLED (128x64, I2C)
// ==================================================

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include <math.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

// ========== HARDWARE CONFIG ==========
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define SDA_PIN 21
#define SCL_PIN 22
#define TOUCH_PIN 27

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ========== WEATHER ICONS (sama seperti sebelumnya) ==========
const unsigned char bmp_clear[] PROGMEM = { 0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0xc0,0x80,0x00,0x0f,0xf0,0x00,0x00,0x3f,0xfc,0x00,0x00,0x7f,0xfe,0x00,0x00,0xff,0xff,0x00,0x06,0xff,0xff,0x60,0x06,0xff,0xff,0x60,0x06,0xff,0xff,0x60,0x00,0xff,0xff,0x00,0x3e,0xff,0xff,0x7c,0x3e,0xff,0xff,0x7c,0x3e,0xff,0xff,0x7c,0x00,0xff,0xff,0x00,0x06,0xff,0xff,0x60,0x06,0xff,0xff,0x60,0x06,0xff,0xff,0x60,0x00,0xff,0xff,0x00,0x00,0x7f,0xfe,0x00,0x00,0x3f,0xfc,0x00,0x01,0x0f,0xf0,0x80,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
const unsigned char bmp_clouds[] PROGMEM = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe0,0x00,0x00,0x0f,0xf8,0x00,0x00,0x1f,0xfc,0x00,0x00,0x3f,0xfe,0x00,0x00,0x3f,0xff,0x00,0x00,0x7f,0xff,0x80,0x00,0xff,0xff,0xc0,0x00,0xff,0xff,0xe0,0x01,0xff,0xff,0xf0,0x03,0xff,0xff,0xf8,0x07,0xff,0xff,0xfc,0x07,0xff,0xff,0xfc,0x0f,0xff,0xff,0xfe,0x0f,0xff,0xff,0xfe,0x1f,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0x0f,0xff,0xff,0xfe,0x07,0xff,0xff,0xfc,0x03,0xff,0xff,0xf8,0x00,0xff,0xff,0xe0,0x00,0x3f,0xff,0x80,0x00,0x0f,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
const unsigned char bmp_rain[] PROGMEM = { 0x00,0x00,0x00,0x00,0x00,0x03,0xe0,0x00,0x00,0x0f,0xf8,0x00,0x00,0x1f,0xfc,0x00,0x00,0x3f,0xfe,0x00,0x00,0x7f,0xff,0x80,0x00,0xff,0xff,0xc0,0x01,0xff,0xff,0xf0,0x03,0xff,0xff,0xf8,0x07,0xff,0xff,0xfc,0x0f,0xff,0xff,0xfe,0x1f,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0x0f,0xff,0xff,0xfe,0x07,0xff,0xff,0xfc,0x03,0xff,0xff,0xf8,0x00,0xff,0xff,0xe0,0x00,0x3f,0xff,0x80,0x00,0x0f,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x0c,0x00,0x00,0x60,0x0c,0x00,0x00,0xe0,0x1c,0x00,0x00,0xc0,0x18,0x00,0x03,0x80,0x70,0x00,0x03,0x80,0x70,0x00,0x03,0x00,0x60,0x00,0x02,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
const unsigned char mini_sun[] PROGMEM = { 0x00,0x00,0x01,0x80,0x00,0x00,0x10,0x08,0x04,0x20,0x03,0xc0,0x27,0xe4,0x07,0xe0,0x07,0xe0,0x27,0xe4,0x03,0xc0,0x04,0x20,0x10,0x08,0x00,0x00,0x01,0x80,0x00,0x00 };
const unsigned char mini_cloud[] PROGMEM = { 0x00,0x00,0x00,0x00,0x01,0xc0,0x07,0xe0,0x0f,0xf0,0x1f,0xf8,0x1f,0xf8,0x3f,0xfc,0x3f,0xfc,0x7f,0xfe,0x3f,0xfe,0x1f,0xfc,0x0f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00 };
const unsigned char mini_rain[] PROGMEM = { 0x00,0x00,0x00,0x00,0x01,0xc0,0x07,0xe0,0x0f,0xf0,0x1f,0xf8,0x1f,0xf8,0x3f,0xfc,0x3f,0xfc,0x7f,0xfe,0x3f,0xfe,0x1f,0xfc,0x00,0x00,0x44,0x44,0x22,0x22,0x11,0x11 };
const unsigned char bmp_tiny_drop[] PROGMEM = { 0x10,0x38,0x7c,0xfe,0xfe,0x7c,0x38,0x00 };
const unsigned char bmp_heart[] PROGMEM = { 0x00,0x00,0x0c,0x60,0x1e,0xf0,0x3f,0xf8,0x7f,0xfc,0x7f,0xfc,0x7f,0xfc,0x3f,0xf8,0x1f,0xf0,0x0f,0xe0,0x07,0xc0,0x03,0x80,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
const unsigned char bmp_zzz[] PROGMEM = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x0c,0x00,0x18,0x00,0x30,0x00,0x7e,0x00,0x00,0x3c,0x00,0x0c,0x00,0x18,0x00,0x30,0x00,0x7c,0x00,0x00,0x00,0x00,0x00 };
const unsigned char bmp_anger[] PROGMEM = { 0x00,0x00,0x11,0x10,0x2a,0x90,0x44,0x40,0x80,0x20,0x80,0x20,0x44,0x40,0x2a,0x90,0x11,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

// ========== GLOBALS ==========
Preferences prefs;
WebServer configServer(80);
bool inConfigMode = false;
bool highBrightness = true;
int currentPage = 0;        // 0=mata, 1=jam, 2=cuaca, 3=world clock, 4=forecast
unsigned long lastPageSwitch = 0;
const unsigned long PAGE_INTERVAL = 8000;
unsigned long lastWeatherUpdate = 0;
float temperature = 0.0, feelsLike = 0.0;
int humidity = 0;
String weatherMain = "Clear";
String weatherDesc = "Sunny";
String city = "Nganjuk";
String countryCode = "ID";
String apiKey = "79ae53ad2d29c77cadfc22f9ad630dca";
String wifiSsid = "";
String wifiPass = "";
String tzString = "WIB-7";

struct ForecastDay {
  String dayName;
  int temp;
  String iconType;
} fcast[3];

// ========== MOODS ==========
#define MOOD_NORMAL 0
#define MOOD_HAPPY 1
#define MOOD_SURPRISED 2
#define MOOD_SLEEPY 3
#define MOOD_ANGRY 4
#define MOOD_SAD 5
#define MOOD_EXCITED 6
#define MOOD_LOVE 7
#define MOOD_SUSPICIOUS 8
int currentMood = MOOD_NORMAL;

// ========== PHYSICS EYES ==========
struct Eye {
  float x, y, w, h;
  float targetX, targetY, targetW, targetH;
  float pupilX, pupilY, targetPupilX, targetPupilY;
  float velX, velY, velW, velH, pVelX, pVelY;
  float k = 0.12, d = 0.60;
  float pk = 0.08, pd = 0.50;
  bool blinking;
  unsigned long lastBlink, nextBlinkTime;

  void init(float _x, float _y, float _w, float _h) {
    x = targetX = _x; y = targetY = _y; w = targetW = _w; h = targetH = _h;
    pupilX = targetPupilX = 0; pupilY = targetPupilY = 0;
    nextBlinkTime = millis() + random(1000, 4000);
    blinking = false;
  }
  void update() {
    float ax = (targetX - x) * k, ay = (targetY - y) * k;
    float aw = (targetW - w) * k, ah = (targetH - h) * k;
    velX = (velX + ax) * d; velY = (velY + ay) * d;
    velW = (velW + aw) * d; velH = (velH + ah) * d;
    x += velX; y += velY; w += velW; h += velH;
    float pax = (targetPupilX - pupilX) * pk, pay = (targetPupilY - pupilY) * pk;
    pVelX = (pVelX + pax) * pd; pVelY = (pVelY + pay) * pd;
    pupilX += pVelX; pupilY += pVelY;
  }
};

Eye leftEye, rightEye;
unsigned long lastSaccade = 0;
unsigned long saccadeInterval = 3000;
float breathVal = 0;

// ========== TOUCH ==========
int tapCounter = 0;
unsigned long lastTapTime = 0;
bool lastPinState = false;
unsigned long pressStartTime = 0;
bool isLongPressHandled = false;
const unsigned long LONG_PRESS_TIME = 800;
const unsigned long DOUBLE_TAP_DELAY = 300;

// ========== PROTOTYPES ==========
void loadConfig();
void saveConfig(const String& s, const String& p, const String& ak, const String& cty, const String& ctry, const String& tz);
void startConfigPortal();
void handleConfigRoot();
void handleConfigSave();
void updateMoodBasedOnWeather();
void getWeatherAndForecast();
void handleTouch();
void drawEyelidMask(float x, float y, float w, float h, int mood, bool isLeft);
void drawUltraProEye(Eye& e, bool isLeft);
void updatePhysicsAndMood();
void drawEmoPage();
void drawClock();
void drawWeatherCard();
void drawWorldClock();
void drawForecastPage();
void playBootAnimation();

// ========== CONFIG PORTAL ==========
void loadConfig() {
  prefs.begin("dasaimochi", true);
  wifiSsid    = prefs.getString("ssid", "");
  wifiPass    = prefs.getString("pass", "");
  apiKey      = prefs.getString("apikey", "");
  city        = prefs.getString("city", "");
  countryCode = prefs.getString("country", "");
  tzString    = prefs.getString("tz", "");
  prefs.end();
  if (wifiSsid.isEmpty()) {
    wifiSsid    = "YourWiFi";
    wifiPass    = "password";
    apiKey      = "79ae53ad2d29c77cadfc22f9ad630dca";
    city        = "Nganjuk";
    countryCode = "ID";
    tzString    = "WIB-7";
  }
}

void saveConfig(const String& s, const String& p, const String& ak,
                const String& cty, const String& ctry, const String& tz) {
  prefs.begin("dasaimochi", false);
  prefs.putString("ssid", s);
  prefs.putString("pass", p);
  prefs.putString("apikey", ak);
  prefs.putString("city", cty);
  prefs.putString("country", ctry);
  prefs.putString("tz", tz);
  prefs.end();
}

void handleConfigRoot() {
  String html = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Dasai Mochi Setup</title>
<style>body{font-family:sans-serif;max-width:420px;margin:30px auto;padding:24px;background:#1a1a2e;color:#eee;}input{width:100%;padding:10px;margin:6px 0;border-radius:6px;background:#2d2d44;color:#fff;border:1px solid #5a5a8a;}button{width:100%;padding:12px;background:#e94560;color:#fff;border:none;border-radius:6px;margin-top:16px;}</style></head><body>
<h1>🍡 Dasai Mochi Setup</h1>
<form action="/save" method="POST">
<label>WiFi SSID</label><input name="ssid" placeholder="Your WiFi" value=")rawliteral";
  html += wifiSsid;
  html += R"rawliteral(">
<label>Password</label><input name="pass" type="password">
<label>OpenWeatherMap API Key</label><input name="apikey" value=")rawliteral";
  html += apiKey;
  html += R"rawliteral(">
<label>City</label><input name="city" value=")rawliteral";
  html += city;
  html += R"rawliteral(">
<label>Country Code (ID, US, etc)</label><input name="country" value=")rawliteral";
  html += countryCode;
  html += R"rawliteral(">
<label>Timezone (ex: WIB-7, IST-5:30, EST5EDT)</label><input name="tz" value=")rawliteral";
  html += tzString;
  html += R"rawliteral(">
<button type="submit">Save & Reboot</button>
</form></body></html>)rawliteral";
  configServer.send(200, "text/html", html);
}

void handleConfigSave() {
  String s = configServer.arg("ssid");
  String p = configServer.arg("pass");
  String ak = configServer.arg("apikey");
  String cty = configServer.arg("city");
  String ctr = configServer.arg("country");
  String tz = configServer.arg("tz");
  if (s.length() == 0) { configServer.send(400, "text/plain", "SSID required"); return; }
  if (ak.length() == 0) ak = apiKey;
  if (cty.length() == 0) cty = city;
  if (ctr.length() == 0) ctr = countryCode;
  if (tz.length() == 0) tz = tzString;
  saveConfig(s, p, ak, cty, ctr, tz);
  configServer.send(200, "text/html", "<html><body><h2>Saved!</h2><p>Rebooting...</p></body></html>");
  delay(2000);
  ESP.restart();
}

void startConfigPortal() {
  inConfigMode = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP("DasaiMochi-Setup", "12345678");
  configServer.on("/", handleConfigRoot);
  configServer.on("/save", HTTP_POST, handleConfigSave);
  configServer.begin();
  display.clearDisplay();
  display.setFont(NULL);
  display.setCursor(0,0);
  display.println("Config Mode\nConnect to WiFi:\nDasaiMochi-Setup\npw:12345678\nThen open 192.168.4.1");
  display.display();
}

// ========== WEATHER ==========
const unsigned char* getBigIcon(String w) {
  if (w == "Clear") return bmp_clear;
  if (w == "Clouds") return bmp_clouds;
  if (w == "Rain" || w == "Drizzle") return bmp_rain;
  return bmp_clouds;
}
const unsigned char* getMiniIcon(String w) {
  if (w == "Clear") return mini_sun;
  if (w == "Rain" || w == "Drizzle") return mini_rain;
  return mini_cloud;
}

void updateMoodBasedOnWeather() {
  if (weatherMain == "Clear") currentMood = MOOD_HAPPY;
  else if (weatherMain == "Rain" || weatherMain == "Drizzle") currentMood = MOOD_SAD;
  else if (weatherMain == "Thunderstorm") currentMood = MOOD_SURPRISED;
  else if (weatherMain == "Clouds") currentMood = MOOD_NORMAL;
  else if (temperature > 30) currentMood = MOOD_EXCITED;
  else if (temperature < 20) currentMood = MOOD_SLEEPY;
  else currentMood = MOOD_NORMAL;
}

void getWeatherAndForecast() {
  if (WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  int code = http.GET();
  if (code == 200) {
    String payload = http.getString();
    JSONVar obj = JSON.parse(payload);
    if (JSON.typeof(obj) != "undefined") {
      temperature = double(obj["main"]["temp"]);
      feelsLike = double(obj["main"]["feels_like"]);
      humidity = int(obj["main"]["humidity"]);
      weatherMain = (const char*)obj["weather"][0]["main"];
      weatherDesc = (const char*)obj["weather"][0]["description"];
      weatherDesc[0] = toupper(weatherDesc[0]);
      updateMoodBasedOnWeather();
    }
  }
  http.end();

  url = "http://api.openweathermap.org/data/2.5/forecast?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric";
  http.begin(url);
  if (http.GET() == 200) {
    String payload = http.getString();
    JSONVar fo = JSON.parse(payload);
    if (JSON.typeof(fo) != "undefined") {
      struct tm t;
      getLocalTime(&t);
      int today = t.tm_wday;
      const char* days[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
      int indices[3] = {7,15,23};
      for (int i=0;i<3;i++) {
        fcast[i].temp = (int)double(fo["list"][indices[i]]["main"]["temp"]);
        fcast[i].iconType = (const char*)fo["list"][indices[i]]["weather"][0]["main"];
        int nextDay = (today + i + 1) % 7;
        fcast[i].dayName = days[nextDay];
      }
    }
  }
  http.end();
}

// ========== TOUCH ==========
void handleTouch() {
  bool nowState = digitalRead(TOUCH_PIN);
  unsigned long now = millis();
  if (nowState && !lastPinState) {
    pressStartTime = now;
    isLongPressHandled = false;
  } else if (nowState && lastPinState) {
    if (!isLongPressHandled && (now - pressStartTime > LONG_PRESS_TIME)) {
      currentPage = 0;
      lastPageSwitch = now;
      isLongPressHandled = true;
    }
  } else if (!nowState && lastPinState) {
    if ((now - pressStartTime) < LONG_PRESS_TIME && !isLongPressHandled) {
      tapCounter++;
      lastTapTime = now;
    }
  }
  lastPinState = nowState;

  if (tapCounter > 0 && (now - lastTapTime > DOUBLE_TAP_DELAY)) {
    if (tapCounter == 2) {
      // Double tap: brightness toggle for SSD1306 (dim)
      highBrightness = !highBrightness;
      display.dim(!highBrightness);  // true = dim, false = normal
      display.display();
    } else if (tapCounter == 1) {
      if (currentPage == 3) currentPage = 1;
      else if (currentPage == 4) currentPage = 2;
      else {
        currentPage++;
        if (currentPage > 2) currentPage = 0;
      }
      lastPageSwitch = millis();
      lastSaccade = 0;
    }
    tapCounter = 0;
  }
}

// ========== PHYSICS DRAWING ==========
void drawEyelidMask(float x, float y, float w, float h, int mood, bool isLeft) {
  int ix = (int)x, iy = (int)y, iw = (int)w, ih = (int)h;
  display.setTextColor(SSD1306_BLACK);
  if (mood == MOOD_ANGRY) {
    if (isLeft) for (int i=0;i<16;i++) display.drawLine(ix, iy+i, ix+iw, iy-6+i, SSD1306_BLACK);
    else for (int i=0;i<16;i++) display.drawLine(ix, iy-6+i, ix+iw, iy+i, SSD1306_BLACK);
  } else if (mood == MOOD_SAD) {
    if (isLeft) for (int i=0;i<16;i++) display.drawLine(ix, iy-6+i, ix+iw, iy+i, SSD1306_BLACK);
    else for (int i=0;i<16;i++) display.drawLine(ix, iy+i, ix+iw, iy-6+i, SSD1306_BLACK);
  } else if (mood == MOOD_HAPPY || mood == MOOD_LOVE || mood == MOOD_EXCITED) {
    display.fillRect(ix, iy+ih-12, iw, 14, SSD1306_BLACK);
    display.fillCircle(ix+iw/2, iy+ih+6, iw/1.3, SSD1306_BLACK);
  } else if (mood == MOOD_SLEEPY) {
    display.fillRect(ix, iy, iw, ih/2+2, SSD1306_BLACK);
  } else if (mood == MOOD_SUSPICIOUS) {
    if (isLeft) display.fillRect(ix, iy, iw, ih/2-2, SSD1306_BLACK);
    else display.fillRect(ix, iy+ih-8, iw, 8, SSD1306_BLACK);
  }
}

void drawUltraProEye(Eye& e, bool isLeft) {
  int ix = (int)e.x, iy = (int)e.y, iw = (int)e.w, ih = (int)e.h;
  int r = (iw < 20) ? 3 : 8;
  display.fillRoundRect(ix, iy, iw, ih, r, SSD1306_WHITE);
  int cx = ix + iw/2, cy = iy + ih/2;
  int pw = iw/2.2, ph = ih/2.2;
  int px = cx + (int)e.pupilX - pw/2;
  int py = cy + (int)e.pupilY - ph/2;
  if (px < ix) px = ix; if (px+pw > ix+iw) px = ix+iw-pw;
  if (py < iy) py = iy; if (py+ph > iy+ih) py = iy+ih-ph;
  display.fillRoundRect(px, py, pw, ph, r/2, SSD1306_BLACK);
  if (iw > 15 && ih > 15) display.fillCircle(px+pw-4, py+4, 2, SSD1306_WHITE);
  drawEyelidMask(e.x, e.y, e.w, e.h, currentMood, isLeft);
}

void updatePhysicsAndMood() {
  unsigned long now = millis();
  breathVal = sin(now / 800.0) * 1.5;
  if (now > leftEye.nextBlinkTime) {
    leftEye.blinking = true; rightEye.blinking = true;
    leftEye.lastBlink = now;
    leftEye.nextBlinkTime = now + random(2000,6000);
  }
  if (leftEye.blinking) {
    leftEye.targetH = 2; rightEye.targetH = 2;
    if (now - leftEye.lastBlink > 120) { leftEye.blinking = false; rightEye.blinking = false; }
  }

  if (!leftEye.blinking && now - lastSaccade > saccadeInterval) {
    lastSaccade = now;
    saccadeInterval = random(500,3000);
    int dir = random(10);
    float lx=0, ly=0;
    if (dir==4) { lx=-6; ly=-4; }
    else if (dir==5) { lx=6; ly=-4; }
    else if (dir==6) { lx=-6; ly=4; }
    else if (dir==7) { lx=6; ly=4; }
    else if (dir==8) { lx=8; ly=0; }
    else if (dir==9) { lx=-8; ly=0; }
    leftEye.targetPupilX = lx; leftEye.targetPupilY = ly;
    rightEye.targetPupilX = lx; rightEye.targetPupilY = ly;
    leftEye.targetX = 18 + lx*0.3; leftEye.targetY = 14 + ly*0.3;
    rightEye.targetX = 74 + lx*0.3; rightEye.targetY = 14 + ly*0.3;
  }

  if (!leftEye.blinking) {
    float baseW = 36, baseH = 36 + breathVal;
    switch (currentMood) {
      case MOOD_NORMAL: leftEye.targetW=baseW; leftEye.targetH=baseH; rightEye.targetW=baseW; rightEye.targetH=baseH; break;
      case MOOD_HAPPY: case MOOD_LOVE: leftEye.targetW=40; leftEye.targetH=32; rightEye.targetW=40; rightEye.targetH=32; break;
      case MOOD_SURPRISED: leftEye.targetW=30; leftEye.targetH=45; rightEye.targetW=30; rightEye.targetH=45; leftEye.targetPupilX += random(-1,2); break;
      case MOOD_SLEEPY: leftEye.targetW=38; leftEye.targetH=30; rightEye.targetW=38; rightEye.targetH=30; break;
      case MOOD_ANGRY: leftEye.targetW=34; leftEye.targetH=32; rightEye.targetW=34; rightEye.targetH=32; break;
      case MOOD_SAD: leftEye.targetW=34; leftEye.targetH=40; rightEye.targetW=34; rightEye.targetH=40; break;
      case MOOD_SUSPICIOUS: leftEye.targetW=36; leftEye.targetH=20; rightEye.targetW=36; rightEye.targetH=42; break;
      default: break;
    }
  }
  leftEye.update(); rightEye.update();
}

void drawEmoPage() {
  updatePhysicsAndMood();
  if (currentMood == MOOD_LOVE) display.drawBitmap(56,0,bmp_heart,16,16,SSD1306_WHITE);
  else if (currentMood == MOOD_SLEEPY) display.drawBitmap(110,0,bmp_zzz,16,16,SSD1306_WHITE);
  else if (currentMood == MOOD_ANGRY) display.drawBitmap(56,0,bmp_anger,16,16,SSD1306_WHITE);
  drawUltraProEye(leftEye, true);
  drawUltraProEye(rightEye, false);
}

void drawClock() {
  struct tm t;
  if (!getLocalTime(&t)) { display.setCursor(30,30); display.print("Syncing..."); return; }
  String ampm = (t.tm_hour>=12)?"PM":"AM";
  int h12 = t.tm_hour%12; if(h12==0) h12=12;
  display.setFont(NULL); display.setCursor(114,0); display.print(ampm);
  display.setFont(&FreeSansBold18pt7b);
  char timeStr[6]; sprintf(timeStr,"%02d:%02d",h12,t.tm_min);
  int16_t x1,y1; uint16_t w,h;
  display.getTextBounds(timeStr,0,0,&x1,&y1,&w,&h);
  display.setCursor((SCREEN_WIDTH-w)/2,42);
  display.print(timeStr);
  display.setFont(&FreeSans9pt7b);
  char dateStr[20]; strftime(dateStr,20,"%a, %b %d",&t);
  display.getTextBounds(dateStr,0,0,&x1,&y1,&w,&h);
  display.setCursor((SCREEN_WIDTH-w)/2,62);
  display.print(dateStr);
}

void drawWeatherCard() {
  if (WiFi.status() != WL_CONNECTED) { display.setCursor(0,0); display.print("No WiFi"); return; }
  display.drawBitmap(96,0,getBigIcon(weatherMain),32,32,SSD1306_WHITE);
  display.setFont(&FreeSansBold9pt7b);
  String c = city; c.toUpperCase();
  display.setCursor(0,14);
  if(c.length()>9) c = c.substring(0,8)+".";
  display.print(c);
  display.setFont(&FreeSansBold18pt7b);
  int tempInt = (int)temperature;
  display.setCursor(0,48);
  display.print(tempInt);
  int16_t x1,y1; uint16_t w,h;
  display.getTextBounds(String(tempInt).c_str(),0,48,&x1,&y1,&w,&h);
  display.fillCircle(x1+w+5,26,4,SSD1306_WHITE);
  display.setFont(NULL);
  display.drawBitmap(88,32,bmp_tiny_drop,8,8,SSD1306_WHITE);
  display.setCursor(100,32); display.print(humidity); display.print("%");
  display.setCursor(88,45); display.print("~"); display.print((int)feelsLike);
  display.drawLine(0,52,128,52,SSD1306_WHITE);
  display.setCursor(0,55); display.print(weatherDesc);
}

void drawWorldClock() {
  time_t now; time(&now);
  time_t india = now + 5*3600 + 30*60;
  time_t sydney = now + 11*3600;
  struct tm* itm = gmtime(&india);
  struct tm* stm = gmtime(&sydney);
  display.fillRect(0,0,128,16,SSD1306_WHITE);
  display.setFont(NULL); display.setTextColor(SSD1306_BLACK);
  display.setCursor(32,4); display.print("WORLD CLOCK");
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(64,18,64,54,SSD1306_WHITE);
  display.setFont(NULL); display.setCursor(16,22); display.print("INDIA");
  display.setFont(&FreeSansBold9pt7b);
  char iStr[10]; sprintf(iStr,"%02d:%02d",itm->tm_hour,itm->tm_min);
  display.setCursor(5,46); display.print(iStr);
  display.setFont(NULL); display.setCursor(78,22); display.print("SYDNEY");
  display.setFont(&FreeSansBold9pt7b);
  char sStr[10]; sprintf(sStr,"%02d:%02d",stm->tm_hour,stm->tm_min);
  display.setCursor(69,46); display.print(sStr);
  display.setFont(NULL); display.setCursor(35,56); display.print("Tap to Exit");
}

void drawForecastPage() {
  display.fillRect(0,0,128,16,SSD1306_WHITE);
  display.setFont(NULL); display.setTextColor(SSD1306_BLACK);
  display.setCursor(20,4); display.print("3-DAY FORECAST");
  display.setTextColor(SSD1306_WHITE);
  display.drawLine(42,16,42,64,SSD1306_WHITE);
  display.drawLine(85,16,85,64,SSD1306_WHITE);
  for(int i=0;i<3;i++) {
    int xStart = i*43, cx = xStart+21;
    display.setFont(NULL);
    String d = fcast[i].dayName; if(d=="") d="Wait";
    display.setCursor(cx - d.length()*3, 20); display.print(d);
    display.drawBitmap(cx-8,28,getMiniIcon(fcast[i].iconType),16,16,SSD1306_WHITE);
    display.setFont(&FreeSansBold9pt7b);
    int16_t x1,y1; uint16_t w,h;
    display.getTextBounds(String(fcast[i].temp).c_str(),0,0,&x1,&y1,&w,&h);
    display.setCursor(cx - w/2 -2, 60); display.print(fcast[i].temp);
    display.fillCircle(cx + w/2 +1, 52, 2, SSD1306_WHITE);
  }
}

// ========== BOOT ANIMATION ==========
void playBootAnimation() {
  int cx=64, cy=32;
  for(int r=0; r<80; r+=4) {
    display.clearDisplay();
    display.fillCircle(cx,cy,r,SSD1306_WHITE);
    display.display();
    delay(10);
  }
  for(int r=0; r<80; r+=4) {
    display.clearDisplay();
    display.fillCircle(cx,cy,80,SSD1306_WHITE);
    display.fillCircle(cx,cy,r,SSD1306_BLACK);
    display.display();
    delay(10);
  }
  display.setFont(&FreeSansBold9pt7b);
  String boot = "Dasai Mochi";
  int16_t x1,y1; uint16_t w,h;
  display.getTextBounds(boot,0,0,&x1,&y1,&w,&h);
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH-w)/2,36);
  display.print(boot);
  display.display();
  delay(2000);
}

// ========== SETUP & LOOP ==========
void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(TOUCH_PIN, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();

  // Force config if touch held at boot for 3 sec
  bool forceConfig = false;
  for(unsigned long t=millis(); millis()-t < 3000; ) {
    if(digitalRead(TOUCH_PIN)) { forceConfig=true; break; }
    delay(80);
  }

  loadConfig();
  if(forceConfig) {
    startConfigPortal();
    return;
  }

  leftEye.init(18,14,36,36);
  rightEye.init(74,14,36,36);
  playBootAnimation();

  display.clearDisplay();
  display.setCursor(40,30);
  display.print("connecting");
  display.display();
  WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
  unsigned long start = millis();
  while(WiFi.status() != WL_CONNECTED && millis()-start < 15000) delay(200);
  if(WiFi.status() != WL_CONNECTED) {
    startConfigPortal();
    return;
  }
  configTime(0,0,"pool.ntp.org");
  setenv("TZ", tzString.c_str(), 1);
  tzset();
  getWeatherAndForecast();
  lastWeatherUpdate = millis();
  lastPageSwitch = millis();
}

void loop() {
  if(inConfigMode) {
    configServer.handleClient();
    return;
  }
  unsigned long now = millis();
  handleTouch();
  if(now - lastWeatherUpdate > 600000) {
    getWeatherAndForecast();
    lastWeatherUpdate = now;
  }
  if(currentPage < 3 && now - lastPageSwitch > PAGE_INTERVAL) {
    currentPage++;
    if(currentPage > 2) currentPage = 0;
    lastPageSwitch = now;
    lastSaccade = 0;
  }
  display.clearDisplay();
  switch(currentPage) {
    case 0: drawEmoPage(); break;
    case 1: drawClock(); break;
    case 2: drawWeatherCard(); break;
    case 3: drawWorldClock(); break;
    case 4: drawForecastPage(); break;
  }
  display.display();
}