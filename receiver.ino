// see: https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
// see: https://learn.adafruit.com/adafruit-trinket-m0-circuitpython-arduino/pinouts
#include <Adafruit_DotStar.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/*
 * mapping pins for nrf24l01 to trinket m0
 * SPI MOSI or blue 4 (A4)
 * SPI SCK or green 3 (A3)
 * SPI MISO or purple 2 (A1)
 * 0 is yellow
 * 1 is orange
 * 
 * because we are only a receiver and not transmitter we'll switch pin 0 (yellow) 
 * to be our buzzer pin e.g. play a sound when we have received a signal
 * 
 */

#define NUMPIXELS 1
#define DATAPIN 7
#define CLOCKPIN 8
#define BUZZER 0 // instead of being able to transmit we'll use this for light

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

//create an RF24 object
RF24 radio(0, 1);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";
int buzzCleared = 1;

int lastBuzz = -1;
int cycles = 0;

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show(); // turns everything off


  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  
  //set the address
  radio.openReadingPipe(0, address);
  
  //Set module as receiver
  radio.startListening();

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH); // buzzer high is off
}

void loop() {
  if (radio.available()) {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    Serial.println(text);
    strip.setPixelColor(0, 0x0000FF); // blue when we receive a message
    strip.show();
    if (buzzCleared) {
      buzzCleared = 0;
      lastBuzz = cycles = 1;
      Serial.printf("buz at: %d current cycle is %d\n", lastBuzz, cycles);
    }
    digitalWrite(BUZZER, LOW); // enables the buzzer when low e.g. voltage flows through and makes that annoying buzz

  } else {
    if (lastBuzz > 0) {
      //Serial.printf("wait for cycles, buz at: %d current cycle is %d\n", lastBuzz, cycles);

      if (cycles - lastBuzz < 10000) {
        ++cycles;
        return; // continue waiting until more time has past
      }
    }
    buzzCleared = 1;
    strip.setPixelColor(0, 0x000000); // blank after sending
    strip.show();
    digitalWrite(BUZZER, HIGH); // turns off the buzzer when held high

  }
  if (!buzzCleared) {
    ++cycles;
  }
}
