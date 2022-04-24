/*
 *  LED Animations With Input Interrupt (Button).
 *
 *  Copyright (C) 2010 Efstathios Chatzikyriakidis (stathis.chatzikyriakidis@gmail.com)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// include for portable "non-local" jumps.
#include <setjmp.h>

// define some new type name definitions.
typedef void (*animFunc) (void);

// array with the pin numbers of the leds.
const int ledPins[] = {3, 4, 5, 6, 7, 8, 9, 10};

// calculate the number of the leds in the array.
const int NUM_LEDS = (int) (sizeof (ledPins) / sizeof (const int));

const int buttonPin = 2;   // the pin number of the button element.
const int buttonIRQ = 0;   // the IRQ number of the button pin.
const long delayTime = 50; // delay time in millis for the leds.

const int BOUNCE_DURATION = 200; // define a bounce time for the button.

volatile int buttonState = 0; // the default animation to start showing.
volatile long bounceTime = 0; // variable to hold ms count to debounce a pressed switch.

// information to restore calling environment.
jmp_buf buf;

// array of animation functions' pointers.
const animFunc ledAnims[] = { fr_one_at_a_time,
                              blink_one_at_a_time,
                              bk_one_at_a_time,
                              blink_all_together,
                              fb_one_at_a_time,
                              loading_effect,
                              fb_all_together,
                              all_anims_together /* this should be the last one */ };

// calculate the number of the animations in the array.
const int NUM_ANIMS = (int) (sizeof (ledAnims) / sizeof (const animFunc));

// startup point entry (runs once).
void setup() {
  // set button element as an input.
  pinMode(buttonPin, INPUT);

  // attach an ISR for the IRQ (for button presses).
  attachInterrupt(buttonIRQ, buttonISR, RISING);

  // set each led pin as an output.
  for (int i = 0; i < NUM_LEDS; i++)
    pinMode(ledPins[i], OUTPUT);
}

// loop the main sketch.
void loop()
{
  // save the environment of the calling function.
  setjmp(buf);

  // dark all the leds before any animation start.
  for (int i = 0; i < NUM_LEDS; i++)
    digitalWrite(ledPins[i], LOW);

  // run led animation according to button state.
  ledAnims[buttonState]();
}

// ISR for the button IRQ (is called on button presses).
void buttonISR () {
  // it ignores presses intervals less than the bounce time.
  if (abs(millis() - bounceTime) > BOUNCE_DURATION) {
    // go to next state.
    buttonState++;

    // check bounds for the state of the button (repeat).
    if (buttonState > NUM_ANIMS-1)
      buttonState = 0;

    // set whatever bounce time in ms is appropriate.
    bounceTime = millis(); 

    // go to the main loop and start the next animation.
    longjmp(buf, 0);
  }
}

// animation #1.
void fr_one_at_a_time () {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[i], LOW);
  }
}

// animation #2.
void bk_one_at_a_time () {
  for (int i = NUM_LEDS-1; i >= 0; i--) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[i], LOW);
  }
}

// animation #3.
void fb_one_at_a_time () {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[i], LOW);
  }

  for (int i = NUM_LEDS-1; i >= 0 ; i--) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[i], LOW);
  }
}

// animation #4.
void fb_all_together () {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
  }

  for (int i = NUM_LEDS-1; i >= 0 ; i--) {
    digitalWrite(ledPins[i], LOW);
    delay(delayTime);
  }
}

// animation #5.
void blink_one_at_a_time () {
  for (int i = 0; i < NUM_LEDS; i++) {
    for (int j = 0; j < 10; j++) {
      digitalWrite(ledPins[i], HIGH);
      delay(delayTime);
      digitalWrite(ledPins[i], LOW);
      delay(delayTime);
    }
  }
}

// animation #6.
void blink_all_together () {
  for (int j = 0; j < 10; j++) {
    for (int i = 0; i < NUM_LEDS; i++)
      digitalWrite(ledPins[i], HIGH);

    delay(delayTime);

    for (int i = 0; i < NUM_LEDS; i++)
      digitalWrite(ledPins[i], LOW);

    delay(delayTime);
  }
}

// animation #7.
void loading_effect() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(ledPins[i], LOW);
    delay(delayTime);
  }
}

// all animations.
void all_anims_together () {
  for (int i = 0; i < NUM_ANIMS-1; i++)
    ledAnims[i]();
}
