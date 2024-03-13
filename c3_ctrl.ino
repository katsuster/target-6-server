#include <Arduino.h>

#include "c3_ble.h"
#include "c3_main.h"

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
  }
}
