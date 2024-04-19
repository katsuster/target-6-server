#ifndef C3_BLE_H__
#define C3_BLE_H__

#define LOCAL_NAME                "Target6"
// Nordic nRF
#define SERVICE_UUID              "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX    "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX    "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

void initBLE(void);
void loopBLE(void);
void txBLE(const char *buf);

#endif /* C3_BLE_H__ */
