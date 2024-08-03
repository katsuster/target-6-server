#ifndef C3_MAIN_H__
#define C3_MAIN_H__

#define ARRAY_SIZE(a)    (sizeof(a) / sizeof((a)[0]))

#define N_SENSORS    6

#define MODE_INIT            0
#define MODE_READY           10
#define MODE_CNTUP_WAIT      100
#define MODE_CNTUP_WAIT2     101
#define MODE_CNTUP_RUN       102
#define MODE_SSHOT_WAIT      200
#define MODE_SSHOT_WAIT2     201
#define MODE_SSHOT_RUN       230
#define MODE_TATK_WAIT       20
#define MODE_TATK_WAIT2      21
#define MODE_TATK_RUN        30
#define MODE_SINGLE_WAIT     40
#define MODE_SINGLE_WAIT2    51
#define MODE_SINGLE_RUN      60
#define MODE_BEEP            70
#define MODE_BEEP_WAIT       71

//GPIO
#define GPIO_UART1_TX        21
#define GPIO_UART1_RX        20

//GPIO: Controller
#define GPIO_BUTTON         0
#define GPIO_BUZZER         1
#define GPIO_START          10

//GPIO: Sensor
#define LED_PIN             2

#define GPIO_SENSOR0        0
#define GPIO_SENSOR1        1
#define GPIO_SENSOR2        4
#define GPIO_SENSOR3        5
#define GPIO_SENSOR4        6
#define GPIO_SENSOR5        7
#define GPIO_TARGET_LED0    8
#define GPIO_TARGET_LED1    9
#define GPIO_TARGET_LED2    10
#define GPIO_TARGET_LED3    18
#define GPIO_TARGET_LED4    19
#define GPIO_TARGET_LED5    21

#define BEEP_TYPE_HIGH           0
#define BEEP_TYPE_SQUARE         1
#define BEEP_HZ_DEFAULT          440
#define BEEP_HZ_MIN              100
#define BEEP_HZ_MAX              4000

#define CNTUP_TIMEOUT_DEFAULT_MS    30000

#define SSHOT_TIMEOUT_MS         30000
#define SSHOT_BLINK_MS           500

#define LED_READY_LEN_MS         3000
#define BEEP_LEN_MS              600
#define TATK_TIMEOUT_MS          30000

struct sensor {
  int id;
  uint8_t pin_in;
  uint8_t pin_out;

  unsigned long mil_st;
  unsigned long mil_last;
  int cnt_hit;
  unsigned long mil_hit;
};

void initGPIOs(void);
int getDeviceID(void);
void setDeviceID(int id);
int getRunMode(void);
void setRunMode(int m);
unsigned long getInitTime(void);
void setInitTime(unsigned long m);
unsigned long getPastTime(void);
int blinkLED(void);

//Controller
int getBeepType(void);
void setBeepType(int t);
int getBeepHz(void);
void setBeepHz(int hz);
void loopController(void);

//Sensor
int getNumSensors(void);
void setNumSensors(int n);
struct sensor *getSensor(int id);
void initSensors(void);
void turnOnAllTargets(void);
void turnOffAllTargets(void);
void txAllTargets(void);
int detectHit(struct sensor *s);
int detectFallingEdge(struct sensor *s, int *last_val);
void loopSensor(void);

//Count up
unsigned long getCntupTimeout(void);
void setCntupTimeout(unsigned long msec);
void initCntup(void);
void loopCntup(void);

//Speed shoot
void initSshot(void);
void loopSshot(void);

//Time attack
void initTatk(void);
void loopTatk(void);

#endif /* C3_MAIN_H__ */
