#ifndef C3_CMD_H__
#define C3_CMD_H__

#define VERSION             "0.1.7"

#define CMD_HELP            "help"
#define CMD_INIT            "init"
#define CMD_BLINK           "blink"
#define CMD_LED_ON          "ledon"
#define CMD_LED_OFF         "ledoff"
#define CMD_TYPE_BEEP       "tbeep"

//Controller
#define CMD_SINGLE          "single"
#define CMD_BEEP            "beep"

//Sensor
#define CMD_CNTUP           "cntup"
#define CMD_SSHOT           "sshot"
#define CMD_TATK            "tatk"

//Result
#define CMD_BUTTON          "button"
#define CMD_HIT             "hit"

void onReceive(String &cmd);

#endif /* C3_CMD_H__ */
