/*

ESP32     OLED
VCC         VCC
GND         GND
22          SCL
21          SDA

*/

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include "bitmap.h"
#include <ESP_Adafruit_SSD1306.h>

#define CODE_VERS  "1.1"  // Code version number

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
//SDA = D2, SCL = D1

/* More OLED stuff*/
int oledDraw = 0;
int oledOverride = 0;


/* Debounce timers for buttons  /// lastDebounceTime = millis();*/
long lastDebounceTime = 0;
long debounceDelay = 3000;

/* Inverted timers for oled*/
long invertDelay = 1000; // 60 sec  delay
long lastInvertTime = 0;
int invertedStatus = 0;

/* Timer for active connection to host*/
boolean activeConn = false;
long lastActiveConn = 0;
#define lastActiveDelay 30000
boolean bootMode = true;

/* Neo Pixel Setup */
#define NEOPIN         5
#define NUMPIXELS      1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);


/* Pre-define NeoPixel colours,  eg. pixels.setPixelColor(0, BLUE);*/
#define BLUE      0x0000FF
#define GREEN     0x008000
#define RED       0xFF0000
#define BLACK     0x000000 // OFF


/*vars for serial input*/
String inputString = "";
boolean stringComplete = false;

/* cycle screen DisplayStyles */
int displayChangeMode = 2;
long lastDisplayChange;
#define displayChangeDelay 18000  // Time between "DisplayStyle" changes


void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x78 >> 1);

  display.clearDisplay();
  display.setTextColor(WHITE);

  display.setRotation(0);// Rotate the display at the start:  0, 1, 2 or 3 = (0, 90, 180 or 270 degrees)
  display.clearDisplay();

  /* stops text wrapping*/
  display.setTextWrap(false); // Stop  "Loads/Temps" wrapping and corrupting static characters


  /* Serial setup, start serial port at 9600 bps and wait for port to open:*/
  Serial.begin(9600); // 32u4 USB Serial Baud Rate
  inputString.reserve(200);

  /* Set up the NeoPixel*/
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(100); // Global Brightness
  pixels.show(); // Turn off all Pixels

  /*Initial Load screen*/

  splashScreen();


}

void loop() {

  /*Serial stuff*/
  serialEvent();
  activityChecker();

  /*change display screen*/
  if ((millis() - lastDisplayChange) > displayChangeDelay)
  {
    if (displayChangeMode == 1) {
      displayChangeMode = 2;
      display.fillRect(0, 0, 128 , 64, BLACK);

    }
    else if (displayChangeMode == 2) {
      displayChangeMode = 3;
      display.fillRect(0, 0, 128 , 64, BLACK);
    }
    else if (displayChangeMode == 3) {
      displayChangeMode = 1;
      display.fillRect(0, 0, 128 , 64, BLACK);
    }

    lastDisplayChange = millis();
  }


  /* OLED DRAW STATS */
  if (stringComplete) {

    if (bootMode) {

      splashScreen2();
      display.clearDisplay();
      display.display();

      bootMode = false;
    }

    lastActiveConn = millis();


    /*NeoPixel Idle colour*/
    pixels.setPixelColor(0, BLUE);
    pixels.show(); // This sends the updated pixel color to the hardware


    if (displayChangeMode == 1) {
      DisplayStyle1();
    }
    else if (displayChangeMode == 2) {
      DisplayStyle1();
    }
    else if (displayChangeMode == 3) {
      DisplayStyle1();
    }


    inputString = "";
    stringComplete = false;

    /* Keep running anti screen burn, whilst serial is active */
    if ((millis() - lastInvertTime) > invertDelay && oledDraw == 1) {
      lastInvertTime = millis();
      //inverter();



    }
  }
}

//END of Main Loop


/*
    __ _   _ _  _  ___ _____ ___ ___  _  _ ___
  | __| | | | \| |/ __|_   _|_ _/ _ \| \| / __|
  | _|| |_| | .` | (__  | |  | | (_) | .` \__ \
  |_|  \___/|_|\_|\___| |_| |___\___/|_|\_|___/

*/


//-------------------------------------------  Serial Events -------------------------------------------------------------

void serialEvent() {

  while (Serial.available()) {          //  32u4 USB Serial Available?
    char inChar = (char)Serial.read();  // Read 32u4 USB Serial
    //Serial.print(inChar); // Debug Incoming Serial

    inputString += inChar;
    if (inChar == '|') {
      stringComplete = true;

      //display.drawRect(82, 0, 44, 10, WHITE); // Position Test
      display.fillRect(115, 0, 42, 10, BLACK); // Flash top right corner when updating
      display.display();

    }


  }
}
void activityChecker() {
  if (millis() - lastActiveConn > lastActiveDelay)

    activeConn = false;
  else
    activeConn = true;
  if (!activeConn) {
    if (invertedStatus)

      //Turn off display when there is no activity
      display.invertDisplay(0);
    display.clearDisplay();
    display.display();

    //Experimental,  attempt to stop intermittent screen flicker when in no activity mode "screen off" (due to inverter function?) fill the screen 128x64 black rectangle
    display.fillRect(0, 0, 128, 64, BLACK);
    display.display();
    oledDraw = 0;

    //Turn off NeoPixel when there is no activity
    pixels.setPixelColor(0, BLACK);
    pixels.show();

  }
}

//-------------------------------------------- Anti Screen Burn inverter ------------------------------------------------


void antiBurn() {
  display.invertDisplay(0);
  display.fillRect(0, 0, 128, 64, BLACK);
  display.display();
  oledDraw = 0;

}


void inverter() {
  if ( invertedStatus == 1 ) {
    invertedStatus = 0;
  } else {
    invertedStatus = 1;
  };
  display.invertDisplay(invertedStatus);
  display.display();
}

//--------------------------------------------- Splash Screens --------------------------------------------------------
void splashScreen() {

  // USB Serial Screen

  display.drawBitmap(0, 0, WaitingDataBMP, 128, 64, WHITE);
  display.display();

  pixels.setPixelColor(0, RED);
  pixels.show(); // This sends the updated pixel color to the hardware

}


void splashScreen2() {
  display.drawBitmap(0, 0, WaitingDataBMP, 128, 64, WHITE);
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display();
}
