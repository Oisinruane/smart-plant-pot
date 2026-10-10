# Smart Plant Pot

A Wemos D1 mini plant pot. A capacitive sensor measures soil moisture, an OLED shows it as a percentage, and a 16-LED ring at the base breathes when the plant needs water. The current level is served as JSON over WiFi:

```bash
$ curl http://plant-pot.local/api/moisture
{"moisture_pct":54,"raw":437,"needs_water":false}
```

- What it is and the parts list: [docs/overview.md](docs/overview.md)
- Wiring: [docs/wiring.md](docs/wiring.md)
- Data flow and calibration: [docs/process_flow.md](docs/process_flow.md)

## Quick start

```bash
brew install platformio
cp src/secrets.example.h src/secrets.h   # add WiFi details, or leave SSID empty for offline
pio run -e d1_mini -t upload              # first flash over USB
pio device monitor                        # watch raw readings for calibration
pio run -e d1_mini_ota -t upload          # later flashes over WiFi
```

## Tests

```bash
g++ -std=c++17 -Wall -I src tests/test_plant_logic.cpp -o /tmp/plant_tests && /tmp/plant_tests
```

## Pins

| D1 mini | Connects to |
|---|---|
| A0 | Sensor AOUT |
| D2 / D1 | OLED SDA / SCL |
| D5 | LED ring MOSFET gate |
| 5V | LED ring supply |
| 3V3 | OLED, sensor |
