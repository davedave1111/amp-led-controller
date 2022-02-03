#include <FastLED.h>
#include <EasyButton.h>

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


//Create two EasyButton objects for our color and mode buttons
EasyButton colorButton(COLOR_BUTTON_PIN);
EasyButton modeButton(MODE_BUTTON_PIN);

void setup() {
  //This section sets up our LEDs
  delay( 3000 ); //delay for safety
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); //create a strip by adding all LEDs
  FastLED.setBrightness(  currentBrightness ); //sets brightness
  fill_solid( leds, NUM_LEDS, CRGB::Red); //sets the strip to read


  //This section sets up everything relating to our buttons
  //initalize the buttons
  colorButton.begin(); 
  modeButton.begin();

  //This section attaches the callbacks required for short and long presses of each button. 
  colorButton.onPressed(cycleColor);
}



void loop() {

  //call these functions on each EazyButton object to read their states
  colorButton.read();
  modeButton.read();

  //update the led show and the updates per second
  FastLED.show();
  FastLED.delay(1000/ UPDATES_PER_SECOND);
}



//this function is used to cycle the chosen color of the lights
void cycleColor(){
  currentColor++; //iterates the current color

  //if we iterate past 8, we need to restart the cycling, so we set currentColor back to 1
  if(currentColor > 8){
    currentColor = 1;
  }

  //This switch statement handles the cycling of quick colors. There are currently 8 "quick" colors you can cycle through 
  //by short pressing the color button, and are as follows in order: Red, Orange, Yellow, Blue, Indigo, Violet, White
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
