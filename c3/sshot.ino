#include <Arduino.h>

#include "ble.h"
#include "main.h"

struct sshot_game_stat {
  int gpio_last_val;

  unsigned long timeout;
  unsigned long timeout_blink;
  unsigned long mil_start;
  unsigned long mil_blink;
  int ind_stop_target;
  int led_stop_target;
  int targets[N_SENSORS];
};

static struct sshot_game_stat sshot_game;

static void sshotInit(struct sshot_game_stat *game)
{
  game->gpio_last_val = LOW;

  game->timeout = SSHOT_TIMEOUT_MS;
  game->timeout_blink = SSHOT_BLINK_MS;
  game->mil_start = 0;
  game->mil_blink = 0;
  game->ind_stop_target = 0;
  game->led_stop_target = HIGH;
  for (int i = 0; i < ARRAY_SIZE(game->targets); i++) {
    game->targets[i] = 0;
  }
}

static void sshotSetupTargets(struct sshot_game_stat *game) {
  for (int i = 0; i < ARRAY_SIZE(game->targets); i++) {
    game->targets[i] = 0;
  }

  game->timeout = SSHOT_TIMEOUT_MS;
  game->timeout_blink = SSHOT_BLINK_MS;
  game->mil_start = millis();
  game->mil_blink = millis();
  game->led_stop_target = HIGH;
  //TODO: change stop target by setting?
  game->ind_stop_target = 0;
}

static int sshotGetStopTarget(struct sshot_game_stat *game) {
  return game->targets[game->ind_stop_target];
}

static void sshotBlinkStopTarget(struct sshot_game_stat *game) {
  if (millis() - game->mil_blink >= game->timeout_blink) {
    struct sensor *s = getSensor(sshotGetStopTarget(game));

    digitalWrite(s->pin_out, game->led_stop_target);

    game->mil_blink = millis();
    game->led_stop_target = (game->led_stop_target == LOW) ? HIGH : LOW;
  }
}

static void sshotShootDownTarget(struct sshot_game_stat *game, int index) {
  game->targets[index] = 1;
}

static int sshotIsFinished(struct sshot_game_stat *game) {
  return sshotGetStopTarget(game);
}

static void sshotWait(struct sshot_game_stat *game) {
  turnOffAllTargets();

  game->gpio_last_val = LOW;
  setRunMode(MODE_SSHOT_WAIT2);
}

static void sshotWait2(struct sshot_game_stat *game) {
  int falling_edge = detectFallingEdge(getSensor(0), &game->gpio_last_val);

  if (falling_edge) {
    turnOnAllTargets();
    initSensors();

    sshotSetupTargets(game);
    setRunMode(MODE_SSHOT_RUN);
  }
}

static void sshotRun(struct sshot_game_stat *game) {
  if (sshotIsFinished(game)) {
    txAllTargets();
    setRunMode(MODE_INIT);
    return;
  }

  sshotBlinkStopTarget(game);

  for (int i = 0; i < getNumSensors(); i++) {
    if (game->targets[i]) {
      continue;
    }

    struct sensor *s = getSensor(i);
    int hit = detectHit(s);

    if (hit) {
      s->mil_hit = getPastTime();
      digitalWrite(s->pin_out, LOW);

      sshotShootDownTarget(game, i);
      break;
    }
  }

  //End the game if timeout
  if (millis() - game->mil_start >= game->timeout) {
    for (int i = 0; i < getNumSensors(); i++) {
      if (game->targets[i]) {
        continue;
      }

      struct sensor *s = getSensor(i);

      s->mil_hit = game->timeout;

      sshotShootDownTarget(game, i);
    }
  }
}

void initSshot(void) {
  sshotInit(&sshot_game);
}

void loopSshot(void) {
  switch (getRunMode()) {
  case MODE_SSHOT_WAIT:
    sshotWait(&sshot_game);
    break;
  case MODE_SSHOT_WAIT2:
    sshotWait2(&sshot_game);
    break;
  case MODE_SSHOT_RUN:
    sshotRun(&sshot_game);
    break;
  }
}
