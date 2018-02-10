# Mini Untztrument Demo, Arduino Style

miniuntz is based on the code at
[Adafruit's Mini Untztrument tutorial](https://learn.adafruit.com/mini-untztrument-3d-printed-midi-controller)
but miniuntz does not depend on Teensy USB code. It uses the Arduino MIDIUSB
library by Gary Grewal which simplifies the software installation.

miniuntz has been tested on the SparkFun Pro Micro 5V which has the same
processor (32u4) as the Leonardo. The Adafruit Itsy Bitsy 5V also has a 32u4
processor so it should also work.

|Trellis  |Pro Micro|
|-------  |---------|
|5V       |RAW|
|GND      |GND|
|SDA      |2|
|SCL      |3|
|INT      |not connected|

miniuntz has been tested on the Adafruit Trinket M0 with bidirectional logic
level changer.  Analog inputs are not tested. But the Trinket M0 should have 3
inputs for analog input, if desired.  This should also work with the Itsy Bitsy
M0, Feather M0, SparkFun SAMD21, and Arduino Zero. All use 3.3V logic levels so
a logic level converter is used.

|Trellis     |LLCONV      |Trinket M0|
|-------     |------      |----------|
|            |     LV     |3.3|
|5V          |HV          |USB|
|GND         |GND-GND     |GND|
|SCL         |HV1-LV1     |SCL|
|SDA         |HV2-LV2     |SDA|
|INT         |HV3-LV3     |not connected|

LLCONV = bidirectional logic level converter such as Adafruit BSS138 board.
 https://www.adafruit.com/product/757

![Photo of Adafruit Trinket M0 and logic level converter](https://github.com/gdsports/miniuntz/blob/master/images/trinket_m0_trellis_a.jpg)
![Photo of back of Adafruit trellis board](https://github.com/gdsports/miniuntz/blob/master/images/trinket_m0_trellis_b.jpg)
![Photo of Trinket M0 and mini Untztrument](https://github.com/gdsports/miniuntz/blob/master/images/trinket_m0_trellis_c.jpg)

