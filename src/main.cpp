#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h>

#include "config.h"
#include "plant_logic.h"
#include "secrets.h"

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
ESP8266WebServer server(80);

Smoother smoother(0.2f);
WaterAlert alert(WATER_ON_BELOW_PCT, WATER_OFF_ABOVE_PCT);

int currentPct = 0;
int currentRaw = 0;

uint32_t lastSample = 0;

bool wifiEnabled() { return sizeof(WIFI_SSID) > 1; }

void drawDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(4);
  String text = String(currentPct) + "%";
  int16_t x, y;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x, &y, &w, &h);
  display.setCursor((OLED_WIDTH - w) / 2, 0);
  display.print(text);

  const int barY = 38, barH = 8;
  display.drawRect(0, barY, OLED_WIDTH, barH, SSD1306_WHITE);
  display.fillRect(2, barY + 2, (OLED_WIDTH - 4) * currentPct / 100, barH - 4, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, OLED_HEIGHT - 8);
  display.print(alert.active() ? "Water me!" : "Moisture OK");
  if (wifiEnabled() && WiFi.status() == WL_CONNECTED) {
    display.setCursor(OLED_WIDTH - 24, OLED_HEIGHT - 8);
    display.print("WiFi");
  }
  display.display();
}

void handleApi() {
  String json = "{\"moisture_pct\":" + String(currentPct) +
                ",\"raw\":" + String(currentRaw) +
                ",\"needs_water\":" + (alert.active() ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void setupNetwork() {
  if (!wifiEnabled()) return;
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  MDNS.begin(HOSTNAME);
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();
  server.on("/api/moisture", handleApi);
  server.begin();
}

void scanI2C() {
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) Serial.printf("I2C device at 0x%02X\n", addr);
  }
}

void sample() {
  currentRaw = lroundf(smoother.update(analogRead(PIN_SENSOR)));
  currentPct = moisturePercent(currentRaw, SENSOR_DRY_RAW, SENSOR_WET_RAW);
  alert.update(currentPct);
  Serial.printf("raw=%d pct=%d water=%d\n", currentRaw, currentPct, alert.active());
  drawDisplay();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_RING, OUTPUT);
  analogWriteRange(PWM_RANGE);
  analogWriteFreq(PWM_FREQ_HZ);
  analogWrite(PIN_LED_RING, 0);

  Wire.begin();
  scanI2C();
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
  }

  setupNetwork();
  sample();
  lastSample = millis();
}

void loop() {
  uint32_t now = millis();

  if (now - lastSample >= SAMPLE_INTERVAL_MS) {
    lastSample = now;
    sample();
  }

  analogWrite(PIN_LED_RING, alert.active() ? breathDuty(now, BREATH_PERIOD_MS, LED_MAX_DUTY) : 0);

  if (wifiEnabled()) {
    server.handleClient();
    MDNS.update();
    ArduinoOTA.handle();
  }
}
