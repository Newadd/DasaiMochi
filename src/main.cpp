#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_random.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define TOUCH_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int lastTouchState = LOW;
unsigned long lastTapTime = 0;
int tapCount = 0;

enum Expression { HAPPY, SAD, SHOCKED, SLEEPY };
Expression currentExpr = HAPPY;
Expression targetExpr = HAPPY;
float transitionProgress = 1.0;
unsigned long transitionStart = 0;
const int transitionDuration = 300;

float eyeLid = 1.0;
bool isBlinking = false;
unsigned long lastBlink = 0;
float heartbeat = 0;

struct Particle {
  int x, y;
  int life;
};
Particle particles[10];
int particleCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT_PULLDOWN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED gagal");
    for (;;);
  }
  display.clearDisplay();
  display.display();

  for (int i = 0; i < 10; i++) particles[i].life = 0;
}

void loop() {
  readTouch();
  updateEffects();
  drawMochiEyes();
  delay(30);
}

void readTouch() {
  int state = digitalRead(TOUCH_PIN);
  if (state == HIGH && lastTouchState == LOW) {
    unsigned long now = millis();
    if (now - lastTapTime < 300) tapCount++;
    else tapCount = 1;
    lastTapTime = now;

    if (tapCount == 1) setExpression(HAPPY);
    else if (tapCount == 2) setExpression(SAD);
    else if (tapCount == 3) setExpression(SHOCKED);
    else if (tapCount >= 4) {
      setExpression(SLEEPY);
      tapCount = 0;
    }
  }
  lastTouchState = state;
}

void setExpression(Expression newExpr) {
  if (newExpr == currentExpr && newExpr == targetExpr) return;
  targetExpr = newExpr;
  transitionProgress = 0.0;
  transitionStart = millis();
  currentExpr = newExpr;
  if (newExpr == HAPPY) spawnParticles(8);
}

void updateEffects() {
  // Kedip random
  if (!isBlinking && millis() - lastBlink > (esp_random() % 3000 + 2000)) {
    isBlinking = true;
    lastBlink = millis();
  }
  if (isBlinking) {
    float t = (millis() - lastBlink) / 100.0;
    if (t >= 1.0) {
      isBlinking = false;
      eyeLid = 1.0;
    } else {
      eyeLid = 1.0 - sin(t * 3.14159);
      if (eyeLid < 0) eyeLid = 0;
      if (eyeLid > 1) eyeLid = 1;
    }
  } else {
    eyeLid = 1.0;
  }

  // Denyut jantung
  heartbeat = 0.7 + 0.3 * sin(millis() / 500.0);

  // Transisi antar ekspresi
  if (transitionProgress < 1.0) {
    transitionProgress = (millis() - transitionStart) / (float)transitionDuration;
    if (transitionProgress > 1.0) transitionProgress = 1.0;
  }

  // Update partikel
  for (int i = 0; i < particleCount; i++) {
    particles[i].life--;
    if (particles[i].life <= 0) {
      for (int j = i; j < particleCount - 1; j++) particles[j] = particles[j + 1];
      particleCount--;
      i--;
    }
  }
}

void spawnParticles(int count) {
  for (int i = 0; i < count && particleCount < 10; i++) {
    particles[particleCount].x = esp_random() % (SCREEN_WIDTH - 40) + 20;
    particles[particleCount].y = esp_random() % (SCREEN_HEIGHT - 40) + 20;
    particles[particleCount].life = esp_random() % 30 + 20;
    particleCount++;
  }
}

void drawMochiEyes() {
  display.clearDisplay();

  // Gambar partikel
  for (int i = 0; i < particleCount; i++) {
    display.drawPixel(particles[i].x, particles[i].y, SSD1306_WHITE);
  }

  int leftEyeX = 35, rightEyeX = 93, eyeY = 32;
  float scale = heartbeat;
  int radius = 12 * (0.8 + 0.2 * scale);

  // Efek transisi (mata membesar/mengecil)
  if (transitionProgress < 0.5 && transitionProgress != 1.0) {
    radius = radius * (1 + (1 - transitionProgress * 2) * 0.5);
  } else if (transitionProgress >= 0.5 && transitionProgress != 1.0) {
    radius = radius * (1 + ((transitionProgress * 2) - 1) * 0.5);
  }

  Expression expr = (transitionProgress < 1.0) ? targetExpr : currentExpr;

  int eyeHeight = radius * 2 * eyeLid;
  if (eyeHeight < 2) eyeHeight = 2;

  switch (expr) {
    case HAPPY:
      display.fillCircle(leftEyeX, eyeY, radius, SSD1306_WHITE);
      display.fillCircle(rightEyeX, eyeY, radius, SSD1306_WHITE);
      display.fillCircle(leftEyeX, eyeY + 2, radius / 2, SSD1306_BLACK);
      display.fillCircle(rightEyeX, eyeY + 2, radius / 2, SSD1306_BLACK);
      display.drawLine(leftEyeX - 12, eyeY - 10, leftEyeX + 12, eyeY - 6, SSD1306_WHITE);
      display.drawLine(rightEyeX - 12, eyeY - 6, rightEyeX + 12, eyeY - 10, SSD1306_WHITE);
      break;
    case SAD:
      display.fillCircle(leftEyeX, eyeY, radius, SSD1306_WHITE);
      display.fillCircle(rightEyeX, eyeY, radius, SSD1306_WHITE);
      display.fillCircle(leftEyeX, eyeY + radius / 3, radius / 2, SSD1306_BLACK);
      display.fillCircle(rightEyeX, eyeY + radius / 3, radius / 2, SSD1306_BLACK);
      display.drawLine(leftEyeX - 12, eyeY - 8, leftEyeX + 8, eyeY - 4, SSD1306_WHITE);
      display.drawLine(rightEyeX - 8, eyeY - 4, rightEyeX + 12, eyeY - 8, SSD1306_WHITE);
      break;
    case SHOCKED:
      display.fillCircle(leftEyeX, eyeY, radius + 3, SSD1306_WHITE);
      display.fillCircle(rightEyeX, eyeY, radius + 3, SSD1306_WHITE);
      display.fillCircle(leftEyeX, eyeY, radius / 2, SSD1306_BLACK);
      display.fillCircle(rightEyeX, eyeY, radius / 2, SSD1306_BLACK);
      display.drawLine(leftEyeX - 10, eyeY - 12, leftEyeX + 10, eyeY - 12, SSD1306_WHITE);
      display.drawLine(rightEyeX - 10, eyeY - 12, rightEyeX + 10, eyeY - 12, SSD1306_WHITE);
      break;
    case SLEEPY:
      display.drawLine(leftEyeX - 10, eyeY, leftEyeX + 10, eyeY, SSD1306_WHITE);
      display.drawLine(rightEyeX - 10, eyeY, rightEyeX + 10, eyeY, SSD1306_WHITE);
      display.drawLine(leftEyeX - 12, eyeY - 4, leftEyeX + 12, eyeY - 2, SSD1306_WHITE);
      display.drawLine(rightEyeX - 12, eyeY - 2, rightEyeX + 12, eyeY - 4, SSD1306_WHITE);
      break;
  }

  // Efek kedip (tutup mata)
  if (eyeLid < 0.95) {
    display.fillRect(leftEyeX - radius, eyeY - radius, radius * 2, eyeHeight, SSD1306_BLACK);
    display.fillRect(rightEyeX - radius, eyeY - radius, radius * 2, eyeHeight, SSD1306_BLACK);
  }

  // Efek glow
  for (int i = 0; i < 2; i++) {
    display.drawCircle(leftEyeX, eyeY, radius + 1 + i, SSD1306_WHITE);
    display.drawCircle(rightEyeX, eyeY, radius + 1 + i, SSD1306_WHITE);
  }

  display.display();
}
