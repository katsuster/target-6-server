#ifndef C3_CMD_H__
#define C3_CMD_H__

#define VERSION             "0.1.4"

#define CMD_HELP            "help"
#define CMD_SHOW            "show"
#define CMD_BLINK           "blink"
#define CMD_INIT            "init"
#define CMD_SIX             "six"
#define CMD_SINGLE          "single"
#define CMD_BEEP            "beep"
#define CMD_INTERVAL_AVE    "iave"
#define CMD_INTERVAL_HIT    "ihit"
#define CMD_THRESHOLD       "thre"
#define CMD_TYPE_BEEP       "tbeep"

void onReceive(String &cmd);

#endif /* C3_CMD_H__ */
