#include <Arduino.h>

#include "c3_ble.h"
#include "c3_main.h"

static int lastVal = LOW;
static int fallingEdge = 0;

void loopSensorInit(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);
  }
}

void loopSingleWait(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    digitalWrite(s->pin_out, HIGH);
  }

  lastVal = LOW;
  fallingEdge = 0;
  setInitTime(millis());
  setRunMode(MODE_SINGLE_WAIT2);
}

void loopSingleWait2(void) {
  int curVal = digitalRead(getSensor(0)->pin_in);
  if (lastVal == HIGH && curVal == LOW) {
    fallingEdge = 1;
  }
  lastVal = curVal;
    digitalWrite(getSensor(1)->pin_out, lastVal);

  if (fallingEdge) {
    for (int i = 0; i < N_SENSORS; i++) {
      struct sensor *s = getSensor(i);

      digitalWrite(s->pin_out, LOW);
    }

    randomSeed(millis());
    setupTargets();
    initSensors();
    setRunMode(MODE_SINGLE_RUN);
  }
}

void loopSingleRun(void) {
  if (isFinishedTarget()) {
    for (int i = 0; i < N_SENSORS; i++) {
      struct sensor *s = getSensor(i);
      char buf[128];

      sprintf(buf, "d:%d s:%d %d:%02d.%03d hit:%d\n",
          getDeviceID(), s->id,
          s->mil_hit / 1000 / 60, (s->mil_hit / 1000) % 60, s->mil_hit % 1000,
          s->cnt_hit);
      txBLE(buf);
    }

    setRunMode(MODE_INIT);
  }

  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    if (i == getCurrentTarget()) {
      digitalWrite(s->pin_out, HIGH);
    } else {
      digitalWrite(s->pin_out, LOW);
    }
  }

  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);
    int hit;

    if (s->pin_type_in == PIN_TYPE_ANALOG) {
      hit = detectHitAnalog(s);
    } else {
      hit = detectHitDigital(s);
    }
    if (!hit) {
      continue;
    }

    if (i == getCurrentTarget()) {
      s->mil_hit = getPastTime();

      nextTarget();
      break;
    }
  }
}

void loopSensor() {
  switch (getRunMode()) {
  case MODE_INIT:
    loopSensorInit();
    break;
  case MODE_SINGLE_WAIT:
    loopSingleWait();
    break;
  case MODE_SINGLE_WAIT2:
    loopSingleWait2();
    break;
  case MODE_SINGLE_RUN:
    loopSingleRun();
    break;
  }
}
