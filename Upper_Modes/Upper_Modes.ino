//Code includes samples written by Mark Kriegsman and Andrew Tuline.  Thanks so much for all your help guys!

#include <FastLED.h>

//---LED SETUP STUFF
#define LED_PIN 4
#define CLOCK_PIN 5
#define BUTTON_PIN 5
#define COLOR_ORDER GBR
#define NUM_LEDS 216      // Change to reflect the number of LEDs you have
int vib_pin = A0;

CRGB leds[NUM_LEDS];      //naming our LED array

//BUTTON SETUP STUFF
byte prevKeyState = HIGH;
int ledMode = 2;  //FIRST ACTIVE MODE

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
// MODE VARIABLES -- Change these numbers to create new interesting modes

int BRIGHTNESS = 100;    //0-255.  Lower number saves battery life, higher number is screamingly bright
int SATURATION = 255;    //0 is white (no color) and 255 is fully saturated with color
int HUE = 0;             //0-255, around the color wheel
int STEPS = 4;           //Wider or narrower bands of color
int SPEEDO = 10;         //The speed of the animation

CRGBPalette16 targetPalette;
// Define variables used by the sequences.
uint8_t  thisfade = 8;                                        // How quickly does it fade? Lower = slower fade rate.
int       thishue = 50;                                       // Starting hue.
uint8_t   thisinc = 1;                                        // Incremental value for rotating hues
uint8_t   thissat = 100;                                      // The saturation, where 255 = brilliant colours.
uint8_t   thisbri = 255;                                      // Brightness of a sequence. Remember, max_bright is the overall limiter.
int       huediff = 256;                                      // Range of random #'s to use for hue
uint8_t thisdelay = 0;                                        // We don't need much delay (if any)

//Reaction Variables
int jumpCount = 0;
bool confettiShow = false;


// Strength Variables
int playMatrix = false;
int matrixCount = 0;
int currentLedPosition = 0;
uint8_t ledstart;                                               // Length of a flashint currentLedPosition = 0;
uint8_t ledlen;                                               // Length of a flash

// Celebration

// Use qsuba for smooth pixel colouring and qsubd for non-smooth pixel colouring
#define qsubd(x, b)  ((x>b)?b:0)                              // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Analog Unsigned subtraction macro. if result <0, then => 0


// SET UP FOR GAME MODE

int GameMode = 2;

//------------------SETUP------------------
void setup()
{
  delay( 2000 ); // power-up safety delay
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  FastLED.addLeds<DOTSTAR, LED_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  //currentBlending = LINEARBLEND;
  //currentPalette = RainbowColors_p;
  pinMode(vib_pin, INPUT);
  Serial.begin(9600);
  FastLED.clear();
}


void loop() {

  int val;
  val = analogRead(vib_pin);
  Serial.print("\n");
  Serial.print(val);
  Serial.print("\n");
  


  switch (GameMode) {
    case 1:
      StrengthGameController();
      break;
    case 2:
      ReactionGameController();
      break;
    default:
      // statements
      break;
  }
}

void StrengthGameController()
{
  int val;
  val = analogRead(vib_pin);
  Serial.print("\n");
//  Serial.print(val);
  Serial.print("\n");
  
  //StrengthGame
  if (playMatrix)
  {
     Serial.println("PLAY MATRIX");
    celebration();

    if (val > 1000)
    {
      matrixCount += 1;
    }

    if (matrixCount >= 300)
    {
      currentLedPosition = 0;
      playMatrix = false;
      FastLED.clear();
    }
  }
  else
  {
    delay(50);

    if (val > 1023)
    {
      StrengthGame(3);
    } else if (val > 1000)
    {
      StrengthGame(1);
    }
  }
}

void ReactionGameController()
{
  int val;
  val = analogRead(vib_pin);
  Serial.print("\n");
  Serial.print(val);
  Serial.print("\n");
  //Reaction Game
  if (val > 800)
  {
    jumpCount += 1;
  }
  reactionGame();

}

void celebration()
{
  //FastLED.clear();
  EVERY_N_MILLIS_I(thisTimer, 100) {                          // This only sets the Initial timer delay. To change this value, you need to use thisTimer.setPeriod(); You could also call it thatTimer and so on.
    uint8_t timeval = beatsin8(10, 20, 50);                   // Create/modify a variable based on the beastsin8() function.
    thisTimer.setPeriod(timeval);                             // Use that as our update timer value.

    matrix_ray(millis() >> 4);                                // This is the main function that's called. We could have not passed the millis()>>4, but it's a quick example of passing an argument.
  }

  EVERY_N_MILLISECONDS(100) {                                 // Fixed rate of a palette blending capability.
    uint8_t maxChanges = 24;
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);
  }

  EVERY_N_SECONDS(5) {                                        // Change the target palette to a random one every 5 seconds.
    static uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }

  FastLED.show();

  Serial.println(LEDS.getFPS());

}

void matrix_ray(uint8_t colorIndex) {                                                 // Send a PWM'd sinewave instead of a random happening of LED's down the strand.

  static uint8_t thisdir = 0;                                                         // We could change the direction if we want to. Static means to assign that value only once.
  static int thisphase = 0;                                                           // Phase change value gets calculated. Static means to assign that value only once.
  uint8_t thiscutoff;                                                                 // You can change the cutoff value to display this wave. Lower value = longer wave.

  thisphase += beatsin8(1, 20, 50);                                                   // You can change direction and speed individually.
  thiscutoff = beatsin8(50, 164, 248);                                                // This variable is used for the PWM of the lighting with the qsubd command below.

  int thisbright = qsubd(cubicwave8(thisphase), thiscutoff);                          // It's PWM time. qsubd sets a minimum value called thiscutoff. If < thiscutoff, then thisbright = 0. Otherwise, thisbright = thiscutoff.

  if (thisdir == 0) {                                                                 // Depending on the direction, we'll put that brightness in one end of the array. Andrew Tuline wrote this.
    leds[0] = ColorFromPalette(currentPalette, colorIndex, thisbright, currentBlending);
    memmove(leds + 1, leds, (NUM_LEDS - 1) * 3);                                      // Oh look, the FastLED method of copying LED values up/down the strand.
  } else {
    leds[NUM_LEDS - 1] = ColorFromPalette( currentPalette, colorIndex, thisbright, currentBlending);
    memmove(leds, leds + 1, (NUM_LEDS - 1) * 3);
  }

} // matrix_ray()

void StrengthGame(int numLights)
{
  fill_solid(leds + currentLedPosition, numLights, CRGB(random(255), random(255), random(255)));
  FastLED.show();// Show a section of LED's*/
  ledstart = currentLedPosition;
  ledlen = numLights;
  currentLedPosition = int(ledstart) + int(ledlen);

  if (currentLedPosition >= NUM_LEDS)
  {
    playMatrix = true;
    matrixCount = 0;
  }
}

void reactionGame()
{
  Serial.print(jumpCount);
  if (confettiShow)
  {
    confetti();
    if (jumpCount >= 500)
    {
      confettiShow = false;
      jumpCount = 0;
    }
  } else
  {
    FastLED.clear();
    lightUpRandom();
    if (jumpCount >= 5)
    {
      confettiShow = true;
      jumpCount = 0;
    }
  }
}


unsigned int dimmer = 1;


void lightUpRandom()
{
  ledstart = random(NUM_LEDS);                               // Determine starting location of flash
  ledlen = 12;                    // Determine length of flash (not to go beyond NUM_LEDS-1)

  fill_solid(leds + ledstart, ledlen, CRGB(random(255), random(255), random(255)));
  FastLED.show();                       // Show a section of LED's
  delay(1000);                                    // each flash only lasts 4-10 milliseconds
  fill_solid(leds + ledstart, ledlen, CHSV(255, 0, 0));     // Clear the section of LED's
  FastLED.show();
}

void confetti()
{
  ChangeMe();                                                 // Check the demo loop for changes to the variables.

  EVERY_N_MILLISECONDS(100) {
    uint8_t maxChanges = 24;
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
  }

  EVERY_N_MILLISECONDS(thisdelay) {                           // FastLED based non-blocking delay to update/display the sequence.
    confetti_pal();
  }

  FastLED.show();
}

// SOLID COLOR -------------------------------------
void Solid()
{
  fill_solid(leds, NUM_LEDS, CHSV(HUE, SATURATION, BRIGHTNESS));
  FastLED.show();
}

void confetti_pal() {                                         // random colored speckles that blink in and fade smoothly

  fadeToBlackBy(leds, NUM_LEDS, thisfade);                    // Low values = slower fade.
  int pos = random16(NUM_LEDS);                               // Pick an LED at random.
  leds[pos] = ColorFromPalette(currentPalette, thishue + random16(huediff) / 4 , thisbri, currentBlending);
  thishue = thishue + thisinc;                                // It increments here.

} // confetti_pal()



void ChangeMe() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.

  uint8_t secondHand = (millis() / 1000) % 15;                // IMPORTANT!!! Change '15' to a different value to change duration of the loop.
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch (secondHand) {
      case  0: targetPalette = OceanColors_p; thisinc = 1; thishue = 192; thissat = 255; thisfade = 2; huediff = 255; break; // You can change values here, one at a time , or altogether.
      case  5: targetPalette = LavaColors_p; thisinc = 2; thishue = 128; thisfade = 8; huediff = 64; break;
      case 10: targetPalette = ForestColors_p; thisinc = 1; thishue = random16(255); thisfade = 1; huediff = 16; break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
      case 15: break;                                         // Here's the matching 15 for the other one.
    }
  }

} // ChangeMe()
