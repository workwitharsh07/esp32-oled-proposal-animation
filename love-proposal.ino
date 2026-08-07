/*
  ================================================================
   "SAY YES" -- Cinematic Proposal Animation
   Galaxy-particle intro -> ring box reveal -> "Will you marry me?"
  ================================================================
   Board:      ESP32 (DevKit / 38-pin or similar)
   Display:    128x64 SH1106 OLED, I2C
   Libraries:  Adafruit_GFX, Adafruit_SH110X

   100% procedurally drawn -- no bitmaps, no images.

   Sequence:
     1. Starburst flash + swirling particle galaxy
     2. Particles converge into a pulsing heart
     3. Heart bursts apart -> transitions to a ring box
     4. Box grows in, lid opens, ring rises with sparkle
     5. Final screen: "Will you marry me?" + heart (holds forever)

   Afxlabz
  ================================================================
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <string.h>

// ---------------- Display ----------------
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define OLED_ADDR      0x3C

// Adjust these if your board wires I2C differently
// (some 38-pin ESP32 DevKits are happier on GPIO4/5 -- swap here if the
// display doesn't init)
#define OLED_SDA       21
#define OLED_SCL       22

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- Timing (ms) ----------------
#define FRAME_DELAY           20
#define FLASH_DURATION        450
#define FORM_DURATION         3200
#define HOLD_DURATION         1400
#define BURST_DURATION        900
#define BOX_APPEAR_DURATION   900
#define LID_OPEN_DURATION     1600
#define RING_RISE_DURATION    1300
#define SPARKLE_DURATION      2200

// ---------------- Intro heart / particles ----------------
#define NUM_PARTICLES  50
#define HEART_CX       64
#define HEART_CY       30
#define HEART_SCALE    1.4f

// ---------------- Background stars ----------------
#define NUM_STARS      18

// ---------------- Ring box geometry ----------------
#define BOX_CX         64
#define BOX_BOTTOM     56
#define BOX_W          54
#define BOX_H          24
#define BOX_LEFT       (BOX_CX - BOX_W / 2)
#define BOX_RIGHT      (BOX_CX + BOX_W / 2)
#define HINGE_Y        (BOX_BOTTOM - BOX_H)

#define LID_THICK        5
#define LID_ARM           24.0f
#define LID_MAX_ANGLE      1.92f   // ~110 degrees

#define RING_RX        11
#define RING_RY        4
#define RING_REST_OFFSET 10

// ---------------- State machine ----------------
enum Stage {
  STAGE_FLASH,
  STAGE_FORM,
  STAGE_HOLD,
  STAGE_BURST,
  STAGE_BOX_APPEAR,
  STAGE_LID_OPEN,
  STAGE_RING_RISE,
  STAGE_SPARKLE,
  STAGE_FINAL
};

Stage stage;
uint32_t stageStart;

float angle0[NUM_PARTICLES];
float radius0[NUM_PARTICLES];
float targetX[NUM_PARTICLES];
float targetY[NUM_PARTICLES];

int16_t starX[NUM_STARS];
int16_t starY[NUM_STARS];

// ================= Easing helpers =================
float easeOutCubic(float x) {
  float f = 1.0f - x;
  return 1.0f - f * f * f;
}

float easeInOutCubic(float x) {
  if (x < 0.5f) return 4.0f * x * x * x;
  float f = -2.0f * x + 2.0f;
  return 1.0f - (f * f * f) / 2.0f;
}

float easeOutBack(float x) {
  const float c1 = 1.70158f;
  const float c3 = c1 + 1.0f;
  float f = x - 1.0f;
  return 1.0f + c3 * f * f * f + c1 * f * f;
}

float lerpF(float a, float b, float t) {
  return a + (b - a) * t;
}

// ================= Generic drawing helpers =================
void drawEllipse(int16_t cx, int16_t cy, int16_t rx, int16_t ry) {
  const int SEGMENTS = 28;
  int16_t px = cx + rx;
  int16_t py = cy;
  for (int i = 1; i <= SEGMENTS; i++) {
    float t = (TWO_PI * i) / SEGMENTS;
    int16_t x = cx + (int16_t)(rx * cos(t));
    int16_t y = cy + (int16_t)(ry * sin(t));
    display.drawLine(px, py, x, y, SH110X_WHITE);
    px = x;
    py = y;
  }
}

void drawFilledHeart(int16_t cx, int16_t cy, int16_t size) {
  if (size < 2) return;
  int16_t r = size / 2;
  display.fillCircle(cx - r, cy, r, SH110X_WHITE);
  display.fillCircle(cx + r, cy, r, SH110X_WHITE);
  display.fillTriangle(cx - size, cy, cx + size, cy, cx, cy + size, SH110X_WHITE);
}

void drawSparkle(int16_t x, int16_t y, int16_t len) {
  if (len < 1) return;
  display.drawFastVLine(x, y - len, 2 * len + 1, SH110X_WHITE);
  display.drawFastHLine(x - len, y, 2 * len + 1, SH110X_WHITE);
  int16_t d = len / 2 + 1;
  display.drawLine(x - d, y - d, x + d, y + d, SH110X_WHITE);
  display.drawLine(x - d, y + d, x + d, y - d, SH110X_WHITE);
}

void drawStarfield(uint32_t nowMs) {
  for (int i = 0; i < NUM_STARS; i++) {
    int phase = (int)((nowMs / 150 + i * 13) % 20);
    if (phase < 15) {
      display.drawPixel(starX[i], starY[i], SH110X_WHITE);
    }
  }
}

// ================= Ring box drawing =================
void drawBoxBody(int16_t topY, int16_t h) {
  if (h < 1) return;
  int16_t r = (h < 10) ? 2 : 4;
  display.fillRoundRect(BOX_LEFT, topY, BOX_W, h, r, SH110X_WHITE);
  if (h > 10) {
    int16_t seamY = topY + (int16_t)(h * 0.32f);
    display.drawFastHLine(BOX_LEFT + 3, seamY, BOX_W - 6, SH110X_BLACK);
    display.fillCircle(BOX_CX, seamY, 1, SH110X_BLACK);
  }
}

void drawLidAt(float liftH) {
  int16_t lidBottom = HINGE_Y - (int16_t)liftH;
  int16_t lidTop = lidBottom - LID_THICK;
  if (liftH > 1) {
    display.drawLine(BOX_LEFT + 4, HINGE_Y, BOX_LEFT + 4, lidBottom, SH110X_WHITE);
    display.drawLine(BOX_RIGHT - 4, HINGE_Y, BOX_RIGHT - 4, lidBottom, SH110X_WHITE);
  }
  display.fillRoundRect(BOX_LEFT, lidTop, BOX_W, LID_THICK, 2, SH110X_WHITE);
}

void drawRing(int16_t y) {
  drawEllipse(BOX_CX, y, RING_RX, RING_RY);
  drawEllipse(BOX_CX, y, RING_RX - 1, RING_RY);

  int16_t gemY = y - RING_RY - 3;
  display.fillTriangle(BOX_CX - 3, gemY, BOX_CX + 3, gemY, BOX_CX, gemY - 4, SH110X_WHITE);
  display.fillTriangle(BOX_CX - 3, gemY, BOX_CX + 3, gemY, BOX_CX, gemY + 3, SH110X_WHITE);

  if ((millis() / 180) % 5 == 0) {
    display.drawPixel(BOX_CX + 5, gemY - 3, SH110X_WHITE);
    display.drawPixel(BOX_CX - 5, gemY + 2, SH110X_WHITE);
  }
}

void drawGroundGlow() {
  drawEllipse(BOX_CX, BOX_BOTTOM + 3, BOX_W / 2 + 5, 3);
}

// ================= Stage updates =================
void updateFlash(uint32_t elapsed) {
  float p = constrain(elapsed / (float)FLASH_DURATION, 0.0f, 1.0f);
  for (int k = 0; k < 3; k++) {
    float kp = constrain(p - k * 0.12f, 0.0f, 1.0f);
    if (kp <= 0.0f) continue;
    int16_t rad = (int16_t)(kp * 46.0f);
    if (rad > 0) display.drawCircle(HEART_CX, HEART_CY, rad, SH110X_WHITE);
  }
}

void updateForm(uint32_t elapsed) {
  float p = constrain(elapsed / (float)FORM_DURATION, 0.0f, 1.0f);
  float w = easeInOutCubic(p);
  for (int i = 0; i < NUM_PARTICLES; i++) {
    float orbitR = radius0[i] * (1.0f - p);
    float orbitA = angle0[i] + p * 9.0f;
    float sx = HEART_CX + orbitR * cos(orbitA);
    float sy = HEART_CY + orbitR * sin(orbitA) * 0.7f;
    float x = sx * (1.0f - w) + targetX[i] * w;
    float y = sy * (1.0f - w) + targetY[i] * w;
    int16_t ix = (int16_t)round(x);
    int16_t iy = (int16_t)round(y);
    display.drawPixel(ix, iy, SH110X_WHITE);
    if (i % 6 == 0) {
      display.drawPixel(ix + 1, iy, SH110X_WHITE);
      display.drawPixel(ix, iy + 1, SH110X_WHITE);
    }
  }
}

void updateHold(uint32_t elapsed) {
  float pulse = 1.0f + 0.06f * sin(elapsed / 220.0f);
  for (int i = 0; i < NUM_PARTICLES; i++) {
    float x = HEART_CX + (targetX[i] - HEART_CX) * pulse;
    float y = HEART_CY + (targetY[i] - HEART_CY) * pulse;
    int16_t ix = (int16_t)round(x);
    int16_t iy = (int16_t)round(y);
    display.drawPixel(ix, iy, SH110X_WHITE);
    if (i % 6 == 0) display.drawPixel(ix + 1, iy, SH110X_WHITE);
  }
}

void updateBurst(uint32_t elapsed) {
  float p = constrain(elapsed / (float)BURST_DURATION, 0.0f, 1.0f);
  float explode = 1.0f + p * 4.0f;
  for (int i = 0; i < NUM_PARTICLES; i++) {
    float dx = targetX[i] - HEART_CX;
    float dy = targetY[i] - HEART_CY;
    float x = HEART_CX + dx * explode;
    float y = HEART_CY + dy * explode;
    if ((float)random(0, 100) > p * 100.0f) {
      display.drawPixel((int16_t)round(x), (int16_t)round(y), SH110X_WHITE);
    }
  }
  int16_t rad = (int16_t)(p * 60.0f);
  if (rad > 0 && rad < 64) display.drawCircle(HEART_CX, HEART_CY, rad, SH110X_WHITE);
}

void updateBoxAppear(uint32_t elapsed) {
  float p = easeOutCubic(constrain(elapsed / (float)BOX_APPEAR_DURATION, 0.0f, 1.0f));
  int16_t h = (int16_t)(BOX_H * p);
  int16_t topY = BOX_BOTTOM - h;
  drawBoxBody(topY, h);
  if (p > 0.5f) drawGroundGlow();
}

void updateLidOpen(uint32_t elapsed) {
  drawBoxBody(HINGE_Y, BOX_H);
  drawGroundGlow();
  float p = easeOutCubic(constrain(elapsed / (float)LID_OPEN_DURATION, 0.0f, 1.0f));
  float theta = p * LID_MAX_ANGLE;
  float liftH = LID_ARM * sin(theta);
  drawLidAt(liftH);
}

void updateRingRise(uint32_t elapsed) {
  drawBoxBody(HINGE_Y, BOX_H);
  drawGroundGlow();
  float liftFinal = LID_ARM * sin(LID_MAX_ANGLE);
  drawLidAt(liftFinal);

  float p = constrain(elapsed / (float)RING_RISE_DURATION, 0.0f, 1.0f);
  float pe = easeOutBack(p);
  float ringY = lerpF(HINGE_Y, HINGE_Y - RING_REST_OFFSET, pe);
  drawRing((int16_t)round(ringY));
}

void sparkleWindow(uint32_t elapsed, uint32_t start, uint32_t end, int16_t baseY, int16_t offX) {
  if (elapsed < start || elapsed > end) return;
  float wp = (elapsed - start) / (float)(end - start);
  float amp = sin(wp * PI);
  int16_t len = (int16_t)(2 + amp * 4);
  drawSparkle(BOX_CX + offX, baseY - RING_RY - 6, len);
}

void updateSparkleHold(uint32_t elapsed) {
  drawBoxBody(HINGE_Y, BOX_H);
  drawGroundGlow();
  float liftFinal = LID_ARM * sin(LID_MAX_ANGLE);
  drawLidAt(liftFinal);

  int16_t ringY = HINGE_Y - RING_REST_OFFSET;
  drawRing(ringY);

  sparkleWindow(elapsed, 300, 550, ringY, 0);
  sparkleWindow(elapsed, 1000, 1250, ringY, -6);
  sparkleWindow(elapsed, 1650, 1900, ringY, 6);
}

void updateFinal(uint32_t elapsed) {
  const char* msg = "Will you marry me?";
  int16_t len = (int16_t)strlen(msg);
  const uint32_t typeMs = 45;

  uint32_t shownU = elapsed / typeMs;
  if (shownU > (uint32_t)len) shownU = (uint32_t)len;
  int16_t shown = (int16_t)shownU;

  char buf[24];
  strncpy(buf, msg, shown);
  buf[shown] = '\0';

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  int16_t x1, y1;
  uint16_t tw, th;
  display.getTextBounds(buf, 0, 0, &x1, &y1, &tw, &th);
  int16_t cx = (SCREEN_WIDTH - tw) / 2 - x1;
  display.setCursor(cx, 26);
  display.print(buf);

  if (shown >= len) {
    uint32_t doneFor = elapsed - (uint32_t)len * typeMs;
    float pulse = 1.0f + 0.15f * sin(doneFor / 500.0f);
    int16_t hs = (int16_t)(6.0f * pulse);
    drawFilledHeart(SCREEN_WIDTH / 2, 44, hs);
  }
}

// ================= Setup / Loop =================
void setup() {
  Serial.begin(115200);

  // Give OLED time to power up
  delay(1000);

  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(100000);   // 100kHz is more reliable than 400kHz

  delay(200);

  if (!display.begin(OLED_ADDR, true)) {
    Serial.println("OLED Init Failed!");
    while (1) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.display();

  randomSeed(esp_random());

  for (int i = 0; i < NUM_PARTICLES; i++) {
    angle0[i] = random(0, 6283) / 1000.0f;
    radius0[i] = 26.0f + random(0, 22);

    float t = (TWO_PI * i) / NUM_PARTICLES;
    float st = sin(t);
    float hx = 16.0f * st * st * st;
    float hy = 13.0f * cos(t) - 5.0f * cos(2 * t) - 2.0f * cos(3 * t) - cos(4 * t);

    targetX[i] = HEART_CX + hx * HEART_SCALE;
    targetY[i] = HEART_CY - hy * HEART_SCALE;
  }

  for (int i = 0; i < NUM_STARS; i++) {
    starX[i] = random(0, SCREEN_WIDTH);
    starY[i] = random(0, SCREEN_HEIGHT);
  }

  stage = STAGE_FLASH;
  stageStart = millis();

  Serial.println("Boot OK");
}

void loop() {
  uint32_t now = millis();
  uint32_t elapsed = now - stageStart;

  display.clearDisplay();

  switch (stage) {
    case STAGE_FLASH:
      drawStarfield(now);
      updateFlash(elapsed);
      if (elapsed > FLASH_DURATION) { stage = STAGE_FORM; stageStart = now; }
      break;

    case STAGE_FORM:
      drawStarfield(now);
      updateForm(elapsed);
      if (elapsed > FORM_DURATION) { stage = STAGE_HOLD; stageStart = now; }
      break;

    case STAGE_HOLD:
      drawStarfield(now);
      updateHold(elapsed);
      if (elapsed > HOLD_DURATION) { stage = STAGE_BURST; stageStart = now; }
      break;

    case STAGE_BURST:
      drawStarfield(now);
      updateBurst(elapsed);
      if (elapsed > BURST_DURATION) { stage = STAGE_BOX_APPEAR; stageStart = now; }
      break;

    case STAGE_BOX_APPEAR:
      updateBoxAppear(elapsed);
      if (elapsed > BOX_APPEAR_DURATION) { stage = STAGE_LID_OPEN; stageStart = now; }
      break;

    case STAGE_LID_OPEN:
      updateLidOpen(elapsed);
      if (elapsed > LID_OPEN_DURATION) { stage = STAGE_RING_RISE; stageStart = now; }
      break;

    case STAGE_RING_RISE:
      updateRingRise(elapsed);
      if (elapsed > RING_RISE_DURATION) { stage = STAGE_SPARKLE; stageStart = now; }
      break;

    case STAGE_SPARKLE:
      updateSparkleHold(elapsed);
      if (elapsed > SPARKLE_DURATION) { stage = STAGE_FINAL; stageStart = now; }
      break;

    case STAGE_FINAL:
      updateFinal(elapsed);
      // Sequence holds here permanently once reached.
      // To loop the whole animation again instead, uncomment:
      // if (elapsed > 6000) { stage = STAGE_FLASH; stageStart = now; }
      break;
  }

  display.display();
  delay(FRAME_DELAY);
}
