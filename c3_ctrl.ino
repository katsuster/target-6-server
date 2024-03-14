#include <Arduino.h>

#include "c3_ble.h"
#include "c3_main.h"

static int beep_type = BEEP_TYPE_HIGH;
static int beep_hz = BEEP_HZ_DEFAULT;

int getBeepType(void)
{
  return beep_type;
}

void setBeepType(int t)
{
  beep_type = t;
}

int getBeepHz(void)
{
  return beep_hz;
}

void setBeepHz(int hz)
{
  beep_hz = hz;
}

void loopCtrlInit(void) {
  int gpios[] = {
    0, 1, 4, 5, 6, 7, 8, 9, 10, 18, 19, /*20, */21,
  };
  for (int i = 0; i < ARRAY_SIZE(gpios); i++) {
    pinMode(gpios[i], INPUT);
  }

  pinMode(0, INPUT);
  pinMode(1, INPUT);
}

void loopMultiWait(void) {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(0, HIGH);
  digitalWrite(1, HIGH);

  setInitTime(millis());
  setRunMode(MODE_MULTI_WAIT2);
}

void loopMultiWait2(void) {
  if (getPastTime() > 3000) {
    digitalWrite(0, LOW);
    digitalWrite(1, LOW);
    delay(10);

    setRunMode(MODE_INIT);
  }
}

void loopMultiBeep(void) {
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  setInitTime(millis());

  setRunMode(MODE_BEEP_WAIT);
}

void loopMultiBeepWait(void) {
  static int beepVal = LOW;
  static unsigned long before = 0;

  if (getPastTime() > 1000) {
    digitalWrite(4, LOW);
    pinMode(4, INPUT);
    setRunMode(MODE_INIT);
  }

  if (getBeepType() == BEEP_TYPE_SQUARE) {
    if (micros() - before > (1000000 / getBeepHz() / 2)) {
      digitalWrite(4, beepVal);
      if (beepVal == LOW) {
        beepVal = HIGH;
      } else {
        beepVal = LOW;
      }

      before = micros();
    }
  }
}

void loopController() {
  switch (getRunMode()) {
  case MODE_INIT:
    loopCtrlInit();
    break;
  case MODE_MULTI_WAIT:
    loopMultiWait();
    break;
  case MODE_MULTI_WAIT2:
    loopMultiWait2();
    break;
  case MODE_BEEP:
    loopMultiBeep();
    break;
  case MODE_BEEP_WAIT:
    loopMultiBeepWait();
    break;
  }
}
