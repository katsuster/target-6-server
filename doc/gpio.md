
# GPIO

This document describes GPIO and their purpose.


## M5Stamp C3

![M5Stamp C3 pinmap](m5_stamp_c3_09.png)

Please refer M5STACK official documents to refer the original and newest version of image.

URL: https://docs.m5stack.com/en/core/stamp_c3

GPIO purpose of M5Stamp C3.

| GPIO | Controller node | direction | Sensor node | direction |
|--|--|--|--|--|
| G0  | SWITCH_BUTTON | Input  | SENSOR0     | Input  |
| G1  | SWITCH_BUZZER | Output | SENSOR1     | Input  |
| G4  | CLOW_IN       | Input  | SENSOR2     | Input  |
| G5  | CLOW_OUT      | Output | SENSOR3     | Input  |
| G6  | CHIGH_IN      | Input  | SENSOR4     | Input  |
| G7  | CHIGH_OUT     | Output | SENSOR5     | Input  |
| G8  | ---           | ---    | TARGET_LED0 | Output |
| G9  | ---           | ---    | TARGET_LED1 | Output |
| G10 | SENSOR_NODE   | Output | TARGET_LED2 | Output |
| G18 | ---           | ---    | TARGET_LED3 | Output |
| G19 | ---           | ---    | TARGET_LED4 | Output |
| G20 | ---           | ---    | ---         | ---    |
| G21 | ---           | ---    | TARGET_LED5 | Output |

Note that polarity of SWITCH_BUTTON is low-active, other input ports are high-active.

| Input signal | Low | High | Note |
|--|--|--|--|
| SWITCH_BUTTON | Pressed    | Released | pull up |
| SENSORn       | Not hit    | Hit      | --- |

| Output signal | Low | High | Note |
|--|--|--|--|
| SENSOR_NODEn  | Do nothing | Start    | --- |
| SWITCH_BUZZER | Do nothing | Beep     | --- |
| TARGET_LEDn   | Off        | On       | --- |

GPIO pullup/pulldown voltage of M5Stamp C3.

| GPIO | INPUT PULLDOWN [V] | INPUT PULLUP [V] | INPUT (Hi-Z) [V] | OUTPUT LOW [V] | OUTPUT HIGH [V] |
|--|--|--|--|--|--|
| G0  | 0    | 3.52 |      | 0    | 3.52 |
| G1  | 0    | 3.52 |      | 0    | 3.52 |
| G4  | 0    | 3.52 |      | 0    | 3.52 |
| G5  | 0    | 3.52 |      | 0    | 3.52 |
| G6  | 0    | 3.52 |      | 0    | 3.52 |
| G7  | 0    | 3.52 |      | 0    | 3.52 |
| G8  | 2.72 | 3.52 | 3.6  | 0    | 3.52 |
| G9  | 3.36 | 3.52 | 3.52 | 0    | 3.52 |
| G10 | 0    | 3.52 |      | 0    | 3.52 |
| G18 | 0    | 3.52 |      | 0    | 3.52 |
| G19 | 0    | 3.52 |      | 0    | 3.52 |
| G20 | 3.52 | 3.52 | 3.52 | 2.16 | 3.52 |
| G21 | 0.72 | 3.52 | 3.52 | 0    | 3.44 |

Legends:

* INPUT PULLDOWN: `pinMode(n, INPUT_PULLDOWN);`
* INPUT PULLUP:   `pinMode(n, INPUT_PULLUP);`
* INPUT Hi-Z:     `pinMode(n, INPUT);`
* OUTPUT LOW:     `pinMode(n, OUTPUT); digitalWrite(n, LOW);`
* OUTPUT HIGH:    `pinMode(n, OUTPUT); digitalWrite(n, HIGH);`

