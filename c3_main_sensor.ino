#include <Arduino.h>

#include "c3_ble.h"
#include "c3_main.h"

static int six_last_val = LOW;
static int six_falling_edge = 0;
static int six_cur_target;
static int six_targets[N_SENSORS + 1];

void snodeInit(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);
  }

  setRunMode(MODE_READY);
}

void snodeReady(void) {
}

void snodeWait(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    digitalWrite(s->pin_out, HIGH);
  }
}

void sixSetupTargets(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    six_targets[i] = i;
  }
  six_targets[N_SENSORS] = -1;

  for (int i = 0; i < 100; i++) {
    int pos = random(N_SENSORS);
    int tmp;

    tmp = six_targets[0];
    six_targets[0] = six_targets[pos];
    six_targets[pos] = tmp;
  }

  six_cur_target = 0;
}

int sixGetCurrentTarget(void) {
  return six_targets[six_cur_target];
}

void sixNextTarget(void) {
  if (six_cur_target >= N_SENSORS) {
    Serial1.printf("Cannot go to next target, already finished.");
    return;
  }

  six_cur_target++;
}

int sixIsFinishedTarget(void) {
  return sixGetCurrentTarget() == -1;
}

void sixWait(void) {
  snodeWait();

  six_last_val = LOW;
  six_falling_edge = 0;
  setInitTime(millis());
  setRunMode(MODE_SIX_WAIT2);
}

void sixWait2(void) {
  int cur_val = digitalRead(getSensor(0)->pin_in);
  if (six_last_val == HIGH && cur_val == LOW) {
    six_falling_edge = 1;
  }
  six_last_val = cur_val;

  if (six_falling_edge) {
    for (int i = 0; i < N_SENSORS; i++) {
      struct sensor *s = getSensor(i);

      digitalWrite(s->pin_out, LOW);
    }

    randomSeed(millis());
    sixSetupTargets();
    initSensors();
    setRunMode(MODE_SIX_RUN);
  }
}

void sixRun(void) {
  if (sixIsFinishedTarget()) {
    for (int i = 0; i < N_SENSORS; i++) {
      struct sensor *s = getSensor(i);
      char buf[128];

      sprintf(buf, "d:%d hit s:%d %d:%02d.%03d hit:%d\n",
          getDeviceID(), s->id,
          s->mil_hit / 1000 / 60, (s->mil_hit / 1000) % 60, s->mil_hit % 1000,
          s->cnt_hit);
      txBLE(buf);
    }

    setRunMode(MODE_INIT);
  }

  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    if (i == sixGetCurrentTarget()) {
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

    if (i == sixGetCurrentTarget()) {
      s->mil_hit = getPastTime();

      sixNextTarget();
      break;
    }
  }
}

void loopSensor() {
  switch (getRunMode()) {
  case MODE_INIT:
    snodeInit();
    break;
  case MODE_READY:
    snodeReady();
    break;
  case MODE_SIX_WAIT:
    sixWait();
    break;
  case MODE_SIX_WAIT2:
    sixWait2();
    break;
  case MODE_SIX_RUN:
    sixRun();
    break;
  }
}
