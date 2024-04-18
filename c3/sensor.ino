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

static struct tatk_game_stat tatk_game;

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

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);

    s->mil_st = millis();
    s->cnt_hit = 0;
    s->mil_hit = 0;
  }

  setInitTime(millis());
}

int detectHit(struct sensor *s) {
  int v = digitalRead(s->pin_in);

  return v == HIGH;
}

static void snodeInit(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);
  }

  tatkInit(&tatk_game);
  setRunMode(MODE_READY);
}

static void snodeReady(void) {
}

static void snodeWait(void) {
  for (int i = 0; i < getNumSensors(); i++) {
    struct sensor *s = getSensor(i);

    digitalWrite(s->pin_out, HIGH);
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
  case MODE_TATK_WAIT:
    tatkWait(&tatk_game);
    break;
  case MODE_TATK_WAIT2:
    tatkWait2(&tatk_game);
    break;
  case MODE_TATK_RUN:
    tatkRun(&tatk_game);
    break;
  }
}
