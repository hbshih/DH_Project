//Code includes samples written by Mark Kriegsman and Andrew Tuline.  Thanks so much for all your help guys!

#include <FastLED.h>

//---LED SETUP STUFF
#define LED_PIN 4
#define CLOCK_PIN 5
#define BUTTON_PIN 5
#define COLOR_ORDER GBR
#define NUM_LEDS 288      // Change to reflect the number of LEDs you have
int vib_pin = A0;

CRGB leds[NUM_LEDS];      //naming our LED array

//BUTTON SETUP STUFF
byte prevKeyState = HIGH;
int ledMode = 2;  //FIRST ACTIVE MODE

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
// MODE VARIABLES -- Change these numbers to create new interesting modes

int BRIGHTNESS = 255;    //0-255.  Lower number saves battery life, higher number is screamingly bright
int SATURATION = 255;    //0 is white (no color) and 255 is fully saturated with color
int HUE = 0;             //0-255, around the color wheel
int STEPS = 4;           //Wider or narrower bands of color
int SPEEDO = 10;         //The speed of the animation

// have 3 independent CRGBs - 2 for the sources and one for the output
/*CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];*/
int jumpCount = 0;

/* Lightning Variables */
// Global variables can be changed on the fly.
uint8_t max_bright= 255;                                      // Overall brightness definition. It can be changed on the fly.
uint8_t frequency = 50;                                       // controls the interval between strikes
uint8_t flashes = 8;                                          //the upper limit of flashes per strike
unsigned int dimmer = 1;

uint8_t ledstart;                                             // Starting location of a flash
uint8_t ledlen;                                               // Length of a flash

/*Rain*/

// Define variables used by the sequences.
uint8_t  thisfade = 8;                                        // How quickly does it fade? Lower = slower fade rate.

/* Sun */
int r = 255;  // Redness starts at zero and goes up to full
int b;  // Blue starts at full and goes down to zero
int g = 0;              // No green needed to go from blue to red
int goTowardValue = 0;
bool goToward = true;

// -- DEFINE WEATHER MODE

int WeatherMode = 1;

//------------------SETUP------------------
void setup()
{
  delay( 2000 ); // power-up safety delay
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  FastLED.addLeds<DOTSTAR, LED_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  FastLED.clear();
  //currentBlending = LINEARBLEND;
  //currentPalette = RainbowColors_p;
  pinMode(vib_pin, INPUT);
  Serial.begin(9600);
}

void loop() {
  int val;
  val = analogRead(vib_pin);
  Serial.print("\n");
  Serial.print(val);
  Serial.print("\n");
  

  if (val < 30)
  {
    switch (WeatherMode) {
      case 3:
        sun();
        Serial.println("P");
        break;
      case 2:
        lightning();
        Serial.println("T");
        break;
      case 4:
        rain();
        Serial.println("R");
        break;
      case 1:
        rainbow_march(100, 5);
        Serial.println("P");
        break;
      default:
        // statements
        break;
    }
  }
}

void lightning()
{
  ledstart = random(NUM_LEDS);                               // Determine starting location of flash
  ledlen = random(NUM_LEDS - ledstart);                      // Determine length of flash (not to go beyond NUM_LEDS-1)

  Serial.println(ledstart);
  Serial.println(ledlen);

  fill_solid(leds + ledstart, ledlen, CHSV(255, 0, 255 / dimmer));
  FastLED.show();                       // Show a section of LED's
  delay(random8(4, 10));                                    // each flash only lasts 4-10 milliseconds
  fill_solid(leds + ledstart, ledlen, CHSV(255, 0, 0));     // Clear the section of LED's
  FastLED.show();
}

void rain()
{
  // chasing: Blue
  fadeToBlackBy(leds, NUM_LEDS, thisfade);                    // Low values = slower fade.
  int pos = random16(NUM_LEDS);                               // Pick an LED at random.
  //leds[pos] = CHSV((thishue + random16(huediff)) / 4 , thissat, thisbri); // I use 12 bits for hue so that the hue increment isn't too quick.
  leds[pos] = CHSV(120, 200, 180); // I use 12 bits for hue so that the hue increment isn't too quick.
  FastLED.show();
}

void rainbow_march(uint8_t thisdelay, uint8_t deltahue) {     // The fill_rainbow call doesn't support brightness levels.

  for (int k = 0; k < NUM_LEDS; k++) {
    uint8_t thishue = millis() * (255 - thisdelay) / 255;       // To change the rate, add a beat or something to the result. 'thisdelay' must be a fixed value.

    // thishue = beat8(50);                                       // This uses a FastLED sawtooth generator. Again, the '50' should not change on the fly.
    // thishue = beatsin8(50,0,255);                              // This can change speeds on the fly. You can also add these to each other.

    fill_rainbow(leds, NUM_LEDS, thishue, deltahue);            // Use FastLED's fill_rainbow routine.
  }
  FastLED.show();
}

void sun()
{
  if (goToward)
  {
    goTowardValue++;

    if (goTowardValue >= 200)
    {
      goToward = false;
    }
  }

  if (!goToward)
  {
    goTowardValue--;
    if (goTowardValue <= 0)
    {
      goToward = true;
    }
  }

  b = goTowardValue;

  // Now loop though each of the LEDs and set each one to the current color
  for (int x = 0; x < NUM_LEDS; x++) {
    leds[x] = CRGB(r, g, b);
  }

  // Display the colors we just set on the actual LEDs
  FastLED.show();

  delay(10);

}



// SOLID COLOR -------------------------------------
void Solid()
{
  fill_solid(leds, NUM_LEDS, CHSV(HUE, SATURATION, BRIGHTNESS));
  FastLED.show();
}
