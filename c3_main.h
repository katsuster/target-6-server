#ifndef C3_MAIN_H__
#define C3_MAIN_H__

#define ARRAY_SIZE(a)    (sizeof(a) / sizeof((a)[0]))

#define N_SENSORS    6

#define PIN_TYPE_ANALOG     1
#define PIN_TYPE_DIGITAL    2

#define MODE_INIT            0
#define MODE_READY           10
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
#define ADC_SENSOR0         0
#define ADC_SENSOR1         1
#define ADC_SENSOR2         4

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

#define INTERVAL_AVE_DEFAULT     5
#define INTERVAL_HIT_DEFAULT     100
#define THRESHOLD_DEFAULT        800
#define BEEP_TYPE_HIGH           0
#define BEEP_TYPE_SQUARE         1
#define BEEP_HZ_DEFAULT          440
#define BEEP_HZ_MIN              100
#define BEEP_HZ_MAX              4000

#define LED_READY_LEN_MS         3000
#define BEEP_LEN_MS              600
#define GAME_TATK_TIMEOUT_MS     30000

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

struct tatk_game_stat {
  int gpio_last_val;
  int gpio_falling_edge;

  unsigned long mil_last_hit;
  int ind_cur_target;
  int targets[N_SENSORS + 1];
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

int getBeepType(void);
void setBeepType(int t);
int getBeepHz(void);
void setBeepHz(int hz);

//Time attack for six targets
void tatkInit(struct tatk_game_stat *game);
void tatkSetupTargets(struct tatk_game_stat *game);
int tatkGetCurrentTarget(struct tatk_game_stat *game);
void tatkHighlightCurrentTarget(struct tatk_game_stat *game);
void tatkNextTarget(struct tatk_game_stat *game);
int tatkIsFinishedTarget(struct tatk_game_stat *game);
void tatkWait(struct tatk_game_stat *game);
void tatkWait2(struct tatk_game_stat *game);
void tatkRun(struct tatk_game_stat *game);

#endif /* C3_MAIN_H__ */
