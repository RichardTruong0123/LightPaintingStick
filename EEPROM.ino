enum EEPROMAddress : uint8_t {
  EEPROM_SPEED_PERCENT      = 1,
  EEPROM_BRIGHTNESS_PERCENT = 2,
  EEPROM_COUNTDOWN_INDEX    = 3,
  EEPROM_DIRECTION_INDEX    = 4,
  EEPROM_FLIP_VERTICAL      = 5,
  EEPROM_AUTO_INCREMENT     = 6,
  EEPROM_REPEAT_INDEX       = 7,
  EEPROM_NUMLED_INDEX       = 8
};

void EEPROM_read() {
  EEPROM.begin(EEPROM_SIZE);

  speedPercent = EEPROM.read(EEPROM_SPEED_PERCENT);
  if (speedPercent < 50 || speedPercent > 200)
    speedPercent = 80;

  brightnessPercent = EEPROM.read(EEPROM_BRIGHTNESS_PERCENT);
  if (brightnessPercent < 5 || brightnessPercent > 100)
    brightnessPercent = 80;

  countdownIndex = EEPROM.read(EEPROM_COUNTDOWN_INDEX);
  if (countdownIndex > 10)
    countdownIndex = 0;

  directionIndex = EEPROM.read(EEPROM_DIRECTION_INDEX);
  if (directionIndex > 1)
    directionIndex = 0;   // 0 = L->R, 1 = R->L

  autoIncrEnabled = EEPROM.read(EEPROM_AUTO_INCREMENT);
  if (autoIncrEnabled != 1)
    autoIncrEnabled = 0;

  flipVerticalEnabled = EEPROM.read(EEPROM_FLIP_VERTICAL);
  if (flipVerticalEnabled != 1)
    flipVerticalEnabled = 0;

  repeatIndex = EEPROM.read(EEPROM_REPEAT_INDEX);
    if (repeatIndex < 0 || repeatIndex > 10) repeatIndex = 0;  //default

  numLedsIndex = EEPROM.read(EEPROM_NUMLED_INDEX);
    if (numLedsIndex < 0 || numLedsIndex > 10) numLedsIndex = 0;  //default
    NUM_LEDS = numLedValues[numLedsIndex];
Serial.printf("EEPROM Read NUM_LEDS = %lu \n", NUM_LEDS);

  // Serial.print("flip Vertical: ");
  // Serial.println(flipVerticalEnabled);
}
