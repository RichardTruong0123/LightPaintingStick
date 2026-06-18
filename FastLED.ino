void UpdateBrightness()
{
    FastLED.setBrightness(
        map(brightnessPercent,
            1,
            100,
            1,
            255));
}