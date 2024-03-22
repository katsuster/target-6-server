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

void cnodeInit(void) {
  int gpios[] = {
    0, 1, 4, 5, 6, 7, 8, 9, 10, 18, 19, /*20, */21,
  };
  for (int i = 0; i < ARRAY_SIZE(gpios); i++) {
    pinMode(gpios[i], INPUT);
  }

  pinMode(GPIO_BUTTON, INPUT_PULLUP);
  pinMode(GPIO_BUZZER, OUTPUT);
  digitalWrite(GPIO_BUZZER, LOW);
  pinMode(GPIO_START, INPUT);

  setRunMode(MODE_READY);
}

void cnodeReady(void) {
  static int before = HIGH;
  static unsigned long ignoreEnd = 0;
  int b = digitalRead(GPIO_BUTTON);
  char buf[64];

  if (millis() < ignoreEnd) {
    //ignore
    return;
  }

  if (b == LOW && before == HIGH) {
    sprintf(buf, "d:%d button press\n", getDeviceID());
    txBLE(buf);
    before = b;
    ignoreEnd = millis();
  } else if (b == HIGH && before == LOW) {
    sprintf(buf, "d:%d button release\n", getDeviceID());
    txBLE(buf);
    before = b;
    ignoreEnd = millis();
  }
}

void cnodeSingleWait(void) {
  pinMode(GPIO_START, OUTPUT);
  digitalWrite(GPIO_START, HIGH);

  setInitTime(millis());
  setRunMode(MODE_SINGLE_WAIT2);
}

void cnodeSingleWait2(void) {
  if (getPastTime() > LED_READY_LEN_MS) {
    digitalWrite(GPIO_START, LOW);

    //Wait enough time to trigger sensor nodes
    delay(10);

    pinMode(GPIO_START, INPUT);
    setRunMode(MODE_BEEP);
  }
}

void cnodeBeep(void) {
  pinMode(GPIO_BUZZER, OUTPUT);
  digitalWrite(GPIO_BUZZER, HIGH);
  setInitTime(millis());

  setRunMode(MODE_BEEP_WAIT);
}

void cnodeBeepWait(void) {
  static int beepVal = LOW;
  static unsigned long before = 0;

  if (getPastTime() > BEEP_LEN_MS) {
    digitalWrite(GPIO_BUZZER, LOW);
    setRunMode(MODE_INIT);
  }

  if (getBeepType() == BEEP_TYPE_SQUARE) {
    if (micros() - before > (1000000 / getBeepHz() / 2)) {
      digitalWrite(GPIO_BUZZER, beepVal);
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
    cnodeInit();
    break;
  case MODE_READY:
    cnodeReady();
    break;
  case MODE_SINGLE_WAIT:
    cnodeSingleWait();
    break;
  case MODE_SINGLE_WAIT2:
    cnodeSingleWait2();
    break;
  case MODE_BEEP:
    cnodeBeep();
    break;
  case MODE_BEEP_WAIT:
    cnodeBeepWait();
    break;
  }
}
