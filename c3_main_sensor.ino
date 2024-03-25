#include <Arduino.h>

#include "c3_ble.h"
#include "c3_main.h"

static struct six_game_stat six_game;

void sixInit(struct six_game_stat *game)
{
  game->gpio_last_val = LOW;
  game->gpio_falling_edge = 0;

  game->mil_last_hit = 0;
  game->ind_cur_target = 0;
  for (int i = 0; i < ARRAY_SIZE(game->targets); i++) {
    game->targets[i] = 0;
  }
}

void sixSetupTargets(struct six_game_stat *game) {
  for (int i = 0; i < N_SENSORS; i++) {
    game->targets[i] = i;
  }
  game->targets[N_SENSORS] = -1;

  randomSeed(millis());
  for (int i = 0; i < 100; i++) {
    int pos = random(N_SENSORS);
    int tmp;

    tmp = game->targets[0];
    game->targets[0] = game->targets[pos];
    game->targets[pos] = tmp;
  }

  game->mil_last_hit = millis();
  game->ind_cur_target = 0;
}

int sixGetCurrentTarget(struct six_game_stat *game) {
  return game->targets[game->ind_cur_target];
}

void sixHighlightCurrentTarget(struct six_game_stat *game) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    if (i == sixGetCurrentTarget(game)) {
      digitalWrite(s->pin_out, HIGH);
    } else {
      digitalWrite(s->pin_out, LOW);
    }
  }
}

void sixNextTarget(struct six_game_stat *game) {
  if (game->ind_cur_target >= N_SENSORS) {
    Serial1.printf("Cannot go to next target, already finished.");
    return;
  }

  game->mil_last_hit = millis();
  game->ind_cur_target++;
}

int sixIsFinishedTarget(struct six_game_stat *game) {
  return sixGetCurrentTarget(game) == -1;
}

void sixWait(struct six_game_stat *game) {
  snodeWait();

  game->gpio_last_val = LOW;
  game->gpio_falling_edge = 0;
  setInitTime(millis());
  setRunMode(MODE_SIX_WAIT2);
}

void sixWait2(struct six_game_stat *game) {
  int cur_val = digitalRead(getSensor(0)->pin_in);
  if (game->gpio_last_val == HIGH && cur_val == LOW) {
    game->gpio_falling_edge = 1;
  }
  game->gpio_last_val = cur_val;

  if (game->gpio_falling_edge) {
    for (int i = 0; i < N_SENSORS; i++) {
      struct sensor *s = getSensor(i);

      digitalWrite(s->pin_out, LOW);
    }

    sixSetupTargets(game);
    initSensors();
    sixHighlightCurrentTarget(game);
    setRunMode(MODE_SIX_RUN);
  }
}

void sixRun(struct six_game_stat *game) {
  if (sixIsFinishedTarget(game)) {
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

    if (i == sixGetCurrentTarget(game)) {
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
    //Go to next target if detect hit or 30 secs past
    if (!hit && millis() - game->mil_last_hit < 30 * 1000) {
      continue;
    }

    if (i == sixGetCurrentTarget(game)) {
      s->mil_hit = getPastTime();

      sixNextTarget(game);
      sixHighlightCurrentTarget(game);
      break;
    }
  }
}

void snodeInit(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);
  }

  sixInit(&six_game);
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
  case MODE_SIX_WAIT:
    sixWait(&six_game);
    break;
  case MODE_SIX_WAIT2:
    sixWait2(&six_game);
    break;
  case MODE_SIX_RUN:
    sixRun(&six_game);
    break;
  }
}
