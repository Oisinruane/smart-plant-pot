#pragma once

#include <math.h>
#include <stdint.h>

// Hardware-free logic so it can be unit tested on the host.

// Capacitive sensors read higher when dry, so the mapping is inverted.
inline int moisturePercent(int raw, int dryRaw, int wetRaw) {
  if (dryRaw == wetRaw) return 0;
  long pct = (long)(dryRaw - raw) * 100 / (dryRaw - wetRaw);
  if (pct < 0) return 0;
  if (pct > 100) return 100;
  return (int)pct;
}

class Smoother {
 public:
  explicit Smoother(float alpha) : alpha_(alpha) {}

  float update(float sample) {
    value_ = primed_ ? value_ + alpha_ * (sample - value_) : sample;
    primed_ = true;
    return value_;
  }

  float value() const { return value_; }

 private:
  float alpha_;
  float value_ = 0;
  bool primed_ = false;
};

// Separate on/off thresholds stop the LED flickering when the reading hovers at the limit.
class WaterAlert {
 public:
  WaterAlert(int onBelowPct, int offAbovePct) : on_(onBelowPct), off_(offAbovePct) {}

  bool update(int pct) {
    if (!active_ && pct < on_) active_ = true;
    else if (active_ && pct > off_) active_ = false;
    return active_;
  }

  bool active() const { return active_; }

 private:
  int on_;
  int off_;
  bool active_ = false;
};

// exp(sin) gives a natural-looking breath: slow at the dim end, quicker through the bright peak.
inline int breathDuty(uint32_t ms, uint32_t periodMs, int maxDuty) {
  const float e = 2.7182818f;
  float phase = (float)(ms % periodMs) / periodMs * 2.0f * (float)M_PI - (float)M_PI / 2.0f;
  float level = (expf(sinf(phase)) - 1.0f / e) / (e - 1.0f / e);
  return (int)lroundf(level * maxDuty);
}
