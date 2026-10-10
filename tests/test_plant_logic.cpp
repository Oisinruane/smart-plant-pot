#include <cassert>
#include <cstdio>

#include "plant_logic.h"

void testMoisturePercent() {
  assert(moisturePercent(620, 620, 280) == 0);
  assert(moisturePercent(280, 620, 280) == 100);
  assert(moisturePercent(450, 620, 280) == 50);
  assert(moisturePercent(900, 620, 280) == 0);
  assert(moisturePercent(100, 620, 280) == 100);
  assert(moisturePercent(500, 500, 500) == 0);
}

void testSmoother() {
  Smoother s(0.5f);
  assert(s.update(100) == 100);
  assert(s.update(200) == 150);
  assert(s.update(200) == 175);
}

void testWaterAlertHysteresis() {
  WaterAlert a(30, 40);
  assert(!a.update(50));
  assert(!a.update(30));
  assert(a.update(29));
  assert(a.update(35));
  assert(a.update(40));
  assert(!a.update(41));
  assert(!a.update(35));
}

void testBreathDuty() {
  assert(breathDuty(0, 4000, 1000) == 0);
  assert(breathDuty(2000, 4000, 1000) == 1000);
  assert(breathDuty(4000, 4000, 1000) == 0);
  for (uint32_t t = 0; t < 8000; t += 37) {
    int d = breathDuty(t, 4000, 1000);
    assert(d >= 0 && d <= 1000);
  }
  for (uint32_t t = 0; t < 2000; t += 50) {
    assert(breathDuty(t, 4000, 1000) <= breathDuty(t + 50, 4000, 1000));
  }
}

int main() {
  testMoisturePercent();
  testSmoother();
  testWaterAlertHysteresis();
  testBreathDuty();
  std::puts("All tests passed");
  return 0;
}
