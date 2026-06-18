void processLights() {

  //  strip.setBrightness(map(brightnessPercent,1,100,1,255));
  LEDS.setBrightness(map(brightnessPercent, 1, 100, 1, 255));
  LEDS.setTemperature(_colorTemperature);
  //  Serial.print("Color Temperature: "); Serial.println(_colorTemperature);

  //read the pushbutton value into a variable
  //  while (digitalRead(triggerButton) == LOW)
  while (firing) {
   Serial.println("In processLights()");
   //  #ifdef DEBUG
    Serial.println("Firing Lights");
    Serial.println(selectedPattern);
    //   #endif
    switch (selectedPattern) {
      case 0:; break;
      case 1: Fill(); break;
      case 2: Fill(); break;  //Rainbow2();break;
      case 3: Fill(); break;
      case 4: Fill(); break;   //Rainbow4();break;
      case 10: Fill(); break;  //CheckerBoard(CRGB::White, CRGB::Black, 50, false);break;
      case 11: Fill(); break;  //CheckerBoard(CRGB::Red, CRGB::Black, 50, false);break;
      case 12: Fill(); break;  //CheckerBoard(CRGB::Green, CRGB::Black, 50, false);break;
      case 13:
        Fill();
        break;  //CheckerBoard(CRGB::Blue, CRGB::Black, 50, false);break;
                //     case 20: Serial.println ("SoftTwinkles");SoftTwinkles(20);break;
                //      case 21: Serial.println ("StarTrails");StarTrails(20);break;
      // case 30:
      //   Serial.println("Cylon Pattern");
      //   Cylon();
      //   break;
      //   //  case 31: BouncingBallsSetup(); BouncingBalls(); break;
      // case 32:
      //   Serial.println("Confetti pattern");
      //   confettiSetup();
      //   confettiLoop();
      //   break;
      // case 33:
      //   RWBGlitter();
      //   break;
      //   //    case 34: juggle_setup(); juggle_loop();break;
      //  case 99: demoRun(); break;

      case 100:
        //bmp_paint(bmp_filename);
        break;

    }  // end of switch

    // turnOFFAll();
    // DrawMenu();
#ifdef DEBUG
    Serial.println("Process Lights DONE");
#endif
  }
}