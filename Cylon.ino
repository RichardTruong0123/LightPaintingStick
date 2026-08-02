void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

int cylonPos = 0;
int cylonDir = 1;
uint8_t cylonHue = 0;

void Cylon(){
	static uint8_t hue = 0;
	Serial.print("x");
	// First slide the led in one direction
  while(firing)  // (digitalRead(triggerButton) == LOW)
  {
		for(int i = 0; i < NUM_LEDS; i++) {
			// Set the i'th led to red 
			leds[i] = CHSV(hue++, 255, 255);
			// Show the leds
			FastLED.show(); 
			// now that we've shown the leds, reset the i'th led to black
			// leds[i] = CRGB::Black;
			fadeall();
			// Wait a little bit before we loop around and do it again
			delay(speedMultiplier(10));
								Check();  
		}
		Serial.print("x");

		// Now go in the other direction.  
		for(int i = (NUM_LEDS)-1; i >= 0; i--) {
			// Set the i'th led to red 
			leds[i] = CHSV(hue++, 255, 255);
			// Show the leds
			FastLED.show();
			// now that we've shown the leds, reset the i'th led to black
			// leds[i] = CRGB::Black;
			fadeall();
			// Wait a little bit before we loop around and do it again
			delay(speedMultiplier(10));
									Check();  
		}     
  }

}


	void cylonSetup()
	{
			cylonPos = 0;
			cylonDir = 1;
			cylonHue = 0;

			fill_solid(leds, NUM_LEDS, CRGB::Black);
	}
	void cylonStep()
	{
			fadeall();

			leds[cylonPos] = CHSV(cylonHue++,255,255);

			FastLED.show();

			EVERY_N_MILLISECONDS(speedMultiplier(10))
			{
					cylonPos += cylonDir;

					if(cylonPos >= NUM_LEDS-1)
							cylonDir = -1;

					if(cylonPos <= 0)
							cylonDir = 1;
			}
	}	
