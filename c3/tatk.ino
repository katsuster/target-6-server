#include <Arduino.h>

#include "ble.h"
#include "main.h"

struct tatk_game_stat {
  int gpio_last_val;

  unsigned long timeout;
  unsigned long mil_last_hit;
  int ind_cur_target;
  int targets[N_SENSORS + 1];
};

static struct tatk_game_stat tatk_game;

static void tatkInit(struct tatk_game_stat *game)
{
  game->gpio_last_val = LOW;

  game->timeout = TATK_TIMEOUT_MS;
  game->mil_last_hit = 0;
  game->ind_cur_target = 0;
  for (int i = 0; i < ARRAY_SIZE(game->targets); i++) {
    game->targets[i] = 0;
  }
}

static void tatkSetupTargets(struct tatk_game_stat *game) {
  for (int i = 0; i < getNumSensors(); i++) {
    game->targets[i] = i;
  }
  game->targets[getNumSensors()] = -1;

  randomSeed(millis());
  for (int i = 0; i < 100; i++) {
    int pos = random(getNumSensors());
    int tmp;

    tmp = game->targets[0];
    game->targets[0] = game->targets[pos];
    game->targets[pos] = tmp;
  }

  game->mil_last_hit = millis();
  game->ind_cur_target = 0;
}

static int tatkGetCurrentTarget(struct tatk_game_stat *game) {
  return game->targets[game->ind_cur_target];
}

static void tatkHighlightCurrentTarget(struct tatk_game_stat *game) {
  for (int i = 0; i < getNumSensors(); i++) {
    struct sensor *s = getSensor(i);

    if (i == tatkGetCurrentTarget(game)) {
      digitalWrite(s->pin_out, HIGH);
    } else {
      digitalWrite(s->pin_out, LOW);
    }
  }
}

static void tatkNextTarget(struct tatk_game_stat *game) {
  if (game->ind_cur_target >= getNumSensors()) {
    Serial1.printf("Cannot go to next target, already finished.");
    return;
  }

  game->mil_last_hit = millis();
  game->ind_cur_target++;
}

static int tatkIsFinished(struct tatk_game_stat *game) {
  return tatkGetCurrentTarget(game) == -1;
}

static void tatkWait(struct tatk_game_stat *game) {
  turnOffAllTargets();

  game->gpio_last_val = LOW;
  setRunMode(MODE_TATK_WAIT2);
}

static void tatkWait2(struct tatk_game_stat *game) {
  int falling_edge = detectFallingEdge(getSensor(0), &game->gpio_last_val);

  if (falling_edge) {
    turnOffAllTargets();
    initSensors();

    tatkSetupTargets(game);
    tatkHighlightCurrentTarget(game);
    setRunMode(MODE_TATK_RUN);
  }
}

static void tatkRun(struct tatk_game_stat *game) {
  if (tatkIsFinished(game)) {
    txAllTargets();
    setRunMode(MODE_INIT);
    return;
  }

  int index = tatkGetCurrentTarget(game);
  struct sensor *s = getSensor(index);
  int hit = detectHit(s);

  //Go to next target if detect hit or timeout
  if (hit || millis() - game->mil_last_hit >= game->timeout) {
    s->mil_hit = getPastTime();

    tatkNextTarget(game);
    tatkHighlightCurrentTarget(game);
  }
}

void initTatk(void) {
  tatkInit(&tatk_game);
}

void loopTatk(void) {
  switch (getRunMode()) {
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
