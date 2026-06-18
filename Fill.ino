#ifndef FILL_H
#define FILL_H


void Check()
{

  
  // TOUCHINFO ti;

  // bool touched  = touch.getSamples(&ti);
  // if (touched) {
  //     firing = false;
  //     demo = false;
  //     Serial.println("TEST turn off button");
  // }
}

void Fill() {
  int numSections = 8;
  int LEDperSection = NUM_LEDS / numSections;
  unsigned long currentMillis;
  
  while (firing)  // digitalRead(triggerButton) == LOW)
  {   
    fill_solid( leds, NUM_LEDS, color);
//    Serial.println ("Paint foreground");
  Serial.println("before foreground");
    Pattern.foreground_fp();
//    Serial.println ("Paint overlay");    
  Serial.println("before overlay");
    Pattern.overlay_fp();
 //   OverlayFunctionPointer();

   #ifndef DISABLE_FASTLED
     FastLED.show();
   #endif
   
    Check(); //check to see if a stop is requested.
    delay(speedMultiplier(50)); 

/*
currentMillis = millis();
    while (float(currentMillis - previousMillis) < speedMultiplier(LEDperSection * 50)) {
      Serial.print("In Delay: "); Serial.println(currentMillis);
      currentMillis = millis();
      Check();
      delay(50); 
    }
  */
  }

Serial.println ("exit Fill");  
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void FillLEDsFromPaletteColors()
{
    static uint8_t startIndex = 9;
//    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors( startIndex);
}

void RunSolidColor() {
  fill_solid( leds, NUM_LEDS, color);
  Pattern.overlay_fp();

   #ifndef DISABLE_FASTLED
     FastLED.show();
   #endif
   
}

void RunColorPalette() {
  Pattern.foreground_fp();
  Pattern.overlay_fp();

   #ifndef DISABLE_FASTLED
     FastLED.show();
   #endif
   
}

void RunFilePattern() {
  bmp_paint2(bmp_filename);
}


void runPatternTask()
{
    static uint32_t lastFrame = 0;

    uint32_t now = millis();

    // 25% => 200ms
    // 100% => 10ms

    uint32_t frameInterval =
        map(speedPercent,
            25,
            100,
            200,
            10);

    if (now - lastFrame < frameInterval)
        return;

    lastFrame = now;
    
    switch(selectedPattern)
    {
        case SelectedPattern::PATTERN_SOLID_COLORS:
            RunSolidColor();
            break;

        case SelectedPattern::PATTERN_COLOR_PALETTE:
            RunColorPalette();
            break;

        case SelectedPattern::PATTERN_CONFETTI:
           // RunConfetti();
            break;

        case SelectedPattern::PATTERN_DEMO:
           // RunDemo();
            break;

        case SelectedPattern::PATTERN_FILE:
            RunFilePattern();
            currentMode = MODE_MENU;

            stop_leds();

            // ledsRunning = false;

            // lv_obj_add_flag(run_screen,
            //                 LV_OBJ_FLAG_HIDDEN);

            // draw_menu(currentMenu,
            //           currentMenuSize,
            //           "Light Painting Stick");
            break;

        default:
            break;
    }

    FastLED.show();
}

#endif