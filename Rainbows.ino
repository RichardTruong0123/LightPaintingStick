#ifndef RAINBOW_H
#define RAINBOW_H

void Rainbow1() {

  uint16_t i, j;
  int LEDperSection = NUM_LEDS / 6;
//  while (firing)  // digitalRead(triggerButton) == LOW)
//  {
    for(i=0; i<NUM_LEDS; i++) {
      switch((i/LEDperSection) % 7){
        case 0:
          //strip.setPixelColor(i, strip.Color(255,0,0));
          leds[i] = CRGB(255,0,0);
          break;
        case 1:
          //strip.setPixelColor(i, strip.Color(255,127,0));
          leds[i] = CRGB(255,127,0);
          break;
        case 2:
          //strip.setPixelColor(i, strip.Color(255,255,0));
          leds[i] = CRGB(255,255,0);
          break;          
        case 3:
          //strip.setPixelColor(i, strip.Color(0,255,0));
          leds[i] = CRGB(0,255,0);
          break;
        case 4:
          //strip.setPixelColor(i, strip.Color(0,0,255));
          leds[i] = CRGB(0,0,255);
          break;
        case 5:
          //strip.setPixelColor(i, strip.Color(75,0,130));
          leds[i] = CRGB(75,0,130);
          break;
        case 6:
          //strip.setPixelColor(i, strip.Color(143,0,255));
          leds[i] = CRGB(143,0,255);
         break;
      }
      //strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    
//    strip.show();
//      FastLED.show();
      Serial.println("Painted Rainbow1");
/*
  while (firing)  // digitalRead(triggerButton) == LOW)
  {    
    Check();
    delay(50);
  }
  */
}

void Rainbow2() {
  
  //Horizontal rainbow - smooth transition
   #ifdef DEBUG
      Serial.println ("Rainbow Pattern 2");
   #endif     
  uint16_t i, j;

  fill_rainbow( leds, NUM_LEDS, 0, 5);
//  FastLED.show();
  
  /*
  while (firing)  // digitalRead(triggerButton) == LOW)
  {    
    Check();
    delay(50);
  }
  Serial.println("exit");
  */
}



//void Rainbow3(uint8_t wait) {
void Rainbow3() {
 // strip.setBrightness(map(brightnessPercent,1,100,1,255));
//  while(firing)  //  (digitalRead(triggerButton) == LOW)
//  {  
     // ChangePalettePeriodically();
    uint8_t brightness = 255;      
      static uint8_t colorIndex = 0;
      colorIndex += 1; /* motion speed */
      
      for( int i = 0; i < NUM_LEDS; i++) {
          leds[i] = ColorFromPalette( RainbowColors_p, colorIndex, brightness, LINEARBLEND);
         // colorIndex += 3;
      }
    
//      FastLED.show();
//      FastLED.delay(speedMultiplier(1000 / 100));
//      Check();      
//  }
}


#define UPDATES_PER_SECOND 100

//void Rainbow4(uint8_t wait) {
void Rainbow4() {
  unsigned long currentMillis = millis();
  static uint8_t startIndex = 0;
//  while(firing)  //  (digitalRead(triggerButton) == LOW)
//  {  
    currentPalette = RainbowColors_p;         
    currentBlending = LINEARBLEND;
    if(float(currentMillis - previousMillis2) > 1000 / UPDATES_PER_SECOND)
    {
      startIndex = startIndex + 1; /* motion speed */
      
      
      FillLEDsFromPaletteColors( startIndex);
       previousMillis2 = currentMillis;
    }

      
//      FastLED.show();
//      FastLED.delay(speedMultiplier(1000 / UPDATES_PER_SECOND));
//       Check();      
//  }
}

/*
void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
   // currentPalette = RainbowColors_p
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( RainbowColors_p, colorIndex, brightness, LINEARBLEND);
        colorIndex += 3;
    }
}
*/

#endif