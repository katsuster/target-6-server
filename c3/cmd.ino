#include <Arduino.h>

#include "ble.h"
#include "cmd.h"
#include "main.h"

static void sendMessage(const char *cmd, const char *result) {
  char buf[128];

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf) - 1, "d:%d %s %s\n", getDeviceID(), cmd, result);
  txBLE(buf);
}

static void sendOK(const char *cmd) {
  sendMessage(cmd, "OK");
}

static void sendNG(const char *cmd) {
  sendMessage(cmd, "NG");
}

static int cmdCommon(String &cmd) {
  int id, r;

  if (cmd.startsWith(CMD_HELP, 0)) {
    Serial1.printf(CMD_HELP "\n");

    txBLE(CMD_HELP        "            : Show this message.\n");
    txBLE(CMD_INIT        " devid      : Set ID and clear timer and counts.\n");
    txBLE(CMD_BLINK       "            : Blink LED.\n");
    txBLE(CMD_TYPE_BEEP   " type hz    : Change type and Hz of beep (type:0-1, hz: 100-4000).\n");
    txBLE("\n");
    txBLE("For control node.\n");
    txBLE(CMD_SINGLE      "            : Wait for start control node (single player mode).\n");
    txBLE(CMD_BEEP        "            : Beep for control node.\n");
    txBLE("\n");
    txBLE("For sensor node.\n");
    txBLE(CMD_CNTUP       "            : Wait for start sensor node (count up game mode).\n");
    txBLE(CMD_TATK        "            : Wait for start sensor node (time attack game mode).\n");
    txBLE("\n");
    txBLE("  ver." VERSION "\n");

    return 0;
  }

  if (cmd.startsWith(CMD_INIT, 0)) {
    Serial1.printf(CMD_INIT "\n");

    id = -1;
    r = sscanf(cmd.c_str(), CMD_INIT " %d", &id);
    if (r <= 1 && id >= 0) {
      setDeviceID(id);
      setRunMode(MODE_INIT);

      sendOK(CMD_INIT);
    } else {
      sendNG(CMD_INIT);
    }

    return 0;
  }

  if (cmd.startsWith(CMD_BLINK, 0)) {
    Serial1.printf(CMD_BLINK "\n");

    blinkLED();
    sendOK(CMD_BLINK);

    return 0;
  }

  if (cmd.startsWith(CMD_TYPE_BEEP, 0)) {
    int tb = BEEP_TYPE_HIGH, hz = BEEP_HZ_DEFAULT;

    Serial1.printf(CMD_TYPE_BEEP "\n");

    r = sscanf(cmd.c_str(), CMD_TYPE_BEEP " %d %d", &tb, &hz);
    if (r == 2 && 0 <= tb && tb <= 1 && BEEP_HZ_MIN <= hz && hz <= BEEP_HZ_MAX) {
      setBeepType(tb);
      setBeepHz(hz);

      sendOK(CMD_TYPE_BEEP);
    } else {
      sendNG(CMD_TYPE_BEEP);
    }

    return 0;
  }

  //unknown command
  return 1;
}

static int cmdControlNode(String &cmd) {
  if (cmd.startsWith(CMD_SINGLE, 0)) {
    Serial1.printf(CMD_SINGLE "\n");

    if (getRunMode() == MODE_READY || getRunMode() == MODE_SINGLE_RUN) {
      setRunMode(MODE_SINGLE_WAIT);

      sendOK(CMD_SINGLE);
    } else {
      sendNG(CMD_SINGLE);
    }

    return 0;
  }

  if (cmd.startsWith(CMD_BEEP, 0)) {
    Serial1.printf(CMD_BEEP "\n");

    if (getDeviceID() >= 0) {
      setRunMode(MODE_BEEP);

      sendOK(CMD_BEEP);
    } else {
      sendNG(CMD_BEEP);
    }

    return 0;
  }

  //unknown command
  return 1;
}

static int cmdSensorNode(String &cmd) {
  int r;

  if (cmd.startsWith(CMD_CNTUP, 0)) {
    int sec = CNTUP_TIMEOUT_DEFAULT_MS;

    Serial1.printf(CMD_CNTUP "\n");

    r = sscanf(cmd.c_str(), CMD_CNTUP " %d", &sec);
    if (getRunMode() == MODE_READY || getRunMode() == MODE_CNTUP_RUN) {
      setNumSensors(N_SENSORS);
      setCntupTimeout(sec);
      setRunMode(MODE_CNTUP_WAIT);

      sendOK(CMD_CNTUP);
    } else {
      sendNG(CMD_CNTUP);
    }

    return 0;
  }

  if (cmd.startsWith(CMD_TATK, 0)) {
    int n = N_SENSORS;

    Serial1.printf(CMD_TATK "\n");

    r = sscanf(cmd.c_str(), CMD_TATK " %d", &n);
    if (getRunMode() == MODE_READY || getRunMode() == MODE_TATK_RUN) {
      setNumSensors(n);
      setRunMode(MODE_TATK_WAIT);

      sendOK(CMD_TATK);
    } else {
      sendNG(CMD_TATK);
    }

    return 0;
  }

  //unknown command
  return 1;
}

void onReceive(String &cmd) {
  int r;

  Serial1.printf("onReceive '%s'\n", cmd.c_str());

  r = cmdCommon(cmd);
  if (r == 0) {
    return;
  }

  if (getDeviceID() == 0) {
    r = cmdControlNode(cmd);
  } else if (getDeviceID() > 0) {
    r = cmdSensorNode(cmd);
  } else {
    //not set
    r = 1;
  }
  if (r == 0) {
    return;
  }

  Serial1.printf("unknown\n");
  sendNG("unknown");
}
