// see: https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
// see: https://learn.adafruit.com/adafruit-trinket-m0-circuitpython-arduino/pinouts
#include <Adafruit_DotStar.h>
/*
 * mapping pins for nrf24l01 to trinket m0
 * SPI MOSI or blue 4 (A4)
 * SPI SCK or green 3 (A3)
 * SPI MISO or purple 2 (A1)
 * 0 is yellow
 * 1 is orange
 */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h>

#define NUMPIXELS 1
#define DATAPIN 7
#define CLOCKPIN 8

#define SENSORPIN A0 // can we use this pin instead for receiving HIGH to send transmission?

/*
 * test without pin 1 e.g. orange to see if we can use this pin as input
 * to activate transmitter otherwise we have to control this by power on/off
 * with another trinket
 */

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);


//create an RF24 object
RF24 radio(0, 1);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";
int count = 0;
char buf[64];

void setup() {

  strip.begin();
  strip.show(); // turns everything off
  
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  
  //set the address
  radio.openWritingPipe(address);
  
  //Set module as transmitter
  radio.stopListening();

}

/* while the power is on spam alerts */
void loop() {

  sprintf(buf, "Alert: %d", count++);
  radio.write(&buf, strlen(buf));

}
