  // CRGB color;

void SetSolidColor(const char *colorName) {

  Serial.printf("SetSolidColor: %s\n",
                colorName);

#ifndef DEBUG

  color = CRGB::Black;

  // =====================================
  // COLOR LOOKUP
  // =====================================

  if (strcmp(colorName, "White") == 0) {
    color = CRGB::White;
  }
  else if (strcmp(colorName, "Red") == 0) {
    color = CRGB::Red;
  }
  else if (strcmp(colorName, "Green") == 0) {
    color = CRGB::Green;
  }
  else if (strcmp(colorName, "Blue") == 0) {
    color = CRGB::Blue;
  }
  else if (strcmp(colorName, "Yellow") == 0) {
    color = CRGB::Yellow;
  }
  else if (strcmp(colorName, "Cyan") == 0) {
    color = CRGB::Cyan;
  }
  else if (strcmp(colorName, "Magenta") == 0) {
    color = CRGB::Magenta;
  }
  else if (strcmp(colorName, "Orange") == 0) {
    color = CRGB::Orange;
  }

  fill_solid(leds, NUM_LEDS, color);

  selectedPattern = PATTERN_SOLID_COLORS;

  selectedSolidColor = colorName;

#endif

  lv_label_set_text_fmt(status_label,
                        "Color: %s",
                        colorName);
}

void SetColorPalette(const char *paletteName)
{
    Serial.printf("Color Palette: %s\n", paletteName);

   // selectedPalette = String(paletteName);

    if (paletteName == "Rainbow") {
      Pattern.foreground_fp = Rainbow1;
    }
    if (paletteName == "Rainbow Stripes") {
      Pattern.foreground_fp = Rainbow1;
    }

    if (paletteName == "Rainbow Pride") {
      Pattern.foreground_fp = Rainbow1;
    }

    if (paletteName == "RB Gradient") {
      Pattern.foreground_fp = Rainbow2;
    }
    if (paletteName == "RB Vertical") {
      Pattern.foreground_fp = Rainbow3;
    }

    if (paletteName == "RB Diagonal") {
      Pattern.foreground_fp = Rainbow4;
    }

    selectedPattern = PATTERN_COLOR_PALETTE;

    lv_label_set_text_fmt(status_label,
                          "Palette: %s",
                          paletteName);
}

void SetOverlay(const char* overlayName)
{
  Serial.printf("Overlay: %s\n", overlayName);

  // Store selected overlay
  selectedOverlay = String(overlayName);

  if (overlayName == "No Overlay") {
    Pattern.overlay_fp = Blank;
  }
  if (overlayName == "Checker 8") {
    Pattern.overlay_fp = CheckerOverlay8;
  }
  if (overlayName == "Checker 12") {
    Pattern.overlay_fp = CheckerOverlay12;
  }
  if (overlayName == "Checker 16") {
    Pattern.overlay_fp = CheckerOverlay16;
  }
  if (overlayName == "Ribbon 1") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Ribbon1;
  }
  if (overlayName == "Ribbon 2") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Ribbon2;
  }
  if (overlayName == "Ribbon 3") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Ribbon3;
  }
  if (overlayName == "Stripe 2") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Stripe2;
  }
  if (overlayName == "Stripe 3") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Stripe3;
  }

  if (overlayName == "Stripe 4") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Stripe4;
  }
  if (overlayName == "Stripe 5") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Stripe5;
  }
  if (overlayName == "Stripe 8") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Stripe8;
  }
  if (overlayName == "Stripe 12") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Stripe12;
  }
  if (overlayName == "Stripe 16") {
    //Pattern.overlay_fp = WhiteTips;
    Pattern.overlay_fp = Stripe16;
  }


    lv_label_set_text_fmt(status_label,
                          "Overlay: %s",
                          overlayName);
}

void SetColorTemperature(const char *temperatureName)
{
    if (strcmp(temperatureName, "No Correction") == 0)
        _colorTemperature = UncorrectedTemperature;

    else if (strcmp(temperatureName, "Candle") == 0)
        _colorTemperature = Candle;

    else if (strcmp(temperatureName, "Tungsten 40W") == 0)
        _colorTemperature = Tungsten40W;

    else if (strcmp(temperatureName, "Tungsten 100W") == 0)
        _colorTemperature = Tungsten100W;

    else if (strcmp(temperatureName, "Halogen") == 0)
        _colorTemperature = Halogen;

    else if (strcmp(temperatureName, "Carbon Arc") == 0)
        _colorTemperature = CarbonArc;

    else if (strcmp(temperatureName, "High Noon Sun") == 0)
        _colorTemperature = HighNoonSun;

    else if (strcmp(temperatureName, "Direct Sunlight") == 0)
        _colorTemperature = DirectSunlight;

    else if (strcmp(temperatureName, "Overcast Sky") == 0)
        _colorTemperature = OvercastSky;

    else if (strcmp(temperatureName, "Clear Blue Sky") == 0)
        _colorTemperature = ClearBlueSky;

    else if (strcmp(temperatureName, "Warm Fluorescent") == 0)
        _colorTemperature = WarmFluorescent;

    else if (strcmp(temperatureName, "Standard FLUOR") == 0)
        _colorTemperature = StandardFluorescent;

    else if (strcmp(temperatureName, "Cool White FLUOR") == 0)
        _colorTemperature = CoolWhiteFluorescent;

    else if (strcmp(temperatureName, "Full Spect FLUOR") == 0)
        _colorTemperature = FullSpectrumFluorescent;

    else if (strcmp(temperatureName, "Grow Light FLUOR") == 0)
        _colorTemperature = GrowLightFluorescent;

    FastLED.setTemperature(_colorTemperature);

    Serial.printf("Color Temperature: %s\n",
                  temperatureName);

    lv_label_set_text_fmt(
        status_label,
        "Temp: %s",
        temperatureName);
}
