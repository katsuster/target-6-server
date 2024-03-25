#include <Arduino.h>

#include "c3_ble.h"
#include "c3_main.h"

static struct sensor sensors[N_SENSORS] = {
  [0] = {
    .id = 0,
    .pin_type_in = PIN_TYPE_DIGITAL,
    .pin_type_out = PIN_TYPE_DIGITAL,
    .pin_in = GPIO_SENSOR0,
    .pin_out = GPIO_TARGET_LED0,
  },

  [1] = {
    .id = 1,
    .pin_type_in = PIN_TYPE_DIGITAL,
    .pin_type_out = PIN_TYPE_DIGITAL,
    .pin_in = GPIO_SENSOR1,
    .pin_out = GPIO_TARGET_LED1,
  },

  [2] = {
    .id = 2,
    .pin_type_in = PIN_TYPE_DIGITAL,
    .pin_type_out = PIN_TYPE_DIGITAL,
    .pin_in = GPIO_SENSOR2,
    .pin_out = GPIO_TARGET_LED2,
  },

  [3] = {
    .id = 3,
    .pin_type_in = PIN_TYPE_DIGITAL,
    .pin_type_out = PIN_TYPE_DIGITAL,
    .pin_in = GPIO_SENSOR3,
    .pin_out = GPIO_TARGET_LED3,
  },

  [4] = {
    .id = 4,
    .pin_type_in = PIN_TYPE_DIGITAL,
    .pin_type_out = PIN_TYPE_DIGITAL,
    .pin_in = GPIO_SENSOR4,
    .pin_out = GPIO_TARGET_LED4,
  },

  [5] = {
    .id = 5,
    .pin_type_in = PIN_TYPE_DIGITAL,
    .pin_type_out = PIN_TYPE_DIGITAL,
    .pin_in = GPIO_SENSOR5,
    .pin_out = GPIO_TARGET_LED5,
  },
};

/* Analog type sample
static struct sensor sensors[N_SENSORS] = {
  [0] = {
    .id = 0,
    .pin_type_in = PIN_TYPE_ANALOG,
    .pin_type_out = PIN_TYPE_DIGITAL,
    .pin_in = ADC_SENSOR0,
    .pin_out = GPIO_TARGET_LED0,
    .col = COL_R,
  },
};
*/

static struct tatk_game_stat tatk_game;

struct sensor *getSensor(int id) {
  return &sensors[id];
}

void initSensors(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);

    s->ave_cnt = 0;
    s->ave_sum = 0;
    s->mil_st = millis();
    s->cnt_hit = 0;
    s->mil_hit = 0;

    s->inter_ave = INTERVAL_AVE_DEFAULT;
    s->inter_hit = INTERVAL_HIT_DEFAULT;
    s->thre_hit = THRESHOLD_DEFAULT;
  }

  setInitTime(millis());
}

int detectHitAnalog(struct sensor *s) {
  uint32_t amv;
  int hit = 0;

  amv = analogRead(s->pin_in);
  s->ave_cnt++;
  s->ave_sum += amv;

  if (millis() - s->mil_st > s->inter_ave) {
    s->ave = s->ave_sum / s->ave_cnt;

    if (s->ave > s->thre_hit) {
      if (millis() - s->mil_last > s->inter_hit) {
        s->cnt_hit++;
        s->mil_last = millis();

        hit = 1;
      }
    }

    s->ave_cnt = 0;
    s->ave_sum = 0;
    s->mil_st = millis();
  }

  return hit;
}

int detectHitDigital(struct sensor *s) {
  int v;

  v = digitalRead(s->pin_in);

  return v == HIGH;
}

void snodeInit(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);
  }

  tatkInit(&tatk_game);
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
