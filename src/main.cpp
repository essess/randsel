#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXEL_PIN 13
#define BUTTON_PIN   10

#define SLOT1   9
#define SLOT2   8
#define SLOT3   7
#define SLOT4   6
#define SLOT5   5
#define SLOT6   4
#define SLOT7   3
#define SLOT8   2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

/** gud stuf here: **/

int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive.
 */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

typedef enum { IDLE, CHECKING, RANDING } state_t;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SLOT1, INPUT_PULLUP);
  pinMode(SLOT2, INPUT_PULLUP);
  pinMode(SLOT3, INPUT_PULLUP);  /**< BROKE */
  pinMode(SLOT4, INPUT_PULLUP);
  pinMode(SLOT5, INPUT_PULLUP);
  pinMode(SLOT6, INPUT_PULLUP);
  pinMode(SLOT7, INPUT_PULLUP);
  pinMode(SLOT8, INPUT_PULLUP);

  strip.setBrightness(64);
  strip.begin();
  strip.show();
}

void loop() {
  int const button = digitalRead(BUTTON_PIN);
  int rnum = rand_lim(7); /* constantly run generator while idle or checking */
  static uint8_t slots = 0;
  static state_t state = IDLE;

  switch( state )
  {
    case RANDING:
      /* do light show and show selection */
      theaterChaseRainbow(2);

      do {
        rnum = rand_lim(7);
      } while( !(slots & (1<<rnum)) );

      strip.clear();
      strip.setPixelColor(rnum, 127,0,127 );
      strip.show();

      while(digitalRead(BUTTON_PIN)==LOW) {
        delay(10);  /* block until button pressed again */
      }
      state = CHECKING;
      break;
    case CHECKING:
      /* go through each mailbox and show detect status on it's led */
      slots = 0;
      for(int i=0; i<=7 ;i++)
      {
        delay(10);
        if( digitalRead(SLOT1-i) == HIGH )
          slots |= (1<<i);
      }
      slots &= ~(1<<2);   /**< slot3 broke :( */

      strip.clear();
      for(int i=0; i<=7 ;i++)
      {
        if( slots & (1<<i) )
          strip.setPixelColor(i, 0,255,127 );
      }
      strip.show();

      if( button == LOW )
        state = slots? RANDING : IDLE;

      break;
    case IDLE:
    default:
      rainbowCycle(1);
      state = (button == HIGH)? CHECKING : IDLE;
  }
}