#ifndef C3_MAIN_H__
#define C3_MAIN_H__

#define ARRAY_SIZE(a)    (sizeof(a) / sizeof((a)[0]))

#define N_SENSORS    6

#define PIN_TYPE_ANALOG     1
#define PIN_TYPE_DIGITAL    2

#define MODE_INIT           0
#define MODE_SINGLE_WAIT    1
#define MODE_SINGLE_WAIT2   11
#define MODE_SINGLE_RUN     2
#define MODE_MULTI_WAIT     3
#define MODE_MULTI_WAIT2    31
#define MODE_MULTI_RUN      4

#define LED_PIN          2
#define ADC_PIN0         0
#define ADC_PIN1         1
#define ADC_PIN2         4
#define GPIO_HIT_PIN0    6
#define GPIO_HIT_PIN1    7
#define GPIO_HIT_PIN2    8

#define INTERVAL_AVE_DEFAULT     5
#define INTERVAL_HIT_DEFAULT     100
#define THRESHOLD_DEFAULT        800

#define COL_R    1
#define COL_G    2
#define COL_B    3

struct sensor {
  int id;
  int pin_type_in;
  int pin_type_out;
  uint8_t pin_in;
  uint8_t pin_out;
  int col;

  int ave;
  int ave_cnt;
  int ave_sum;
  unsigned long mil_st;
  unsigned long mil_last;
  int cnt_hit;
  unsigned long mil_hit;

  unsigned long inter_ave;
  unsigned long inter_hit;
  uint32_t thre_hit;
};

void initSensors(void);
struct sensor *getSensor(int id);
int getDeviceID(void);
void setDeviceID(int id);
int getRunMode(void);
void setRunMode(int m);
unsigned long getInitTime(void);
void setInitTime(unsigned long m);
int turnonColLED(int col);
int turnoffColLED(int col);
int blinkLED(void);

void loopController();
void loopSensor();

#endif /* C3_MAIN_H__ */
