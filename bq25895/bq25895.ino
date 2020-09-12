
BERNEBEER
Home
Github
Contact Me
Home » 3.1A powerbank, 2 x BC1.2 ports, Arduino IDE configurable, 4 RGB LEDS

Gijs van Berne 09/02/2017 No comments
3.1A powerbank, 2 x BC1.2 ports, Arduino IDE configurable, 4 RGB LEDS

A small configurable 3.1A lithium battery powerbank, 2 x BC1.2 power ports, 4 RGB status LEDs double as a powerful torch. Arduino ISP programmable. Actually handles amperage without dropping below 5V output!

BQ25895 charge / boost IC
3.1A combined output current
2 x USB type-A
Quickcharge 2.0 micro USB input, max 3.25A
Atmega328P-MU microcontroller
Arduino IDE programmable (ISP pins broken out)
MAX17043 state-of-charge IC
Deepsleep mode
4 x RGB WS2812b leds
Thermistor pins
6A fuse protection
ESD protection
Tactile switch power (on/off or function)
Serial out (debug)
A small powerbank which uses bq25895’s On The Go 5.3V 3.1A output to power two USB ports + Quickcharge 2.0 high voltage charging. I added an ATMEGA328P-MU to handle all bq’s I2c communications and control 4 WS2812b RGB leds. Program the LEDs using the ISP pins to display animations based on input current, cell voltage, state of charge, program a torch etc. A MAX17043G is placed in parallel with the battery to measure state of charge, also configurable by I2c.


Top

Bottom

Schematic

3.2A test
A few caveats: The microcontroller is powered by OTG so, when bq25895 is in sleep mode, all power is lost. You can only wake the device using the QON pin (connected to the button). A new version of this board is in the making, which will add an extra boost converter (simultaneous charging and boosting with powerpath management), a usb type C  port (replaces one type A) and always on LDO (ability to wake system on usb attach, detach).

Preliminary powerbank code
#include <I2C.h>
#include <FastLED.h>
#include "LowPower.h"

#define BQ25895 0x6a
#define MAX17043 0x36
#define LED_PIN     3
#define NUM_LEDS    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

word stateOfCharge;
byte batteryVoltageMSB;
byte batteryVoltageLSB;
unsigned long batteryVoltage;
int ledPin = 13;
int btnPin = 4;
int ledFet = 5;
int otgPin = A3;
unsigned long previousMillis = 0;
unsigned long previousBrightnessMillis = 0;
unsigned long previousSerialMillis = 0;
int brightness = 0;
boolean ledDirection = 0;
int valueChargeregister;
int inputTypeArrayValue;
byte readByte;
int chargeCurrent;
boolean flashlightMode = false;
boolean checkBatLevelFlag = true;
unsigned long lastBatteryLevelCheck;
int baseBatLevel;
int baseSocLevel;

char inputType [8] [16] = { 
 { "No Input" },
 { "USB SDP" },
 { "CDP, 1.5A" }, 
 { "DCP, 3.25A" }, 
 { "High Volt, 1.5A" }, 
 { "Unknown, 0.5A" }, 
 { "Non Std" }, 
 { "OTG" }, 
};

char chargeStatus [4] [12] = { 
 { "No chrg" }, 
 { "Pre chrg" }, 
 { "Fast chrg" }, 
 { "Chrg Done!" },  
};

void setup() {

  // Pinmodes
  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT);
  pinMode(ledFet, OUTPUT);
  pinMode(otgPin, OUTPUT);

  // Enable PMID supply to USBs
  digitalWrite(otgPin, HIGH);
  // P-channel MOSFET connected from PMID to LEDs, drive LOW to supply LEDs
  digitalWrite(ledFet, LOW);

  Serial.begin(9600);
  I2c.begin();

  // Initialise WS2812b leds
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(10);

  // Powerup led indication
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
  }
 
  FastLED.show();

  // Delay for full reset
  delay(1000);

  // Full BQ25895 reset
  I2c.write(BQ25895, 0x14, B10111001);

  // Initialise ADC, force D+ D- detection (anders geen DCP etc detection bij wake from sleep en charger connected) + set ADC to single shot
  I2c.write(BQ25895, 0x02, B11111111);

  // Set boost voltage to 5.3V
  I2c.write(BQ25895, 0x0a, B11000011);

  // Set minimum system voltage to 3.7v 
  I2c.write(BQ25895, 0x03, B01111110);

  // Disable POR
  I2c.write(BQ25895, 0x09, B01001000);

  // Set fast charge current limit (resistor 220ohm on pcb => Kilim / Rilim => 355 (or max 380) / 220 = 1.6A)
  I2c.write(BQ25895, 0x04, B00100000);

  // Set input current limit
  I2c.write(BQ25895, 0x00, B01101000);

  // Set charge termination limit to 192mA (uC + BQ + MAX + leds will not consume more than... )
  I2c.write(BQ25895, 0x05, B00010010);

  // Atmega ADC disabled, save power, not needed
  // Disable the ADC by setting the ADEN bit (bit 7)  of the ADCSRA register to zero.
  ADCSRA = ADCSRA & B01111111;
  
  // Disable the analog comparator by setting the ACD bit (bit 7) of the ACSR register to one.
  ACSR = B10000000;
  
  // Disable digital input buffers on all analog input pins
  DIDR0 = DIDR0 | B00111111;

}

void loop() {

  // Make sure P-channel Mosfet for leds is enhanced
  digitalWrite(ledFet, LOW);

  ////////// BQ and MAX stuff here  
  // Periodically reset BQ25895 watchdog + read relevant BQ and MAX registers 
  unsigned long currentMillis = millis();  
  if ( currentMillis - previousMillis > 1000 ) {

    // Reset Watchdog
    I2c.write(BQ25895, 0x03, B01111110);
  
    // Read input / charge type register, input status
    I2c.read(BQ25895, 0x0b, 1);
    readByte = I2c.receive(); 
    byte bitMask = B11100000;
    inputTypeArrayValue = (readByte & bitMask) >> 5;

    // From input/charge type register, find charge status
    bitMask = B00011000;
    valueChargeregister = (readByte & bitMask) >> 3;
  
    // Read ADC conversion of charge current
    I2c.read(BQ25895, 0x12, 1);
    readByte = I2c.receive();
    chargeCurrent = readByte * 50;
  
    // Read fuel gauge state of charge register
    I2c.read(MAX17043, 0x04, 2);
    stateOfCharge = I2c.receive();

    // Read 12bit battery voltage from VCELL register
    I2c.read(MAX17043, 0x02, 1);
    batteryVoltageMSB = I2c.receive();
    I2c.read(MAX17043, 0x03, 1);
    batteryVoltageLSB = I2c.receive();
    batteryVoltage = word(batteryVoltageMSB, batteryVoltageLSB);
    batteryVoltage = (batteryVoltage >> 4) * 125 / 100;
    
    previousMillis = currentMillis;

  }
    
  ////////// Serial print out info
  unsigned long currentSerialMillis = millis();
  if (currentSerialMillis - previousSerialMillis > 1000) {
  
    Serial.print("Input type: ");
    Serial.println(inputType[inputTypeArrayValue]);
  
    Serial.print("Charge status: ");
    Serial.println(chargeStatus[valueChargeregister]);
  
    Serial.print("Charge current: ");
    Serial.print(chargeCurrent);
    Serial.println("mA");
  
    Serial.print("SOC: ");
    Serial.print(stateOfCharge, DEC);
    Serial.println("%");

    Serial.print("Bat voltage: ");
    Serial.print(batteryVoltage, DEC);
    Serial.println("mV");

    previousSerialMillis = currentSerialMillis;

  }

  ////////// Shutdown stuff here
  // If no charge  
  if (valueChargeregister == 0 && flashlightMode == false ) {

    // Take initial reading
    if (checkBatLevelFlag == true) {
      baseBatLevel = batteryVoltage;
      baseSocLevel = stateOfCharge;
      lastBatteryLevelCheck = millis();
      checkBatLevelFlag = false;
      Serial.println("Initial reading done");
      Serial.println(baseBatLevel);
    }

    // Check if batterylevel mV && SOC stay on same level, if it does within time set, shutdown
    if ( (millis() - lastBatteryLevelCheck) > 30000) {      
      if ( baseBatLevel == batteryVoltage && baseSocLevel == stateOfCharge) {
        shutdownSequence();
      }
      else {
        // Check again...
        checkBatLevelFlag = true;
      }
    }
    
  }

  ////////// LED stuff here    
  // Map stateOfCharge to leds, add 1 to always have 1 led lit
  int batToLed = map(stateOfCharge, 0, 100, 0, NUM_LEDS) + 1;

  // Dim all leds before fresh led sequence
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }

  // Switch cases mapped to valueChargeregister
  switch (valueChargeregister) {
    
    // No charge sequence
    case 0: {
      // Light leds in accordance with SOC
      for (int i = 0; i < batToLed; i++) {
        leds[i] = CRGB::Blue;
      }
      // Dim leds not in accordance with SOC
      for (int i = batToLed; i <= NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      
      // Set brightness in time
      unsigned long currentBrightnessMillis = millis();  
      if (currentBrightnessMillis - previousBrightnessMillis > 100) {
        
        if (ledDirection == 0) {
          brightness++;
        }
        else {
          brightness--;
        }
        if (brightness == 20 || brightness == 0 ) {
          ledDirection = !ledDirection;     
        }    
        FastLED.setBrightness(brightness);
        
        previousBrightnessMillis = currentBrightnessMillis;
        
      }        
      break;      
    }
    
    // Precharge sequence
    case 1: {      
      for (int i = 0; i < batToLed; i++) {
        leds[i] = CRGB::Green;
      }
    
      for (int i = batToLed; i <= NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      
      // Set brightness in time
      unsigned long currentBrightnessMillis = millis();  
      if (currentBrightnessMillis - previousBrightnessMillis > 10) {
        
        if (ledDirection == 0) {
          brightness++;
        }
        else {
          brightness--;
        }
        if (brightness == 30 || brightness == 0 ) {
          ledDirection = !ledDirection;     
        }    
        FastLED.setBrightness(brightness);
        
        previousBrightnessMillis = currentBrightnessMillis;
        
      }        
      break;   
    }
    
    // Fast charge sequence
    case 2: {      
      for (int i = 0; i < batToLed; i++) {
        leds[i] = CRGB::Green;
      }
    
      for (int i = batToLed; i <= NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      
      // Set brightness in time
      unsigned long currentBrightnessMillis = millis();  
      if (currentBrightnessMillis - previousBrightnessMillis > 10) {
        
        if (ledDirection == 0) {
          brightness++;
        }
        else {
          brightness--;
        }
        if (brightness == 30 || brightness == 0 ) {
          ledDirection = !ledDirection;     
        }    
        FastLED.setBrightness(brightness);
        
        previousBrightnessMillis = currentBrightnessMillis;
        
      }        
      break;
    }
    
    // Charge done sequence
    case 3: {      
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Green;
      }
      break;      
    }
    
  } // End switch

  // Show led sequence set in switch case
  FastLED.show();

  ////////// Flashlight stuff
  // Watchdog is not reset while in while loop...
  while (flashlightMode == true) {

      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;
      }

      FastLED.setBrightness(100);
      FastLED.show();

      if (digitalRead(btnPin) == LOW) {
        delay(200);
        flashlightMode = false;
        checkBatLevelFlag = true;
        break;
      }
    
  }

  ////////// Button stuff here
  // Set deepsleepmode BQ25895, BATFET disconnect or btn short press stuff
  if(digitalRead(btnPin) == LOW){
    unsigned long startTime = millis();
    // Other stuff to do when button is pressed here...
    flashlightMode = !flashlightMode;
    while(digitalRead(btnPin) == LOW){
      unsigned long nowTime = millis();
      const int holdTime = 2000;           
      if(nowTime > (startTime + holdTime)){  // If button held for 2000ms
        FastLED.show();
        shutdownSequence();
      }
    }
  }
  
} // End main loop

void shutdownSequence() {
  // Disable ledfet
  digitalWrite(ledFet, HIGH);
  delay(100);
  // Batfet disable + delay bit 3 set to zero (datasheet error), shutdown immediately
  I2c.write(BQ25895, 0x09, B01100000);
}
view rawgistfile1.txt hosted with ❤ by GitHub
Order OSHpark pcb
OSHpark link: https://oshpark.com/shared_projects/okTyvBmD

Recent posts
PCBWay pcb prototype review: 2oz copper, 65x40mm, 5 pcs @ $28
14/10/2017
Rigol DS5022M oscilloscope software, driver and manual
10/09/2017
Charge ‘n Boost: Arduino IDE Programmable Powerbank
16/06/2017
3.1A powerbank, 2 x BC1.2 ports, Arduino IDE configurable, 4 RGB LEDS
09/02/2017
Recent comments
Random Partridge quote
Alan on calculations:

"No offence Lynn, but technically your life's not worth insuring."

Widget created by Folderfabriek
Nifty links
Voltage divider calculator
Trace width current calculator
3D Gerber viewer online
Online component inventory system
Convert mils to mm
Cable voltage drop calculator
Contact
Bernebeer
T. +31(0)502100152
E. gijs@bernebeer.nl
VAT. NL1944.50.120.B.01
Meta
Log in
Entries RSS
Comments RSS
WordPress.org
Send me a message!
Type name here
Type email address here
Type message here (auto-expands)

Theme & content © 2020 Bernebeer
