#include <FastLED.h>
#include <ezButton.h>

#define LED_PIN     0
#define NUM_LEDS    90  //I have 44 LEDs in the back of the amp and 46 in front
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];


#define COLOR_BUTTON_PIN  4
#define MODE_BUTTON_PIN  6



#define UPDATES_PER_SECOND 100


//led_contoller_sketch
//Author: davedave1111
//Purpose: 
//This code was written for a metro mini,the adafruit clone of an arduino nano.
//It provides the logic for the small control box I created to drive LED lights 
//I've installed in my 120 watt Bugera amplifier, with two pushbuttons to allow a 
//user to change the color and mode of the LED's. Additionally, I've included a 
//MPU-6050 accelerometer to be used to activate the lights upon sound for one of the
//modes I plan on writing. 

//declare and initialize state variables for the buttons
int colorButtonState = 0;
int modeButtonState = 0;

//declear and initalize brightness variable to 100%
int currentBrightness = 100;

//declare a variable to keep track of the current color
int currentColor = 1;

int colorButtonPressTime = 0;
int modeButtonPressTime = 0;


ezButton colorButton(COLOR_BUTTON_PIN);
ezButton modeButton(MODE_BUTTON_PIN);

void setup() {
  //Set a power on delay, declare our LEDS and add them to a strip, set the brightness, and display red
  delay( 3000 ); 
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  currentBrightness );
  fill_solid( leds, NUM_LEDS, CRGB::Red);
}



void loop() {
  
  colorButton.loop(); // MUST call the loop() function first

  
  if(colorButton.isPressed()){
    cycleColor();
  }

  FastLED.show();
  FastLED.delay(1000/ UPDATES_PER_SECOND);
}

  







void cycleColor(){
  currentColor++;

  if(currentColor > 8){
    currentColor = 1;
  }

  switch (currentColor) {
  case 1:
    fill_solid( leds, NUM_LEDS, CRGB::Red);
    break;
  case 2:
    fill_solid( leds, NUM_LEDS, CRGB::Orange);
    break;
  case 3:
    fill_solid( leds, NUM_LEDS, CRGB::Yellow);
    break;
  case 4:
    fill_solid( leds, NUM_LEDS, CRGB::Green);
    break;
  case 5:
    fill_solid( leds, NUM_LEDS, CRGB::Blue);
    break;
  case 6:
    fill_solid( leds, NUM_LEDS, CRGB::Indigo);
    break;
  case 7:
    fill_solid( leds, NUM_LEDS, CRGB::Violet);
    break;
  case 8:
    fill_solid( leds, NUM_LEDS, CRGB::White);
    break;
  default:
    break;
  }
  
  
}
