#ifndef C3_CMD_H__
#define C3_CMD_H__

#define CMD_HELP            "help"
#define CMD_SHOW            "show"
#define CMD_BLINK           "blink"
#define CMD_INIT            "init"
#define CMD_SINGLE          "single"
#define CMD_MULTI           "multi"
#define CMD_CANCEL          "cancel"
#define CMD_INTERVAL_AVE    "iave"
#define CMD_INTERVAL_HIT    "ihit"
#define CMD_THRESHOLD       "thre"

void onReceive(String &cmd);

#endif /* C3_CMD_H__ */
