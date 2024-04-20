#include <Arduino.h>

#include "ble.h"
#include "main.h"

struct cntup_game_stat {
  int gpio_last_val;

  unsigned long timeout;
  int last_target;
  unsigned long mil_last_hit;
  int ind_cur_target;
  int targets[N_SENSORS + 1];
};

static struct cntup_game_stat cntup_game;

static void cntupInit(struct cntup_game_stat *game)
{
  game->gpio_last_val = LOW;

  game->timeout = CNTUP_TIMEOUT_MS;
  game->last_target = -1;
  game->mil_last_hit = 0;
  game->ind_cur_target = 0;
  for (int i = 0; i < ARRAY_SIZE(game->targets); i++) {
    game->targets[i] = 0;
  }
}

static void cntupSetupTargets(struct cntup_game_stat *game) {
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

  if (game->targets[0] == game->last_target) {
    int tmp;

    tmp = game->targets[0];
    game->targets[0] = game->targets[1];
    game->targets[1] = tmp;
  }

  //do not touch game->last_target
  game->mil_last_hit = millis();
  game->ind_cur_target = 0;
}

static int cntupGetCurrentTarget(struct cntup_game_stat *game) {
  return game->targets[game->ind_cur_target];
}

static void cntupHighlightCurrentTarget(struct cntup_game_stat *game) {
  for (int i = 0; i < getNumSensors(); i++) {
    struct sensor *s = getSensor(i);

    if (i == cntupGetCurrentTarget(game)) {
      digitalWrite(s->pin_out, HIGH);
    } else {
      digitalWrite(s->pin_out, LOW);
    }
  }
}

static void cntupNextTarget(struct cntup_game_stat *game) {
  if (game->ind_cur_target >= getNumSensors()) {
    Serial1.printf("Cannot go to next target, already finished.");
    return;
  }

  game->last_target = cntupGetCurrentTarget(game);
  game->mil_last_hit = millis();
  game->ind_cur_target++;

  if (game->ind_cur_target >= getNumSensors()) {
    cntupSetupTargets(game);
  }
}

static int cntupIsFinished(struct cntup_game_stat *game) {
  return (getPastTime() >= game->timeout);
}

static void cntupWait(struct cntup_game_stat *game) {
  turnOnAllTargets();

  game->gpio_last_val = LOW;
  setRunMode(MODE_CNTUP_WAIT2);
}

static void cntupWait2(struct cntup_game_stat *game) {
  int falling_edge = detectFallingEdge(getSensor(0), &game->gpio_last_val);

  if (falling_edge) {
    turnOffAllTargets();
    initSensors();

    cntupSetupTargets(game);
    cntupHighlightCurrentTarget(game);
    setRunMode(MODE_CNTUP_RUN);
  }
}

static void cntupRun(struct cntup_game_stat *game) {
  if (cntupIsFinished(game)) {
    txAllTargets();
    setRunMode(MODE_INIT);
    return;
  }

  int index = cntupGetCurrentTarget(game);
  struct sensor *s = getSensor(index);
  int hit = detectHit(s);

  if (hit) {
    s->cnt_hit++;
    s->mil_hit = getPastTime();

    cntupNextTarget(game);
    cntupHighlightCurrentTarget(game);
  }
}

void initCntup(void) {
  cntupInit(&cntup_game);
}

void loopCntup(void) {
  switch (getRunMode()) {
  case MODE_CNTUP_WAIT:
    cntupWait(&cntup_game);
    break;
  case MODE_CNTUP_WAIT2:
    cntupWait2(&cntup_game);
    break;
  case MODE_CNTUP_RUN:
    cntupRun(&cntup_game);
    break;
  }
}
