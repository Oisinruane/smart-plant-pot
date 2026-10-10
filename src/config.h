#pragma once

// D1 mini pin mapping (see docs/wiring.md)
constexpr int PIN_SENSOR = A0;
constexpr int PIN_LED_RING = D5;  // GPIO14, not a boot-strap pin
// OLED uses the default I2C pins: SDA = D2 (GPIO4), SCL = D1 (GPIO5)

constexpr int OLED_WIDTH = 128;
constexpr int OLED_HEIGHT = 64;
constexpr uint8_t OLED_ADDR = 0x3C;

// Calibrate per sensor: raw reading in open air (dry) and in a glass of water (wet).
constexpr int SENSOR_DRY_RAW = 620;
constexpr int SENSOR_WET_RAW = 280;

constexpr int WATER_ON_BELOW_PCT = 30;
constexpr int WATER_OFF_ABOVE_PCT = 40;

constexpr uint32_t SAMPLE_INTERVAL_MS = 1000;

constexpr uint32_t BREATH_PERIOD_MS = 4000;
constexpr int PWM_RANGE = 1023;
constexpr int LED_MAX_DUTY = 700;  // keeps the ring below full current
constexpr uint32_t PWM_FREQ_HZ = 2000;
