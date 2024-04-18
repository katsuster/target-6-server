#ifndef C3_CMD_H__
#define C3_CMD_H__

#define VERSION             "0.1.5"

#define CMD_HELP            "help"
#define CMD_BLINK           "blink"
#define CMD_INIT            "init"
#define CMD_TATK            "tatk"
#define CMD_SINGLE          "single"
#define CMD_BEEP            "beep"
#define CMD_TYPE_BEEP       "tbeep"

void onReceive(String &cmd);

#endif /* C3_CMD_H__ */
