#include <Arduino.h>

#include "c3_ble.h"
#include "c3_main.h"

void tatkInit(struct tatk_game_stat *game)
{
  game->gpio_last_val = LOW;
  game->gpio_falling_edge = 0;

  game->mil_last_hit = 0;
  game->ind_cur_target = 0;
  for (int i = 0; i < ARRAY_SIZE(game->targets); i++) {
    game->targets[i] = 0;
  }
}

void tatkSetupTargets(struct tatk_game_stat *game) {
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

int tatkGetCurrentTarget(struct tatk_game_stat *game) {
  return game->targets[game->ind_cur_target];
}

void tatkHighlightCurrentTarget(struct tatk_game_stat *game) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    if (i == tatkGetCurrentTarget(game)) {
      digitalWrite(s->pin_out, HIGH);
    } else {
      digitalWrite(s->pin_out, LOW);
    }
  }
}

void tatkNextTarget(struct tatk_game_stat *game) {
  if (game->ind_cur_target >= N_SENSORS) {
    Serial1.printf("Cannot go to next target, already finished.");
    return;
  }

  game->mil_last_hit = millis();
  game->ind_cur_target++;
}

int tatkIsFinishedTarget(struct tatk_game_stat *game) {
  return tatkGetCurrentTarget(game) == -1;
}

void tatkWait(struct tatk_game_stat *game) {
  snodeWait();

  game->gpio_last_val = LOW;
  game->gpio_falling_edge = 0;
  setInitTime(millis());
  setRunMode(MODE_TATK_WAIT2);
}

void tatkWait2(struct tatk_game_stat *game) {
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

    tatkSetupTargets(game);
    initSensors();
    tatkHighlightCurrentTarget(game);
    setRunMode(MODE_TATK_RUN);
  }
}

void tatkRun(struct tatk_game_stat *game) {
  if (tatkIsFinishedTarget(game)) {
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

    if (i == tatkGetCurrentTarget(game)) {
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
    if (!hit && millis() - game->mil_last_hit < TATK_TIMEOUT_MS) {
      continue;
    }

    if (i == tatkGetCurrentTarget(game)) {
      s->mil_hit = getPastTime();

      tatkNextTarget(game);
      tatkHighlightCurrentTarget(game);
      break;
    }
  }
}
