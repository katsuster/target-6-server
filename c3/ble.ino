#include <Arduino.h>
// Bluetooth LE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "ble.h"
#include "cmd.h"

static BLEServer *pServer = NULL;
static BLECharacteristic *pTxCharacteristic;
static bool deviceConnected = false;
static bool oldDeviceConnected = false;

#define RX_QUEUE_LEN    8
#define RX_CMD_MAX      128

struct rx_cmd {
  char buf[RX_CMD_MAX];
};

static QueueHandle_t rxQueue;

// Bluetooth LE Change Connect State
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

// Bluetooth LE Receive
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue().c_str();
    struct rx_cmd c;

    if (rxValue.length() == 0) {
      return;
    }

    // Do not handle commands in BLE task, notify() in callback may be lost
    memset(&c, 0, sizeof(c));
    strncpy(c.buf, rxValue.c_str(), sizeof(c.buf) - 1);
    if (xQueueSend(rxQueue, &c, 0) != pdTRUE) {
      //Serial1.printf("rx queue full\n");
    }
  }
};

// Bluetooth LE initialize
void initBLE(void) {
  rxQueue = xQueueCreate(RX_QUEUE_LEN, sizeof(struct rx_cmd));

  // Create the BLE Device
  BLEDevice::init(LOCAL_NAME);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
}

void loopBLE(void) {
    struct rx_cmd c;

    // receiving
    while (xQueueReceive(rxQueue, &c, 0) == pdTRUE) {
        String cmd = String(c.buf);
        onReceive(cmd);
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(1000);
        pServer->startAdvertising();
        //Serial1.println("startAdvertising");
        oldDeviceConnected = deviceConnected;
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }
}

void txBLE(const char *buf) {
  pTxCharacteristic->setValue((uint8_t *)buf, strlen(buf));
  pTxCharacteristic->notify();
  delay(10);
}
