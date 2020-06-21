# Get a chim when someone enters the driveway

Using a combination of Trinket M0, a IR light break sensor, nrf24l01 radios and a raspberry PI to receive a ding dong when someone walks or drives into the drive way


We will play music when someone or something breaks the light beam which should indicate they have entered the driveway


Because we're using Trinket M0 outside in the driveway and it has limited pins to transmit and also respond to a 
light break sensor GPIO pin we need two Trinket M0's.   We also have batteries so we'll be using verters from adafruit.
IMO these are nice beause they both boost and buck voltage while not being as good of a boost they serve both use cases
well so we'll be able to use a feature of the buck/boost converter to power on/off another Trinket M0 that will when activated
send our signal to our home Trinket that will activate this program running on our raspberry PI 3 B+ to play some music chims

*Note we could simplify our setup with a bigger micro controller that would combine irbreak.ino and transmitter.ino two trinkets into 1 but I could not figure out away to send message without using all available trinket pins*

Part 1 - https://youtu.be/JmLufwKJ6Tg (demo of the setup working with bread boards)

# IR break beam sensor - trinket m0 
see: irbreak.ino
![alt text](https://github.com/taf2/alert-sensor/blob/master/images/ir-break-beam.jpg?raw=true)

Job is to enable power to trinket transmitter when pin 4 is low

PIN 4 - yellow wire from break beam sensor when LOW the beam is broken
PIN 2 - attached to enable pin on Boost/Buck converter
GND   - common ground connected to break beam sensor and power source (very important otherwise it will never go low)
BAT   - connected to postive battery

# Wiring nRF24L01 to trinkets
![alt text](https://github.com/taf2/alert-sensor/blob/master/images/Pinout-nRF24L01-PA-LNA-External-Antenna-Wireless-Transceiver-Module.png?raw=true)
![alt text](https://github.com/taf2/alert-sensor/blob/master/images/nrf24l01-wiring.jpg?raw=true)

```
[YELLOW] - CE (Chip Enable) is an active-HIGH pin. When selected the nRF24L01 will either transmit or receive, depending upon which mode it is currently in.
[ORANGE] - CSN (Chip Select Not) is an active-LOW pin and is normally kept HIGH. When this pin goes low, the nRF24L01 begins listening on its SPI port for data and processes it accordingly.
[GREEN]  - SCK (Serial Clock) accepts clock pulses provided by the SPI bus Master.
[BLUE]   - MOSI (Master Out Slave In) is SPI input to the nRF24L01.
[PURPLE] - MISO (Master In Slave Out) is SPI output from the nRF24L01.
[GREY]   - IRQ is an interrupt pin that can alert the master when new data is available to process.
```

# transmitter - trinket m0 nRF24L01

see: transmitter.ino
![alt text](https://github.com/taf2/alert-sensor/blob/master/images/transmitter.jpg?raw=true)

Job is to send a message when power is on.  For usage of nrf24l01 see https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/

PIN 0 - yellow wire
PIN 1 - orange wire
PIN 2 - purple wire
PIN 3 - green wire
PIN 4 - blue wire
GND
BAT

# receiver - trinket m0 nRF24L01
*Note, i did not need to attach the grey wire in both transmitter or receiver.   On the receiver we seem to be able to get away with not attaching the yellow wire - this frees up one pin for signaling our pi*

see: receiver.ino
![alt text](https://github.com/taf2/alert-sensor/blob/master/images/receiver.jpg?raw=true)

PIN 0 - signal wire (not the yellow wire pin held high and goes low when we received a signal)
PIN 1 - orange wire
PIN 2 - purple wire
PIN 3 - green wire
PIN 4 - blue wire
GND
BAT

# chim player - raspberry pi

see play.c
see: https://lauri.xn--vsandi-pxa.com/2013/12/implementing-mp3-player.en.html
be sure to install audio libraries
```
sudo apt install libmad0-dev libpulse-dev pulseaudio
# and run the pulseaudio daemon
pulseaudio -D
```
*Note doorbell.mp3 is located in the checkout directory which in my case is /home/pi/alert-sensor/doorbell.mp3 anywhere else and you will need to modify play.c*

PIN 36 - held HIGH unless we need to play music chim
PIN 34 - GND for led indicator
PIN 32 - held LOW unless we need to blink the led because of a break in our sensor beam detected


Attaching the raspberry pi to a speaker and play an mp3

# case fitting
For the remote IR sensor and transmitter I'm using a PVC Conduit  L connector.   I [3d printed an antenna cap](parts/antenna-fitting.stl) to plug one of the 
holes for the antenna to sit securely.   On the other open hole I [3d printed another block](parts/ir-sensor-mount.stl) with a cut away for the IR sensor.
I drilled a small 3/4 in hole into the side of the PVC to run a power wire and secured it with a little bit of extra shrink wrap.

See: https://www.homedepot.com/p/Carlon-1-2-in-Schedule-40-and-80-PVC-Type-LB-Conduit-Body-Case-of-20-E986D-CTN/202205910

# power setup

To power the remote setup I don't have good power from my driveway nor do I want to dig it up to run power lines. So solar is a 
nice option and hopefully will allow this project to run for a very long time without having to replace batteries.  To do this
I use adafruit lipo charger learn more here: https://learn.adafruit.com/usb-dc-and-solar-lipoly-charger/using-the-charger
