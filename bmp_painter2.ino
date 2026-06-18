CRGB* bmpFrameBuffer = nullptr;

uint32_t bmpWidth = 0;
uint32_t bmpHeight = 0;
uint32_t totalFrames = 0;

bool bmpLoaded = false;

void freeBmpFrameBuffer()
{
    if (bmpFrameBuffer)
    {
        free(bmpFrameBuffer);
        bmpFrameBuffer = nullptr;
    }

    bmpLoaded = false;
}

bool loadBmpFrameBuffer(String filename)
{
    freeBmpFrameBuffer();

    if (!openFrame(filename))
    {
        Serial.println("Failed to open BMP");
        return false;
    }

    // Read BMP dimensions
    bmpFile.seek(18);

    int32_t fileWidth;
    int32_t fileHeight;

    bmpFile.read((uint8_t*)&fileHeight, 4);
    bmpFile.read((uint8_t*)&fileWidth, 4);

    uint32_t sourceHeight = abs(fileHeight);

    bmpWidth = fileWidth;

    // Only use as many rows as we can display
    bmpHeight = min(sourceHeight, (uint32_t)NUM_LEDS);

    totalFrames = bmpWidth;

    Serial.printf(
        "BMP Width=%lu Height=%lu  Using=%lu LEDs\n",
        bmpWidth,
        sourceHeight,
        bmpHeight);

    size_t frameBufferSize =
        bmpWidth *
        bmpHeight *
        sizeof(CRGB);

    bmpFrameBuffer =
        (CRGB*)ps_malloc(frameBufferSize);

    if (!bmpFrameBuffer)
    {
        Serial.printf(
            "Failed to allocate %u bytes\n",
            frameBufferSize);

        bmpFile.close();
        return false;
    }

    // Move to pixel data
    bmpFile.seek(54);

    uint8_t rgb[3];

    for (uint32_t x = 0; x < bmpWidth; x++)
    {
        for (uint32_t y = 0; y < sourceHeight; y++)
        {
            if (bmpFile.read(rgb, 3) != 3)
            {
                Serial.println("Unexpected EOF");

                freeBmpFrameBuffer();
                bmpFile.close();

                return false;
            }

            // Only keep rows that fit on the strip
            if (y < bmpHeight)
            {
                bmpFrameBuffer[(x * bmpHeight) + y] =
                    CRGB(
                        gammaTable[rgb[2]],
                        gammaTable[rgb[1]],
                        gammaTable[rgb[0]]);
            }
        }
    }

    bmpFile.close();

    bmpLoaded = true;

    Serial.printf(
        "Framebuffer loaded (%u bytes)\n",
        frameBufferSize);

    return true;
}

void showBmpFrame(uint32_t frame)
{
    if (!bmpLoaded)
        return;

    if (frame >= totalFrames)
        return;

    // Clear all LEDs first
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    CRGB* dst = &leds[NUM_LEDS - 1];
    CRGB* src = &bmpFrameBuffer[frame * bmpHeight];

    if (flipVerticalEnabled == false)
    {
        memcpy(leds, src, bmpHeight * sizeof(CRGB));
    }
    else
    {
        CRGB* dst = &leds[NUM_LEDS - 1];

        for (uint32_t i = 0; i < bmpHeight; i++)
        {
            *dst-- = *src++;
        }
    }
    FastLED.show();
}

/* =========================================================
   TOUCH READ
   ========================================================= */

bool readTouch() {

  uint8_t fingers = readRegister(CST816D_FINGER_NUM);

  if (fingers == 0) {
    return false;
  }
  firing = false;
  return true;
}

void bmp_paint2(String filename)
{
    //This is loaded when file explorer selected the file event.
    // if (!loadBmpFrameBuffer(filename))
    //     return;

    LEDS.setBrightness(
        map(
            brightnessPercent,
            1,
            100,
            1,
            255));

    uint32_t frameInterval =
        map(
            speedPercent,
            25,
            100,
            50,
            1);

    firing = true;
    bool forever = (repeatIndex == 9);

    int32_t startFrame = (directionIndex == 0)
                             ? 0   // left to right
                             : totalFrames - 1;  //right to left

    int32_t endFrame = (directionIndex == 0)
                           ? totalFrames
                           : -1;

    int32_t step = (directionIndex == 0)
                       ? 1
                       : -1;

    if (forever)
    {
        while (firing)
        {
            for (int32_t frame = startFrame;
                frame != endFrame;
                frame += step)
            {
                showBmpFrame(frame);
                delay(frameInterval);
                
                if (readTouch())
                    break;
            }
        }
    }
    else
    {
        for (uint32_t repeatCount = 0;
            repeatCount < repeatIndex + 1;
            repeatCount++)
        {
            Serial.printf("Repeat Count: %d", repeatCount);

            for (int32_t frame = startFrame;
                frame != endFrame;
                frame += step)
            {
                showBmpFrame(frame);
                delay(frameInterval);

                if (readTouch())
                    break;
            }
        }
        firing = false;

        Serial.println("BMP playback complete");
    }
}