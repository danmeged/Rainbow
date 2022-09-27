#include <LiquidCrystal.h> // LCD
#include <DS3231.h> // Clock - http://www.rinkydinkelectronics.com/library.php?id=73
#include "SoftwareSerial.h" // For MP3 Serial Communication
#include "DFRobotDFPlayerMini.h" // For MP3 DFPlayer mini Module
#include <FastLED.h>
#include <DMXSerial.h>
#define DMX_USE_PORT1
#include "DMX_LED.h"


const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t MP3_TX_PIN = 2; // Connects to module's RX 
static const uint8_t MP3_RX_PIN = 3; // Connects to module's TX 

// Create the Player object
SoftwareSerial softwareSerial(MP3_RX_PIN, MP3_TX_PIN);
DFRobotDFPlayerMini player;

// Is preforming action? aka blinking
bool blinking = false;
bool dayModeStart = false;
bool nightModeStart = false;
unsigned long blinkingMillis = 0; // Store the overall time LED is blinking
unsigned long blinkInterval = 10000; // interval at which to blink (milliseconds)

// Relay
static const uint8_t RELAY_PIN = 10; // the number of the relay pin

// Rainbow colors
CRGB rainbow_colors[] = {
  CRGB(255, 6, 0), //red
  CRGB(255, 80, 0), //orange
  CRGB(255, 255, 0), //yellow
  CRGB(127, 0, 255), // violet
  CRGB(115, 6, 230), // purple
  CRGB(75, 0, 130), //indigo
  CRGB(6, 190, 5), //green
  CRGB(0, 6, 255), //blue
  // CRGB(0, 0, 255), //test
};

#define RAINBOW_SIZE (sizeof(rainbow_colors)/sizeof(*rainbow_colors))

DMX_LED led1(4,5,6, rainbow_colors[0]); // set as red
DMX_LED led2(13,14,15, rainbow_colors[0]); // set as red

void setup() {
  // Set up the relay pin and set it off
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Shutdown relay at first

  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Init RTC module
  rtc.begin();

  // Init DMX serial connection - lights
  DMXSerial.init(DMXController);

  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);
  
  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
    lcd.print("Rainbow Chasers!");
    player.enableDAC();
    player.outputSetting(true, 100);

    // Set volume to maximum (0 to 30).
    player.volume(30);
    // Play the first MP3 file on the SD card
    // player.play(1);
  } else {
    lcd.print("DF Failed!");
  }

}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);

  // Get time
  Time my_time = rtc.getTime();
  
  if (is_day_mode(my_time)) {
    if(!dayModeStart) {
      // start of day mode
      dayModeStart = true;
      nightModeStart = false;
    }
    lcd.print("DAY "+String(my_time.hour)+":"+String(my_time.min)+":"+String(my_time.sec));
    day_mode(my_time);
    // day_mode_continuous();
  }
  else {
    lcd.print("NIGHT "+String(my_time.hour)+":"+String(my_time.min)+":"+String(my_time.sec));
    if(!nightModeStart) {
      // start of night mode
      nightModeStart = true;
      dayModeStart = false;
      stop_fountain();
    }
    night_mode(my_time);

  }
  
}


// Return True if time is between 8:00 and 20:00
boolean is_day_mode(Time my_time) {
  return (8 <= my_time.hour && my_time.hour <= 20 );
  // return (48 <= my_time.min && my_time.min < 49 );
}


// Continuous day mode logic
void day_mode_continuous() {
  start_fountain();
}

// Day mode logic
void day_mode(Time my_time) {
  // hi day
  // TODO: is (my_time.sec < 2) ok?
  if (my_time.min % 2 == 0 && my_time.sec < 2) {
   if (!blinking) {
      // Start blinking
      start_fountain();
      blinking = true;
      blinkingMillis = millis();
    }
  }
  
  // Check the time passed from when started blinking
  if(blinking) {
    if (millis() - blinkingMillis >= blinkInterval) {
        // Stop blinking
        stop_fountain();
        blinking = false;
    }
  }

  // If not blinking, wait for a second
  if(!blinking) {
    delay(1000);
  }
}


// Night mode logic
void night_mode(Time my_time) {
  // hi night
}




void start_fountain(){
  digitalWrite(RELAY_PIN, LOW);
}

void stop_fountain(){
  digitalWrite(RELAY_PIN, HIGH);
}

