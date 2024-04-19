#include <Arduino.h>

#include "ble.h"
#include "main.h"

struct cntup_game_stat {
  int gpio_last_val;

  unsigned long mil_last_hit;
  int ind_cur_target;
  int targets[N_SENSORS + 1];
};

static struct cntup_game_stat cntup_game;

static void cntupInit(struct cntup_game_stat *game)
{
  game->gpio_last_val = LOW;

  game->mil_last_hit = 0;
  game->ind_cur_target = 0;
  for (int i = 0; i < ARRAY_SIZE(game->targets); i++) {
    game->targets[i] = 0;
  }
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

    setRunMode(MODE_CNTUP_RUN);
  }
}

static int cntupIsFinished(struct cntup_game_stat *game) {
  //tentative
  return 0;
}

static void cntupRun(struct cntup_game_stat *game) {
  if (cntupIsFinished(game)) {
    txAllTargets();
    setRunMode(MODE_INIT);
    return;
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
