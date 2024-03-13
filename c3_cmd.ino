#include <Arduino.h>

#include "c3_ble.h"
#include "c3_cmd.h"
#include "c3_main.h"

void sendMessage(const char *cmd, const char *result) {
  char buf[128];

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf) - 1, "d:%d %s %s\n", getDeviceID(), cmd, result);
  txBLE(buf);
}

void sendOK(const char *cmd) {
  sendMessage(cmd, "OK");
}

void sendNG(const char *cmd) {
  sendMessage(cmd, "NG");
}

int cmdCommon(String &cmd) {
  int id, n, r;

  if (cmd.startsWith(CMD_HELP, 0)) {
    Serial1.printf(CMD_HELP "\n");

    txBLE(CMD_SHOW        "            : Show all parameters.\n");
    txBLE(CMD_BLINK       "            : Blink LED.\n");
    txBLE(CMD_INIT        " devid      : Set ID and clear timer and counts.\n");
    txBLE(CMD_SINGLE      "            : Wait for start (single mode).\n");
    txBLE(CMD_MULTI       "            : Wait for start (multi mode).\n");
    txBLE(CMD_INTERVAL_AVE" sensor num : Change interval of hit detection [ms] (1-100).\n");
    txBLE(CMD_INTERVAL_HIT" sensor num : Change interval of hit to hit [ms] (10-10000).\n");
    txBLE(CMD_THRESHOLD   " sensor num : Change threshold of sound sensor (100-4095).\n");
    txBLE(CMD_HELP        "            : Show this message.\n");

    return 0;
  }

  if (cmd.startsWith(CMD_SHOW, 0)) {
    Serial1.printf(CMD_SHOW "\n");

    for (int i = 0; i < N_SENSORS; i++) {
      struct sensor *s = getSensor(i);
      char buf[128];

      sprintf(buf, "d:%d s:%d iave:%d, ihit:%d, thre:%d\n",
          getDeviceID(), i,
          s->inter_ave, s->inter_hit, s->thre_hit);
      txBLE(buf);
    }

    return 0;
  }

  if (cmd.startsWith(CMD_INIT, 0)) {
    Serial1.printf(CMD_INIT "\n");

    id = -1;
    r = sscanf(cmd.c_str(), CMD_INIT " %d", &id);
    if (r == 1 && id >= 0) {
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

  if (cmd.startsWith(CMD_INTERVAL_AVE, 0)) {
    Serial1.printf(CMD_INTERVAL_AVE "\n");

    r = sscanf(cmd.c_str(), CMD_INTERVAL_AVE " %d %d", &id, &n);
    if (r == 2 && 0 <= id && id <= 2 && 1 <= n && n <= 100) {
      struct sensor *s = getSensor(id);

      s->inter_ave = n;

      sendOK(CMD_INTERVAL_AVE);
    } else {
      sendNG(CMD_INTERVAL_AVE);
    }

    return 0;
  }

  if (cmd.startsWith(CMD_INTERVAL_HIT, 0)) {
    Serial1.printf(CMD_INTERVAL_HIT "\n");

    r = sscanf(cmd.c_str(), CMD_INTERVAL_HIT " %d %d", &id, &n);
    if (r == 2 && 0 <= id && id <= 2 && 10 <= n && n <= 10000) {
      struct sensor *s = getSensor(id);

      s->inter_hit = n;

      sendOK(CMD_INTERVAL_HIT);
    } else {
      sendNG(CMD_INTERVAL_HIT);
    }

    return 0;
  }

  if (cmd.startsWith(CMD_THRESHOLD, 0)) {
    Serial1.printf(CMD_THRESHOLD "\n");

    r = sscanf(cmd.c_str(), CMD_THRESHOLD " %d %d", &id, &n);
    if (r == 2 && 0 <= id && id <= 2 && 100 <= n && n <= 0xfff) {
      struct sensor *s = getSensor(id);

      s->thre_hit = n;

      sendOK(CMD_THRESHOLD);
    } else {
      sendNG(CMD_THRESHOLD);
    }

    return 0;
  }

  //unknown command
  return 1;
}

int cmdControlNode(String &cmd) {
  if (cmd.startsWith(CMD_MULTI, 0)) {
    Serial1.printf(CMD_MULTI "\n");

    if (getDeviceID() >= 0 && (getRunMode() == MODE_INIT || getRunMode() == MODE_MULTI_RUN)) {
      setRunMode(MODE_MULTI_WAIT);

      sendOK(CMD_MULTI);
    } else {
      sendNG(CMD_MULTI);
    }

    return 0;
  }

  //unknown command
  return 1;
}

int cmdSensorNode(String &cmd) {
  if (cmd.startsWith(CMD_SINGLE, 0)) {
    Serial1.printf(CMD_SINGLE "\n");

    if (getDeviceID() >= 0 && (getRunMode() == MODE_INIT || getRunMode() == MODE_SINGLE_RUN)) {
      setRunMode(MODE_SINGLE_WAIT);

      sendOK(CMD_SINGLE);
    } else {
      sendNG(CMD_SINGLE);
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
