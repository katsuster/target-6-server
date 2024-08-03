#include <Arduino.h>

#include "ble.h"
#include "main.h"

static int num_sensors = N_SENSORS;
static struct sensor sensors[N_SENSORS] = {
  [0] = {
    .id = 0,
    .pin_in = GPIO_SENSOR0,
    .pin_out = GPIO_TARGET_LED0,
  },

  [1] = {
    .id = 1,
    .pin_in = GPIO_SENSOR1,
    .pin_out = GPIO_TARGET_LED1,
  },

  [2] = {
    .id = 2,
    .pin_in = GPIO_SENSOR2,
    .pin_out = GPIO_TARGET_LED2,
  },

  [3] = {
    .id = 3,
    .pin_in = GPIO_SENSOR3,
    .pin_out = GPIO_TARGET_LED3,
  },

  [4] = {
    .id = 4,
    .pin_in = GPIO_SENSOR4,
    .pin_out = GPIO_TARGET_LED4,
  },

  [5] = {
    .id = 5,
    .pin_in = GPIO_SENSOR5,
    .pin_out = GPIO_TARGET_LED5,
  },
};

int getNumSensors(void) {
  return num_sensors;
}

void setNumSensors(int n) {
  if (1 <= n && n <= N_SENSORS) {
    num_sensors = n;
  }
}

struct sensor *getSensor(int id) {
  return &sensors[id];
}

void initSensors(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    s->mil_st = millis();
    s->cnt_hit = 0;
    s->mil_hit = 0;
  }

  setInitTime(millis());
}

void turnOnAllTargets(void) {
  for (int i = 0; i < getNumSensors(); i++) {
    struct sensor *s = getSensor(i);

    digitalWrite(s->pin_out, HIGH);
  }
}

void turnOffAllTargets(void) {
  for (int i = 0; i < getNumSensors(); i++) {
    struct sensor *s = getSensor(i);

    digitalWrite(s->pin_out, LOW);
  }
}

void txAllTargets(void) {
  for (int i = 0; i < getNumSensors(); i++) {
    struct sensor *s = getSensor(i);
    char buf[128];

    sprintf(buf, "d:%d hit s:%d %d:%02d.%03d h:%d\n",
        getDeviceID(), s->id,
        s->mil_hit / 1000 / 60, (s->mil_hit / 1000) % 60, s->mil_hit % 1000,
        s->cnt_hit);
    txBLE(buf);
  }
}

int detectHit(struct sensor *s) {
  int v = digitalRead(s->pin_in);

  return v == HIGH;
}

int detectFallingEdge(struct sensor *s, int *last_val) {
  int cur_val = digitalRead(s->pin_in);

  if (*last_val == HIGH && cur_val == LOW) {
    return 1;
  }
  *last_val = cur_val;

  return 0;
}

static void snodeInit(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);
  }

  initCntup();
  initTatk();
  setRunMode(MODE_READY);
}

static void snodeReady(void) {
}

void loopSensor() {
  switch (getRunMode()) {
  case MODE_INIT:
    snodeInit();
    break;
  case MODE_READY:
    snodeReady();
    break;
  default:
    loopCntup();
    loopSshot();
    loopTatk();
    break;
  }
}
