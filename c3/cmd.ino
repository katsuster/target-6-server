#include <Arduino.h>

#include "ble.h"
#include "cmd.h"
#include "main.h"

static void sendResult(const char *cmd, bool ok) {
  char buf[128];

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf) - 1, "d:%d %s %s\n", getDeviceID(), cmd, ok ? "OK" : "NG");
  txBLE(buf);
}

static int cmdCommon(String &cmd) {
  int id, r;

  if (cmd.startsWith(CMD_HELP, 0)) {
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
    txBLE(CMD_SSHOT       "            : Wait for start sensor node (speed shoot game mode).\n");
    txBLE(CMD_TATK        "            : Wait for start sensor node (time attack game mode).\n");
    txBLE("\n");
    txBLE("  ver." VERSION "\n");
  } else if (cmd.startsWith(CMD_INIT, 0)) {
    id = -1;
    r = sscanf(cmd.c_str(), CMD_INIT " %d", &id);
    if (r <= 1 && id >= 0) {
      offLED();
      setDeviceID(id);
      setRunMode(MODE_INIT);

      sendResult(CMD_INIT, true);
    } else {
      sendResult(CMD_INIT, false);
    }
  } else if (cmd.startsWith(CMD_BLINK, 0)) {
    blinkLED();
    sendResult(CMD_BLINK, true);
  } else if (cmd.startsWith(CMD_LED_ON, 0)) {
    onLED();
    sendResult(CMD_LED_ON, true);
  } else if (cmd.startsWith(CMD_LED_OFF, 0)) {
    offLED();
    sendResult(CMD_LED_OFF, true);
  } else if (cmd.startsWith(CMD_TYPE_BEEP, 0)) {
    int tb = BEEP_TYPE_HIGH, hz = BEEP_HZ_DEFAULT;

    r = sscanf(cmd.c_str(), CMD_TYPE_BEEP " %d %d", &tb, &hz);
    if (r == 2 && 0 <= tb && tb <= 1 && BEEP_HZ_MIN <= hz && hz <= BEEP_HZ_MAX) {
      setBeepType(tb);
      setBeepHz(hz);

      sendResult(CMD_TYPE_BEEP, true);
    } else {
      sendResult(CMD_TYPE_BEEP, false);
    }
  } else {
    //unknwon command
    return 1;
  }

  return 0;
}

static int cmdControlNode(String &cmd) {
  if (cmd.startsWith(CMD_SINGLE, 0)) {
    if (getRunMode() == MODE_READY || getRunMode() == MODE_SINGLE_RUN) {
      offLED();
      setRunMode(MODE_SINGLE_WAIT);

      sendResult(CMD_SINGLE, true);
    } else {
      sendResult(CMD_SINGLE, false);
    }
  } else if (cmd.startsWith(CMD_BEEP, 0)) {
    if (getDeviceID() >= 0) {
      setRunMode(MODE_BEEP);

      sendResult(CMD_BEEP, true);
    } else {
      sendResult(CMD_BEEP, false);
    }
  } else {
    //unknown command
    return 1;
  }

  return 0;
}

static int cmdSensorNode(String &cmd) {
  int r;

  if (cmd.startsWith(CMD_CNTUP, 0)) {
    int sec = CNTUP_TIMEOUT_DEFAULT_MS;

    r = sscanf(cmd.c_str(), CMD_CNTUP " %d", &sec);
    if (r == 1 && (getRunMode() == MODE_READY || getRunMode() == MODE_CNTUP_RUN)) {
      offLED();
      setNumSensors(N_SENSORS);
      setCntupTimeout(sec);
      setRunMode(MODE_CNTUP_WAIT);

      sendResult(CMD_CNTUP, true);
    } else {
      sendResult(CMD_CNTUP, false);
    }
  } else if (cmd.startsWith(CMD_SSHOT, 0)) {
    int n = N_SENSORS;

    r = sscanf(cmd.c_str(), CMD_SSHOT " %d", &n);
    if (r == 1 && (getRunMode() == MODE_READY || getRunMode() == MODE_SSHOT_RUN)) {
      offLED();
      setNumSensors(n);
      setRunMode(MODE_SSHOT_WAIT);

      sendResult(CMD_SSHOT, true);
    } else {
      sendResult(CMD_SSHOT, false);
    }
  } else if (cmd.startsWith(CMD_TATK, 0)) {
    int n = N_SENSORS;

    r = sscanf(cmd.c_str(), CMD_TATK " %d", &n);
    if (r == 1 && (getRunMode() == MODE_READY || getRunMode() == MODE_TATK_RUN)) {
      offLED();
      setNumSensors(n);
      setRunMode(MODE_TATK_WAIT);

      sendResult(CMD_TATK, true);
    } else {
      sendResult(CMD_TATK, false);
    }
  } else {
    //unknown command
    return 1;
  }

  return 0;
}

void onReceive(String &cmd) {
  int r;

  r = cmdCommon(cmd);
  if (r == 0) {
    return;
  }

  if (getDeviceID() == 0) {
    r = cmdControlNode(cmd);
  } else if (getDeviceID() > 0) {
    r = cmdSensorNode(cmd);
  }
  if (r != 0) {
    sendResult("unknown", false);
  }
}
