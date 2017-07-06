// NOTE!!!
// Set #undef USE_SOFTWARE_SERIAL in ATTinyCore\hardware\avr\1.1.2\variants\tinyX5\pins_arduino.h

// 60+190+60 = 310

// Fuse/Core config:
// Timer 1 Clock: 64MHz
// Clock: 8MHz internal

// LED_PIN must support PWM,
// ATtiny85: 0 and 1. Use core from https://github.com/SpenceKonde/ATTinyCore
// ATmega328: 3, 5, 6, 9, 10, and 11

//TODO:
// Check datasheet page 99 for required frequency

#define LED_PIN 1
#define BTN1_PIN 3
#define BTN2_PIN 4
#define PIN_UNUSED_1 0
#define TX 2 // As used hardcoded by TinyDebugSerial

// Times are in ms
// DEBOUNCE_DELAY: time to allow jitter on a button before accepting its new state
// DIMM_TIMEOUT:   time to wait, after pressing a button, before starting to dimm
// DIMM_STEP_TIME: time between stepping to the next dimm level
#define DEBOUNCE_DELAY 20
#define DIMM_TIMEOUT 1000
#define DIMM_STEP_TIME 500

// Dimm settings
// dimmMin: minimum dimm level (as written to OCR1A
// dimmStep: increase per dimm step
// dimmMax: value at which the dimm level is reset to dimmMin
#define dimmMin 1
#define dimmStep 20
#define dimmMax 255

// End of configuration

#include <TinyDebugSerial.h> // 2886 -> 210

//#include <SoftwareSerial.h> // 4028 -> 1352
//#define RX 0
//SoftwareSerial Serial(RX, TX); // RX, TX

//#include "FakeSerial.h" // 2676
//FakeSerial Serial;

#ifndef SIGRD // used in boot.h by boot_signature_byte_get_short(addr)
#define SIGRD RSIG
#endif

//TODO: remove after fuse checking/fixing
#include <avr/boot.h>

void setup() {
  pinMode(PIN_UNUSED_1, INPUT_PULLUP);
  
  // put your setup code here, to run once:
  OSCCAL = 0x6A;
  Serial.begin(9600);

  // TODO: remove all this checking stuff
  Serial.println(F("PWM frequency, register check - pre setup:"));
  Serial.print(F("Fuses (L/H/E): ")); // http://www.engbedded.com/fusecalc/
  Serial.print(boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS), HEX);
  Serial.print(F("/"));
  Serial.print(boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS), HEX);
  Serial.print(F("/"));
  Serial.print(boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS), HEX);
  Serial.println();
  Serial.print(F("Lock bits: "));
  Serial.println(boot_lock_fuse_bits_get(GET_LOCK_BITS));
  Serial.print(F("Signature: "));
  Serial.print(boot_signature_byte_get(0), HEX);
  Serial.print(boot_signature_byte_get(2), HEX);
  Serial.println(boot_signature_byte_get(4), HEX);
  Serial.print(F("8.0 MHz cal: "));
  Serial.println(boot_signature_byte_get(1), HEX);
  Serial.print(F("6.4 MHz cal: "));
  Serial.println(boot_signature_byte_get(3), HEX);
  Serial.print(F("PLLCSR: ")); Serial.println(PLLCSR, HEX);
  Serial.print(F("TCCR1: ")); Serial.println(TCCR1, HEX);
  Serial.print(F("OCR1C: ")); Serial.println(OCR1C, HEX);
  Serial.print(F("OCR1A: ")); Serial.println(OCR1A, HEX);
  Serial.print(F("TCCR0A: ")); Serial.println(TCCR0A, HEX);
  Serial.print(F("TCCR0B: ")); Serial.println(TCCR0B, HEX);

  Serial.println(F("Running setup"));
  buttonsSetup();
  lightSetup();

  Serial.println(F("PWM frequency, register check - pre setup:"));
  Serial.print(F("PLLCSR: ")); Serial.println(PLLCSR, HEX);
  Serial.print(F("TCCR1: ")); Serial.println(TCCR1, HEX);
  Serial.print(F("OCR1C: ")); Serial.println(OCR1C, HEX);
  Serial.print(F("OCR1A: ")); Serial.println(OCR1A, HEX);
  Serial.print(F("TCCR0A: ")); Serial.println(TCCR0A, HEX);
  Serial.print(F("TCCR0B: ")); Serial.println(TCCR0B, HEX);

  Serial.println(F("Blink LED"));
  lightOn();
  Serial.println(F("PWM frequency, register check - pre setup:"));
  Serial.print(F("PLLCSR: ")); Serial.println(PLLCSR, HEX);
  Serial.print(F("TCCR1: ")); Serial.println(TCCR1, HEX);
  Serial.print(F("OCR1C: ")); Serial.println(OCR1C, HEX);
  Serial.print(F("OCR1A: ")); Serial.println(OCR1A, HEX);
  Serial.print(F("TCCR0A: ")); Serial.println(TCCR0A, HEX);
  Serial.print(F("TCCR0B: ")); Serial.println(TCCR0B, HEX);

  delay(500);
  lightOff();

  Serial.println(F("PWM frequency, register check - pre setup:"));
  Serial.print(F("PLLCSR: ")); Serial.println(PLLCSR, HEX);
  Serial.print(F("TCCR1: ")); Serial.println(TCCR1, HEX);
  Serial.print(F("OCR1C: ")); Serial.println(OCR1C, HEX);
  Serial.print(F("OCR1A: ")); Serial.println(OCR1A, HEX);
  Serial.print(F("TCCR0A: ")); Serial.println(TCCR0A, HEX);
  Serial.print(F("TCCR0B: ")); Serial.println(TCCR0B, HEX);
}

uint8_t wasTouching = false;
unsigned long loopStart = 0;
unsigned long now = 0;

void loop() {
  loopStart = millis();

  uint8_t stableTouch = buttonsReadTouch();

  if(stableTouch != wasTouching) {
    Serial.print("stableTouch: ");
    Serial.println(stableTouch);

    // state change
    if(stableTouch == 3) {
      Serial.println("EASTER EGG MODE!");
      easterEgg();
    } else if(stableTouch && !wasTouching) {
      // Release -> Touch
      Serial.println("Touch");
      onTouch();
    } else if(!stableTouch && wasTouching) {
      // Touch -> Release
      Serial.println("Release");
      onTouchRelease();

      deepSleep();
    }
    wasTouching = stableTouch;
  } else if(stableTouch) {
    // Still touching
    onTouching();
  }
}
