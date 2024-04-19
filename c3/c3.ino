//#include <M5Stack.h>
#include <Arduino.h>

#include "ble.h"
#include "main.h"

static int device_id = -1;
static int run_mode = MODE_INIT;
static unsigned long mil_init;
static int blink = 0;

void initGPIOs(void) {
  int gpios[] = {
    0, 1, 4, 5, 6, 7, 8, 9, 10, 18, 19, /*20, */21,
  };

  //set the resolution to 12 bits (0-4096)
  analogReadResolution(12);
  //0 mV ... 2500 mV
  analogSetAttenuation(ADC_11db);

  for (int i = 0; i < ARRAY_SIZE(gpios); i++) {
    pinMode(gpios[i], OUTPUT);
    digitalWrite(gpios[i], LOW);
  }
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
  run_mode = m;
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

void loop(void) {
  if (getDeviceID() == 0) {
    loopController();
  } else if (getDeviceID() > 0) {
    loopSensor();
  }

  loopBLE();
}

void setup(void) {
  Serial1.begin(115200, SERIAL_8N1, GPIO_UART1_RX, GPIO_UART1_TX);

  uint8_t macBT[6];
  esp_read_mac(macBT, ESP_MAC_BT);
  Serial1.printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);

  delay(10);

  initGPIOs();
  initBLE();
}
