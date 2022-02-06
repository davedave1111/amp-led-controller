#include <FastLED.h>
#include <EasyButton.h>

//setting up variables for the LED strips
#define LED_PIN     0   //digital pin for the LED strip on the board
#define NUM_LEDS    90  //I have 44 LEDs in the back of the amp and 46 in front
#define LED_TYPE    WS2812B //declare the LED typ
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
CRGB leds[NUM_LEDS]; //create a strip

//Variables for the buttons
#define COLOR_BUTTON_PIN  4 //Digital pin for the color button
#define MODE_BUTTON_PIN  6 //digital pin for the mode button
#define LONG_PRESS_TIME_MS 1000 //this is the time in milliseconds we want for our long press function on each button

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
int currentBrightness = 255;

//This is the variable we will use to keep track of the level we are on for brightness
//it starts at 10 as we are on level 10, 100%
int brightnessTracker = 10;

//declare a variable to keep track of the current color
int currentColor = 1;

//This variable is used to determine how fast colors fade. It is the ms time it delays
int ledFadeDelay = 2;

//Create two EasyButton objects for our color and mode buttons
//*NOTE* the true and false values given to the constructors denote that we
//want to invert the button input and not use the built in pull-up resistor respectively, 
//as I designed the circuit to use 10k ohm pulldown resistors to ground on each button. 
EasyButton colorButton(COLOR_BUTTON_PIN, 40, true, false);
EasyButton modeButton(MODE_BUTTON_PIN, 40, true, false);


//Typical setup function where we get the LEDS and buttons setup 
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

  //for the color button
  colorButton.onPressed(cycleQuickColor); //attaches the cycleQuickColor to the callback, for our short press
  colorButton.onPressedFor(LONG_PRESS_TIME_MS, colorFadeSelector); //attaches the colorFadeSelector function for our long press

  //for the mode button
  modeButton.onPressedFor(LONG_PRESS_TIME_MS, changeBrightness); //attaches the changeBrightness method for the long press
}


//This is the main program loop. It is incredibly sparce, as all it needs to do is call a couple functions
//to service the LEDs and buttons. Most of the heavy lifting is done in the functions below. 
void loop() {

  //ensure we have the correct brightness
  FastLED.setBrightness(currentBrightness);

  //call these functions on each EazyButton object to read their states
  colorButton.read();
  modeButton.read();

  //update the led show and the updates per second
  FastLED.show();
  FastLED.delay(1000/ UPDATES_PER_SECOND);
}



//this function is used to cycle the chosen quick color (ei color of the rainbow) with a single button press
void cycleQuickColor(){
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


//Used to change the current brightness of the lights. This is attached to the isPressedFor callback for the mode
//button. Essentially, if the mode button is held down for the given long press time, it allows the user to choose 
//a desired brightness, from level 1 - 10
void changeBrightness(){
  confirmLight(); //flash the lights to confirm we have successfully entered the long-press mode 
  brightnessTracker = 1; //set us to the lowest brightness
  while(true){

    FastLED.setBrightness(currentBrightness); //set the brightness
    FastLED.show(); //show

    //Here we handle if the brightness tracking variable goes beyond the number of modes. 
    //if it does we reset the value to one
    if(brightnessTracker > 10){
      brightnessTracker = 1;
    }

    //read both buttons to ensure we are aware of the inputs they have at a given moment
    modeButton.read();
    colorButton.read();

    //In this mode, the color button acts as the "exit" button, so we check if it has been pressed.
    if(colorButton.isPressed()){
      currentColor--; //We need to decrement the currentColor variable, as a press will automatically increment the quick color
      confirmLight(); //flash t5he confirm light as we are changing modes
      return; //return ending this function
    }
    
    //in the brightness toggle mode, the mode button acts as user input to iterate through all brightnesses
    if(modeButton.isPressed()){
      brightnessTracker++; //if it is pressed, we increment the brightness tracker
      delay(250); // had to add this delay, because if we dont it moves too fast
    }
    
    //This switch essentially maps the brightnessTracker to values incrementing by 25, with the last
    //incrementing by 30 to hit full brightness. These are roughly evenly spaced out brightnesses, so it works
    //for having 10 different brightness levels. 
    switch (brightnessTracker) {
    case 1:
      currentBrightness = 25;
      break;
    case 2:
      currentBrightness = 50;
      break;
    case 3:
      currentBrightness = 75;
      break;
    case 4:
      currentBrightness = 100;
      break;
    case 5:
      currentBrightness = 125;
      break;
    case 6:
      currentBrightness = 150;
      break;
    case 7:
      currentBrightness = 175;
      break;
    case 8:
      currentBrightness = 200;
      break;
    case 9:
      currentBrightness = 225;
      break;
    case 10:
      currentBrightness = 255;
      break;
    default:
      break;
    }
   
  }
}

//This function is called whenever we need to confirm a button press has triggered a 
//Change in modes, either entering color slider mode, leaving color slider mode, entering
//brightness change mode and leaving brightness mode. It will flash off, on, off, on before 
//ending. 
void confirmLight(){
  FastLED.setBrightness(0);
  FastLED.show();
  delay(250); 
  FastLED.setBrightness(currentBrightness);
  FastLED.show();
  delay(250);
  FastLED.setBrightness(0);
  FastLED.show();
  delay(250);
  FastLED.setBrightness(currentBrightness);
  FastLED.show();
  return;
  
}





//This mode is used to fade between all possible colors to allow for more precise color picking
//beyond just the 8 "quick" colors. It fades through all colors, and when a the mode button is 
//pressed, it retains the color and exits the color selection mode. I may edit this later to also double
//as a stand alone mode, and keep track of whether I am selecting a new color or just using this as an animation. 
void colorFadeSelector(){
  bool isRunning = true;
  confirmLight();
  while(isRunning == true){
    //start from red
    for( int colorStep=0; colorStep <= 255; colorStep++ ) {

    int r = 255;
    int g = 0;
    int b = colorStep;

    // Now loop though each of the LEDs and set each one to the current color
    for(int x = 0; x < NUM_LEDS; x++){
        leds[x] = CRGB(r,g,b);
    }

    modeButton.read();
    if(modeButton.isPressed()){
      confirmLight();
      isRunning = false;
      return;
    }

    // Display the colors we just set on the actual LEDs
    delay(ledFadeDelay); 
    FastLED.show();
    }

    //into blue
    for( int colorStep=255; colorStep >= 0; colorStep-- ) {

      int r = colorStep;
      int g = 0;
      int b = 255;

      // Now loop though each of the LEDs and set each one to the current color
      for(int x = 0; x < NUM_LEDS; x++){
          leds[x] = CRGB(r,g,b);
      }

      modeButton.read(); //Check if the button to exit is pressed, then set isRunning to false to terminate the loop, confirm the selection and return. 
      if(modeButton.isPressed()){
        isRunning = false;
        confirmLight();
        return;
      }

      // Display the colors we just set on the actual LEDs
      delay(ledFadeDelay); 
      FastLED.show();
    }

    //start from blue
    for( int colorStep=0; colorStep <= 255; colorStep++ ) {

      int r = 0;
      int g = colorStep;
      int b = 255; 

      // Now loop though each of the LEDs and set each one to the current color
      for(int x = 0; x < NUM_LEDS; x++){
          leds[x] = CRGB(r,g,b);
      }

      modeButton.read(); //Check if the button to exit is pressed, then set isRunning to false to terminate the loop, confirm the selection and return. 
      if(modeButton.isPressed()){
        isRunning = false;
        confirmLight();
        return;
      }
      

      // Display the colors we just set on the actual LEDs
      delay(ledFadeDelay); 
      FastLED.show();
    } 

    //into green
    for( int colorStep=255; colorStep >= 0; colorStep-- ) {

      int r = 0;
      int g = 255;
      int b = colorStep; 

      // Now loop though each of the LEDs and set each one to the current color
      for(int x = 0; x < NUM_LEDS; x++){
          leds[x] = CRGB(r,g,b);
      }

      modeButton.read(); //Check if the button to exit is pressed, then set isRunning to false to terminate the loop, confirm the selection and return. 
      if(modeButton.isPressed()){
        isRunning = false;
        confirmLight();
        return;
      }

      // Display the colors we just set on the actual LEDs
      delay(ledFadeDelay); 
      LEDS.show();
    }

    //start from green
    for( int colorStep=0; colorStep <= 255; colorStep++ ) {

      int r = colorStep;
      int g = 255;
      int b = 0;

      // Now loop though each of the LEDs and set each one to the current color
      for(int x = 0; x < NUM_LEDS; x++){
          leds[x] = CRGB(r,g,b);
      }

      modeButton.read(); //Check if the button to exit is pressed, then set isRunning to false to terminate the loop, confirm the selection and return. 
      if(modeButton.isPressed()){
        isRunning = false;
        confirmLight();
        return;
      }

      // Display the colors we just set on the actual LEDs
      delay(ledFadeDelay); 
      LEDS.show();
    }

    //into yellow
    for( int colorStep=255; colorStep >= 0; colorStep-- ) {

      int r = 255;
      int g = colorStep;
      int b = 0;

      // Now loop though each of the LEDs and set each one to the current color
      for(int x = 0; x < NUM_LEDS; x++){
          leds[x] = CRGB(r,g,b);
      }

      modeButton.read(); //Check if the button to exit is pressed, then set isRunning to false to terminate the loop, confirm the selection and return. 
      if(modeButton.isPressed()){
        isRunning = false;
        confirmLight();
        return;
      }

      // Display the colors we just set on the actual LEDs
    delay(ledFadeDelay); 
    LEDS.show();
    }

  }

} 
