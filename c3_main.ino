//#include <M5Stack.h>
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

static int device_id = -1;
static int run_mode = MODE_INIT;
static int cur_pos_target;
static int targets[N_SENSORS + 1];
static unsigned long mil_init;
static uint8_t col_r, col_g, col_b;
static int blink = 0;

void initGPIOs(void) {
  int gpios[] = {
    0, 1, 4, 5, 6, 7, 8, 9, 10, 18, 19, /*20, */21,
  };
  for (int i = 0; i < ARRAY_SIZE(gpios); i++) {
    pinMode(gpios[i], INPUT);
  }
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

    pinMode(s->pin_in, INPUT);
    pinMode(s->pin_out, OUTPUT);
    digitalWrite(s->pin_out, LOW);
  }
}

void initSensors(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    struct sensor *s = getSensor(i);

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

struct sensor *getSensor(int id) {
  return &sensors[id];
}

int getDeviceID(void) {
  return device_id;
}

void setDeviceID(int id) {
  device_id = id;
}

int getRunMode(void) {
  return run_mode;
}

void setRunMode(int m) {
  switch (m) {
  case MODE_INIT:
  case MODE_READY:
  case MODE_SINGLE_WAIT:
  case MODE_SINGLE_WAIT2:
  case MODE_SINGLE_RUN:
  case MODE_MULTI_WAIT:
  case MODE_MULTI_WAIT2:
  case MODE_MULTI_RUN:
  case MODE_BEEP:
  case MODE_BEEP_WAIT:
    break;
  default:
    Serial1.printf("%d is unknown mode.\n", m);
    break;
  }

  run_mode = m;
}

void setupTargets(void) {
  for (int i = 0; i < N_SENSORS; i++) {
    targets[i] = i;
  }
  targets[N_SENSORS] = -1;

  for (int i = 0; i < 100; i++) {
    int pos = random(N_SENSORS);
    int tmp;

    tmp = targets[0];
    targets[0] = targets[pos];
    targets[pos] = tmp;
  }

  cur_pos_target = 0;
}

int getCurrentTarget(void) {
  return targets[cur_pos_target];
}

void nextTarget(void) {
  if (cur_pos_target >= N_SENSORS) {
    Serial1.printf("Cannot go to next target, already finished.");
    return;
  }

  cur_pos_target++;
}

int isFinishedTarget(void) {
  return getCurrentTarget() == -1;
}

unsigned long getInitTime(void) {
  return mil_init;
}

void setInitTime(unsigned long m) {
  mil_init = m;
}

unsigned long getPastTime(void) {
  return millis() - mil_init;
}

int setColLED(int col, uint8_t val) {
  switch (col) {
  case COL_R:
    col_r = val;
    break;
  case COL_G:
    col_g = val;
    break;
  case COL_B:
    col_b = val;
    break;
  default:
    return -1;
  }

  neopixelWrite(LED_PIN, col_r, col_g, col_b);

  return 0;
}

int turnonColLED(int col) {
  return setColLED(col, 250);
}

int turnoffColLED(int col) {
  return setColLED(col, 0);
}

int blinkLED(void) {
  if (blink) {
    neopixelWrite(LED_PIN, 250, 250, 250);
  } else {
    neopixelWrite(LED_PIN, 0, 0, 0);
  }

  blink = !blink;
  delay(10);

  return 0;
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

void loop() {
  if (getDeviceID() == 0) {
    loopController();
  } else if (getDeviceID() > 0) {
    loopSensor();
  }

  loopBLE();
}

void setup() {
  //Serial.begin(115200);

  int RX_PIN = 20;
  int TX_PIN = 21;
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  //set the resolution to 12 bits (0-4096)
  analogReadResolution(12);
  //0 mV ... 2500 mV
  analogSetAttenuation(ADC_11db);

  initGPIOs();

  uint8_t macBT[6];
  esp_read_mac(macBT, ESP_MAC_BT);
  Serial1.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);

  initBLE();
}
