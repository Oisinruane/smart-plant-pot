# Process flow

```
Soil ──▶ Capacitive sensor ──AOUT──▶ A0 (0–1023)
                                        │
                                        ▼
                         Smoother (EMA, α = 0.2), every 1 s
                                        │
                                        ▼
                  moisturePercent(raw, DRY_RAW, WET_RAW) → 0–100 %
                                        │
          ┌─────────────────────────────┼────────────────────────────┐
          ▼                             ▼                            ▼
   OLED: "NN%" + bar +         WaterAlert hysteresis         GET /api/moisture
   "Water me!" / "Moisture OK"  on < 30 %, off > 40 %         (current level, JSON)
                                        │
                                        ▼
                         LED ring: breathDuty() PWM on D5
                         (off when alert inactive)
```

## Steps

1. **Sample**: `analogRead(A0)` once a second, smoothed with an exponential moving average to remove ADC noise.
2. **Convert**: map raw to a percentage using the dry (air) and wet (water) calibration points in `config.h`. Capacitive sensors read *lower* when wetter, so the mapping is inverted and clamped to 0–100 %.
3. **Decide**: `WaterAlert` switches on below 30 % and only switches off above 40 %, so the ring doesn't flicker on and off around one threshold.
4. **Show**: the OLED redraws after every sample.
5. **Breathe**: every loop pass, `breathDuty()` computes the PWM duty from `millis()` using an `exp(sin)` curve (4 s period, capped at 70 % duty). The loop never blocks, so the breathing stays smooth while WiFi runs.
6. **Serve**: with WiFi configured, `http://plant-pot.local/api/moisture` returns the current reading. There's no web page and no stored history; the homelab dashboard polls this endpoint and keeps the history itself.

```bash
$ curl http://plant-pot.local/api/moisture
{"moisture_pct":54,"raw":437,"needs_water":false}
```

Firmware updates go over WiFi (ArduinoOTA) after the first USB flash.

## Calibration

1. Flash, open the serial monitor (`pio device monitor`).
2. Hold the sensor in air and note `raw` → `SENSOR_DRY_RAW`.
3. Put it in a glass of water up to the line and note `raw` → `SENSOR_WET_RAW`.
4. Re-flash.
