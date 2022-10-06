#include <LiquidCrystal.h> // LCD
#include <DS3231.h> // Clock - http://www.rinkydinkelectronics.com/library.php?id=73
#include "SoftwareSerial.h" // For MP3 Serial Communication
#include "DFRobotDFPlayerMini.h" // For MP3 DFPlayer mini Module
#include <FastLED.h>
#include <DMXSerial.h>
#define DMX_USE_PORT1
#include "DMX_LED.h"
#include "Song.h"

// Init LCD pins and LCD object
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);


// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t MP3_TX_PIN = 2; // Connects to module's RX 
static const uint8_t MP3_RX_PIN = 3; // Connects to module's TX 
static const uint8_t MP3_BUSY_PIN = 13; // Connects to module's BUSY


// Create the Player object
SoftwareSerial softwareSerial(MP3_RX_PIN, MP3_TX_PIN);
DFRobotDFPlayerMini player;


// Is preforming action? aka blinking
bool blinking = false;
bool dayModeStart = false; // did day mode just started
bool nightModeStart = false; // did night mode just started
bool notPlaying = true; // is not playing
bool countDownStarted = false; // did countdown started
bool countDownFinished = false; // did countdown finished
unsigned long blinkingMillis = 0; // Store the overall time LED is blinking
unsigned long countdownMillis = 0; // Store the overall time of countdown
unsigned long blinkInterval = 300000; // interval at which to blink (milliseconds)
unsigned long nightBlinkInterval = 13000; // interval at which to blink (milliseconds)


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
};

CRGB no_color = CRGB(0, 0, 0);
#define RAINBOW_SIZE (sizeof(rainbow_colors)/sizeof(*rainbow_colors))

// DMS leds
DMX_LED my_dmx_leds[] = {
  // DMX_LED(73,74,75, no_color),
  // DMX_LED(1,2,3, no_color),
  // DMX_LED(67,68,69, no_color),
  DMX_LED(13,14,15, no_color),
  DMX_LED(109,110,111, no_color),
  DMX_LED(40,41,42, no_color),
  DMX_LED(49,50,51, no_color)
};
#define DMX_LED_SIZE (sizeof(my_dmx_leds)/sizeof(*my_dmx_leds))

// Songs
int rate = 0;
Song my_songs[]= {
  Song(120,1), // rainbow & waterfall
  Song(120,2), // dolly & miley
  Song(120,3), // here comes the sun
  Song(120,5), // robert plant
  Song(120,6), // over the rainbow (original)
  Song(120,7), // bob marley rainbow country
  Song(120,8), // make me rainbow (jazz)
  Song(120,9), // over the rainbow - Israel Kamakawiwoole
  Song(120,10), // shes a rainbow - rolling stones
  Song(120,11), // over the rainbow - bossa nova
  Song(120,12), // ananim hem satlanim
  Song(120,13), // over the rainbow - acoustic singer
};
#define SONGS_NUM (sizeof(my_songs)/sizeof(*my_songs))
Song countDownSong = Song(0,4); // CountDown


// Peripheral leds
#define NUM_LEDS 100

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.
#define DATA_PIN 8

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() {
  // Set up the relay pin and set it off
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Shutdown relay at first

  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Init RTC module
  rtc.begin();

  // Wait for one second before DMX
  delay(1000);
  // Init DMX serial connection - lights
  DMXSerial.init(DMXController);
  
  // Init serial port for DFPlayer Mini
  // Wait for one second before softwareSerial
  delay(1000);
  softwareSerial.begin(9600);
  
  // Start communication with DFPlayer Mini
  // Wait for one second before init DFPlayer
  delay(3000);
  if (player.begin(softwareSerial)) {
    lcd.print("Rainbow Chasers!");
    
    // Set busy pin as input
    pinMode(MP3_BUSY_PIN, INPUT);

    player.enableDAC();
    player.outputSetting(true, 100);

    // Set volume to maximum (0 to 30).
    player.volume(30);
  } else {
    lcd.print("DF Failed!");
  }

  unsigned long unixTime = rtc.getUnixTime(rtc.getTime());
  randomSeed(unixTime);

  // Peripheral leds setup
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  FastLED.clear();
  FastLED.show();
}


void loop() {
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);

  // Get time
  Time my_time = rtc.getTime();
  
  if (is_day_mode(my_time)) {
    prepare_day(my_time);
    day_mode(my_time);
    // day_mode_continuous();
  }
  else {
    prepare_night(my_time);
    night_mode(my_time);
  }
}


// Return True if time is between 8:00 and 20:00
boolean is_day_mode(Time my_time) {
  return (8 <= my_time.hour && my_time.hour <= 20 );
}


// Continuous day mode logic
void day_mode_continuous() {
  start_fountain();
}


// Day mode logic
void day_mode(Time my_time) {
  // hi day
  
  if ((8 <= my_time.hour && my_time.hour < 12) ||
      (14 <= my_time.hour && my_time.hour < 18)) {
    if (!blinking) {
      // Start blinking
      start_fountain();
      blinking = true;
      blinkingMillis = millis(); 
    }
  }
  else {
    stop_fountain();
    blinking = false;
  }
  
  close_lights();
  stop_peripheral_leds();
  delay(1000);
}


// Night mode logic
void night_mode(Time my_time) {
  // hi night
  
  start_peripheral_leds();
  
  // check if its time to countdown
  if (my_time.min == 56 && !blinking && !countDownStarted) {
    player.play(countDownSong.getTrackNumber());
    countDownStarted = true;
    countdownMillis = millis();
  }
  if (countDownStarted) {
    notPlaying = digitalRead(MP3_BUSY_PIN);
    if (millis() - countdownMillis >= nightBlinkInterval && notPlaying) {
      countDownFinished = true;
    }    
  }

  // if (my_time.min%1 == 0 && countDownFinished && !blinking ) {
  if (countDownFinished && !blinking ) {
    int track_index = random(SONGS_NUM-1);
    int bpm = my_songs[track_index].getBPM();
    int track_num = my_songs[track_index].getTrackNumber();
    rate = 60000/bpm;
    lcd.print("track:"+String(track_num));
    player.play(track_num);
      
    // Start blinking
    start_fountain();
    blinking = true;
    blinkingMillis = millis();
  }

  if(blinking) {
    //  Check if interval passed
    notPlaying = digitalRead(MP3_BUSY_PIN);
    if (millis() - blinkingMillis >= nightBlinkInterval && notPlaying) {
      // Stop blinking
      stop_fountain();
      blinking = false;
      close_lights();
      countDownStarted = false;
      countDownFinished = false;
    }

    EVERY_N_MILLISECONDS(rate) {
      // Set color
      for(int i=0; i<=DMX_LED_SIZE; ++i) {
        CRGB newColor = rainbow_colors[random(RAINBOW_SIZE)];
        while (my_dmx_leds[i].isSameColor(newColor)) {
          newColor = rainbow_colors[random(RAINBOW_SIZE)];
        } 
        my_dmx_leds[i].setNewColor(newColor);
      }
    }

    // Blend color
    EVERY_N_MILLISECONDS(2) {
      for(int i=0; i<=DMX_LED_SIZE; ++i) {
        my_dmx_leds[i].blendColor();
      }  
    }
  }

  if(!blinking) {
    close_lights();
    // delay(1000);
  }
}

// Preare the day mode function
void prepare_day(Time my_time) {
  char lcd_msg[16] = ""; // LCD Message
  if(!dayModeStart) {
    // start of day mode
    dayModeStart = true;
    nightModeStart = false;
    blinking = false;
    lcd.print("                ");
    lcd.setCursor(0, 1);
  }
  lcd.print("DAY "+String(my_time.hour)+":"+String(my_time.min)+":"+String(my_time.sec));
}

// Preare the night mode function
void prepare_night(Time my_time) {
  char lcd_msg[16]; // LCD Message
  if(!nightModeStart) {
    // start of night mode
    nightModeStart = true;
    dayModeStart = false;
    blinking = false;
    stop_fountain();
    lcd.print("                ");
    lcd.setCursor(0, 1); 
  }
  lcd.print("NIGHT "+String(my_time.hour)+":"+String(my_time.min)+":"+String(my_time.sec));
}

// Start the relay with fountain
void start_fountain(){
  digitalWrite(RELAY_PIN, LOW);
}

// Stop the relay with fountain
void stop_fountain(){
  digitalWrite(RELAY_PIN, HIGH);
}

// Close DMX lights
void close_lights() {
  for(int i=0; i<=DMX_LED_SIZE; ++i) {
    my_dmx_leds[i].setColor(no_color);
  }
}

// Start peripheral leds lights
void start_peripheral_leds() {
  pride();
  FastLED.show();
}

// Stop peripheral leds lights
void stop_peripheral_leds() {
  FastLED.clear();  
}

// ========================================================================================
// pride 2015

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}
