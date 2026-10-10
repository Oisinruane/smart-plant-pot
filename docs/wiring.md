# Wiring

## Pin map (Wemos D1 mini)

| D1 mini | GPIO | Connects to | Notes |
|---|---|---|---|
| 5V | – | LED ring + (anodes) | USB 5 V, unregulated |
| 3V3 | – | OLED VCC, sensor VCC | |
| G | – | All grounds, MOSFET source | |
| A0 | ADC | Sensor AOUT | On-board 220k/100k divider gives a 0–3.2 V range |
| D2 | 4 | OLED SDA | Default I2C |
| D1 | 5 | OLED SCL | Default I2C |
| D5 | 14 | MOSFET gate via 100 Ω | Not a boot-strap pin, so the ring stays dark at boot |

Avoid D3, D4 and D8: they are boot-strap pins and a MOSFET gate can stop the board booting.

## Diagram

```
                USB 5V
                  │
     ┌────────────┼──────────────────────────────┐
     │        D1 mini                            │
     │   5V ──────────────┬──────────────────────┘
     │                    │  (+470 µF across ring supply)
     │                    ├──[120Ω]──▶|── ┐
     │                    ├──[120Ω]──▶|── ┤   16 × white SMD LED,
     │                    │     …  ×16    ┤   one resistor each
     │                    └──[120Ω]──▶|── ┤
     │                                    │ RING_K
     │                                  D │
     │   D5 ──[100Ω]──────────────────G──┤ AO3400A (N-MOSFET)
     │                     │            S │
     │                  [100k]            │
     │                     │              │
     │   G ────────────────┴──────────────┴──── GND
     │
     │   3V3 ──┬────────── OLED VCC
     │         └────────── Sensor VCC
     │   D2 ────────────── OLED SDA
     │   D1 ────────────── OLED SCL
     │   A0 ────────────── Sensor AOUT
     └────────────────────────────────────────────
```

## LED ring

Same style of LEDs as the helping hand ring: small white SMD LEDs, forward voltage about 3.0–3.2 V.

- Supply from **5 V**. 3.3 V leaves almost no headroom, so brightness would vary from LED to LED.
- **One resistor per LED.** Two white LEDs in series need more than 5 V, and paralleling LEDs on a shared resistor lets the lowest-Vf LED hog the current.
- Resistor: (5 − 3.1) V / 15 mA ≈ **120 Ω**, so about 15 mA per LED and about 240 mA for the ring. Use 150 Ω for a dimmer, cooler ring.
- Switch the low side with a logic-level N-MOSFET. **AO3400A** (SOT-23, fully on at 2.5 V gate drive) for the PCB. On a breadboard, use an AO3400 breakout or an IRLB8721 (TO-220). A 2N7000 drops too much voltage at this current with a 3.3 V gate.
- 100 kΩ gate pull-down keeps the ring off while the ESP8266 boots. 100 Ω in series with the gate limits ringing.
- PWM runs at 2 kHz, which is fast enough to be flicker-free on camera and slow enough for clean MOSFET switching without a gate driver.

## Moisture sensor (capacitive v1.2)

- Check the timer IC: **TLC555** is fine at 3.3 V. **NE555** needs about 5 V, but then AOUT can exceed the D1 mini's 3.2 V A0 range. Prefer a TLC555 board.
- Some v1.2 boards have R4 (1 MΩ bleed resistor) not connected to ground, which makes readings drift slowly. If readings creep, bridge R4's ground side to GND.
- Seal the top electronics with conformal coat or heat shrink. The board edge isn't waterproof.

## Power budget

| Load | Typical | Peak |
|---|---|---|
| ESP8266 (WiFi on) | 80 mA | 350 mA |
| LED ring (breathing, max duty 70%) | ~80 mA average | 240 mA (PWM on-pulse) |
| OLED | 15 mA | 25 mA |
| Sensor | 5 mA | 5 mA |
| **Total** | **~180 mA** | **~620 mA** |

Any 1 A USB supply covers this.
