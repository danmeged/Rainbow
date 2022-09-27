#include <LiquidCrystal.h> // LCD
#include <DS3231.h> // Clock - http://www.rinkydinkelectronics.com/library.php?id=73
#include "SoftwareSerial.h" // For MP3 Serial Communication
#include "DFRobotDFPlayerMini.h" // For MP3 DFPlayer mini Module

const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);


// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2; // Connects to module's RX 
static const uint8_t PIN_MP3_RX = 3; // Connects to module's TX 

// Create the Player object
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

// is preforming action? aka blinking
boolean blinking = false;
unsigned long blinkStartTime;
unsigned long blinkPeriod = 10000;

void setup() {


  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Init RTC module
  rtc.begin();

  /*
  // DEBUG - REMOVE!!!!
  Serial.begin(9600);
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.print(rtc.getTimeStr());
  // DEBUG - REMOVE!!!!
  */

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

  Time my_time = rtc.getTime();
  
  if (is_day_mode(my_time)) {
    lcd.print("DAY "+String(my_time.hour)+":"+String(my_time.min));
  }
  else {
    lcd.print("NIGHT "+String(my_time.hour)+":"+String(my_time.min));
  }
  
}

// Return True if time is between 8:00 and 20:00
boolean is_day_mode(Time my_time) {
  return (8 <= my_time.hour && my_time.hour <= 20 );
}

// Day mode logic
void day_mode() {
  // hi day
}

// Night mode logic
void night_mode() {
  // hi night
}
