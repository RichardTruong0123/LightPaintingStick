#ifndef OVERLAY_INO
#define OVERLAY_INO

typedef struct TMarqueeSegment {
  CRGB* mBase;
  uint16_t mLength;
} TMarqueeSegment;

// Definitions for the "segments" of the LED strip(s) to 
// be treated as separate areas.  They can be in 
// different CRGB arrays, or all in the same one, or any combination.
// Each segment is defined as a starting point in an array, and a length.
#ifndef NUM_LEDS_PER_STRIP
// Sixty-pixel demo, with six segments in it.
TMarqueeSegment gMarqueeSegments[] = {
  { leds +  0 * (NUM_LEDS/6), (NUM_LEDS/6)},
  { leds +  1 * (NUM_LEDS/6), (NUM_LEDS/6)},
  { leds +  2 * (NUM_LEDS/6), (NUM_LEDS/6)},
  { leds +  3 * (NUM_LEDS/6), (NUM_LEDS/6)},
  { leds +  4 * (NUM_LEDS/6), (NUM_LEDS/6)},
  { leds +  5 * (NUM_LEDS/6), (NUM_LEDS/6)}
};
#endif

void Checker(int numSections) {
  static bool Even = true;
//  int numSections = 8;
  int LEDperSection = NUM_LEDS / numSections;

    unsigned long currentMillis = millis();

    if(float(currentMillis - previousMillis) > speedMultiplier(LEDperSection * 20))
    {
       Even = !Even;
       previousMillis = currentMillis;
    }
 
    for (int i = 0; i < NUM_LEDS; i++)
    {
      if ((i/LEDperSection)%2 == Even)   // even row
      {
        leds[i] =  CRGB::Black;          
      } 
    }
}

void Ribbon1()
{
   for (int i = 0; i < 3; i++)
   {
     leds[i] = CRGB::White;
     leds[NUM_LEDS - 1 - i] = CRGB::White;
   }
//     Serial.println("Painted Whitetips");
}

void CheckerOverlay8() {
  Checker(8);
}

void CheckerOverlay12() {
  Checker(12);
}

void CheckerOverlay16() {
  Checker(16);
}

void BlackStripeForRibbon(int location, int width)
{
  if (width/2 > location) return;
   
  int startingPoint = location - width/2;
  for (int i = startingPoint; i < startingPoint + width; i++)
  {
    leds[i] = CRGB::Black; 
  }

   for (int i = startingPoint; i < startingPoint+2; i++)
   {
     leds[i] = CRGB::White;
     leds[i+width - 2] = CRGB::White;
   }   
}

void Ribbon2()
{
   for (int i = 0; i < 3; i++)
   {
     leds[i] = CRGB::White;
     leds[NUM_LEDS - 1 - i] = CRGB::White;
   }
   BlackStripeForRibbon(NUM_LEDS/2, 16);
}

void Ribbon3()
{
   for (int i = 0; i < 3; i++)
   {
     leds[i] = CRGB::White;
     leds[NUM_LEDS - 1 - i] = CRGB::White;
   }
   
   BlackStripeForRibbon(NUM_LEDS/3, 16);
   BlackStripeForRibbon((NUM_LEDS*2)/3, 16);   
}

void Ribbon4()
{
   for (int i = 0; i < 3; i++)
   {
     leds[i] = CRGB::White;
     leds[NUM_LEDS - 1 - i] = CRGB::White;
   }
   
   BlackStripeForRibbon(NUM_LEDS/4, 12);
   BlackStripeForRibbon((NUM_LEDS*2)/4, 12);   
   BlackStripeForRibbon((NUM_LEDS*3)/4, 12);   
}

void BlackStripe(int location, int width)
{
  if (width/2 > location) return;
   
  int startingPoint = location - width/2;
  for (int i = startingPoint; i < startingPoint + width; i++)
  {
    leds[i] = CRGB::Black; 
  } 
}

void Stripe_n(int n)
{
  int numOfLEDs = 12;
   if (n>12) {
      numOfLEDs = NUM_LEDS / n;
   }
     
   for (int i = 1; i < n; i++)
   {
     BlackStripe(NUM_LEDS*i/n, numOfLEDs);
  //   BlackStripe((NUM_LEDS*2)/3, numOfLEDs);   
   }
}

void Stripe2()
{
//   BlackStripe(NUM_LEDS/2, 14);
   Stripe_n(2);
}
void Stripe3()
{
  // BlackStripe(NUM_LEDS/3, 12);
  // BlackStripe((NUM_LEDS*2)/3, 12);   
   Stripe_n(3);
}
void Stripe4()
{
   Stripe_n(4);
}
void Stripe5()
{
   Stripe_n(5);
}
void Stripe8()
{
   Stripe_n(8);
}
void Stripe12()
{
   Stripe_n(12);
}
void Stripe16()
{
   Stripe_n(16);
}

void CheckerWithWhiteBoarderOverlay() {
  static bool Even = true;
  int numSections = 8;
  int LEDperSection = NUM_LEDS / numSections;

    unsigned long currentMillis = millis();

    if(float(currentMillis - previousMillis) > speedMultiplier(LEDperSection * 20))
    {
       Even = !Even;
       previousMillis = currentMillis;
    }
 
    for (int i = 0; i < NUM_LEDS; i++)
    {
      if ((i/LEDperSection)%2 == Even)   // even row
      {
        leds[i] =  CRGB::Black;          
      } 
    }
}

void MarqueeEffectCrawlX( const struct TMarqueeSegment& segment, const CRGB& color)
{
  CRGB* segleds = segment.mBase;
  uint16_t len = segment.mLength;
//  static uint8_t start = 0;
  //start = addmod8( start, 1, 4);
  uint8_t start = millis();
  start = (start >> 5) & 0x03;
  for( uint16_t i = start; i < len; i += 4) {
    segleds[i] = color;
    if( (i + 1) < len) {
      segleds[i+1] = color;
    }
  }
}

void MarqueeEffectCrawlColor( const struct TMarqueeSegment& segment)
{
  MarqueeEffectCrawlX( segment, CHSV( millis() / 16, 200, 255));
}

float speedMultiplier(float value) {
  return value * ((float)100 / speedPercent);
}
#endif