/****************************************************************************
MIT License

Copyright (c) 2018 gdsports625@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/

/*
 * Mini Untztrument Demo, Arduino Style
 *
 * https://learn.adafruit.com/mini-untztrument-3d-printed-midi-controller
 *
 * miniuntz is based on the code at the above link but it does not depend on
 * Teensy USB code. It uses the Arduino MIDIUSB library by Gary Grewal which
 * simplifies the software installation.
 *
 * WARNING: Do not enable ANALOG_INPUT unless pots are hooked up to the analog
 * inputs. If nothing is connected, the analog inputs will pick up random noise
 * which will result in a never ending stream of MIDI CC messages.
 *
 * miniuntz has been tested on the SparkFun Pro Micro 5V which has the same
 * processor (32u4) as the Leonardo. The Adafruit Itsy Bitsy 5V also has a 32u4
 * processor so it should also work.
 *
 * Trellis  Pro Micro
 * =======  =========
 * 5V       RAW
 * GND      GND
 * SDA      2
 * SCL      3
 * INT      not connected
 *
 * miniuntz has been tested on the Adafruit Trinket M0 with bidirectional logic
 * level changer.  Analog inputs not tested. But the Trinket M0 should have 3
 * inputs for analog input, if desired.  This should also work with the Itsy
 * Bitsy M0, Feather M0, SparkFun SAMD21, and Arduino Zero. All use 3.3V logic
 * levels so a logic level converter is required.
 *
 * Trellis     LLCONV      Trinket M0
 * =======     ======      =======
 *                  LV     3.3
 * 5V          HV          USB
 * GND         GND-GND     GND
 * SCL         HV1-LV1     SCL
 * SDA         HV2-LV2     SDA
 * INT         HV3-LV3     not connected
 *
 * LLCONV = bidirectional logic level converter such as Adafruit BSS138 board.
 *  https://www.adafruit.com/product/757
 */

#include <Wire.h>
#include <Adafruit_Trellis.h>
#include <MIDIUSB.h>

#define LED     LED_BUILTIN // Pin for heartbeat LED (shows code is working)
#define CHANNEL 1           // MIDI channel number

Adafruit_Trellis trellis;

/*
 * Do not enable without connecting pots to the inputs.
 */
//#define ANALOG_INPUT

uint8_t       heart        = 0;  // Heartbeat LED counter
unsigned long prevReadTime = 0L; // Keypad polling timer
#ifdef ANALOG_INPUT
uint8_t       mod;
uint8_t       vel;
uint8_t       fxc;
uint8_t       rate;
#endif

uint8_t note[] = {
  60, 61, 62, 63,
  56, 57, 58, 59,
  52, 53, 54, 55,
  48, 49, 50, 51
};

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, (byte)(0x90 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, (byte)(0x80 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, (byte) (0xB0 | channel), control, value};
  MidiUSB.sendMIDI(event);
}

void setup() {
  pinMode(LED, OUTPUT);
  trellis.begin(0x70); // Pass I2C address
#ifdef __AVR__
  // Default Arduino I2C speed is 100 KHz, but the HT16K33 supports
  // 400 KHz.  We can force this for faster read & refresh, but may
  // break compatibility with other I2C devices...so be prepared to
  // comment this out, or save & restore value as needed.
  TWBR = 12;
#endif
  trellis.clear();
  trellis.writeDisplay();
#ifdef ANALOG_INPUT
  mod = map(analogRead(0), 0, 1023, 0, 127);
  vel = map(analogRead(1), 0, 1023, 0, 127);
  fxc = map(analogRead(2), 0, 1023, 0, 127);
  rate = map(analogRead(3),0, 1023, 0, 127);
  controlChange(CHANNEL,  1, mod);
  controlChange(CHANNEL, 11, vel);
  controlChange(CHANNEL, 12, fxc);
  controlChange(CHANNEL, 13, rate);
#endif
}

void loop() {
  unsigned long t = millis();
  if((t - prevReadTime) >= 20L) { // 20ms = min Trellis poll time
    if(trellis.readSwitches()) {  // Button state change?

      for(uint8_t i=0; i<16; i++) { // For each button...
        if(trellis.justPressed(i)) {
          noteOn(CHANNEL, note[i], 127);

          trellis.setLED(i);
        } else if(trellis.justReleased(i)) {
          noteOn(CHANNEL, note[i], 0);
          trellis.clrLED(i);
        }
      }
      trellis.writeDisplay();
    }
#ifdef ANALOG_INPUT
    uint8_t newModulation = map(analogRead(0), 0, 1023, 0, 127);
    if(mod != newModulation) {
      mod = newModulation;
      controlChange(CHANNEL, 1, mod);
    }
    uint8_t newVelocity = map(analogRead(1), 0, 1023, 0, 127);
    if(vel != newVelocity) {
      vel = newVelocity;
      controlChange(CHANNEL, 11, vel);
    }
    uint8_t newEffect = map(analogRead(2), 0, 1023, 0, 127);
    if(fxc != newEffect) {
      fxc = newEffect;
      controlChange(CHANNEL, 12, fxc);
    }
    uint8_t newRate = map(analogRead(3), 0, 1023, 0, 127);
    if(rate !=newRate) {
      rate = newRate;
      controlChange(CHANNEL, 13, rate);
    }
#endif
    prevReadTime = t;
    digitalWrite(LED, ++heart & 32); // Blink = alive
    MidiUSB.flush();
  }
  (void)MidiUSB.read(); // Discard incoming MIDI messages
}
