#include <Adafruit_DotStar.h>
#include <SPI.h>

#define NUMPIXELS 1
#define DATAPIN 7
#define CLOCKPIN 8

Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);

/* 
  IR Breakbeam sensor demo!
  from adafruit site
*/

#define ALARM 1
#define LEDPIN 2
// Pin 13: Arduino has an LED connected on pin 13
// Pin 11: Teensy 2.0 has the LED on pin 11
// Pin  6: Teensy++ 2.0 has the LED on pin 6
// Pin 13: Teensy 3.0 has the LED on pin 13
 
#define SENSORPIN 4
 
void setup() {
  strip.begin();
  strip.show(); // turns everything off
  
  // initialize the LED pin as an output:
  pinMode(LEDPIN, OUTPUT);    
  pinMode(ALARM, OUTPUT);  
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);     
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup
  digitalWrite(ALARM, HIGH);
  Serial.begin(9600);
}
 
void loop(){
  // read the state of the pushbutton value:
  const int sensorState = digitalRead(SENSORPIN);
 
  // check if the sensor beam is broken
  // if it is, the sensorState is LOW:
  if (sensorState == LOW) {     
    // turn LED on:
    digitalWrite(LEDPIN, HIGH);  
    digitalWrite(ALARM, LOW);

    Serial.println("Broken");
    
  } 
  else {
    // turn LED off:
    digitalWrite(LEDPIN, LOW); 
    digitalWrite(ALARM, HIGH);

    Serial.println("Unbroken");
  }

}
