// Bodmer's BMP image rendering function

static lv_img_dsc_t bmp_dsc;
static uint16_t bmp_buffer[240 * 320]; // adjust if needed

void drawBmp(const char *filename, int16_t x, int16_t y) {

  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFS;

  // Open requested file on SD card
//  bmpFS = SPIFFS.open(filename, "r");
  bmpFS = SD.open(filename, FILE_READ);

  if (!bmpFS)
  {
    Serial.print("File not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16-bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

////////////////////////////////
// void loadBmpToLVGL(const char *filename)
// {
//   File bmpFS = SD.open(filename);

//   if (!bmpFS)
//     return;

//   if (read16(bmpFS) != 0x4D42)
//     return;

//   read32(bmpFS);
//   read32(bmpFS);

//   uint32_t offset = read32(bmpFS);
//   read32(bmpFS);

//   int w = read32(bmpFS);
//   int h = read32(bmpFS);

//   if (read16(bmpFS) != 1 || read16(bmpFS) != 24)
//     return;

//   bmpFS.seek(offset);

//   uint16_t padding = (4 - ((w * 3) & 3)) & 3;
//   uint8_t line[w * 3 + padding];

//   int index = 0;

//   for (int row = 0; row < h; row++)
//   {
//     bmpFS.read(line, sizeof(line));

//     uint8_t *p = line;

//     for (int col = 0; col < w; col++)
//     {
//       uint8_t b = *p++;
//       uint8_t g = *p++;
//       uint8_t r = *p++;

//       bmp_buffer[index++] =
//           ((r & 0xF8) << 8) |
//           ((g & 0xFC) << 3) |
//           (b >> 3);
//     }
//   }

//   bmpFS.close();

//   bmp_dsc.header.cf = LV_COLOR_FORMAT_NATIVE;
//   bmp_dsc.header.w = w;
//   bmp_dsc.header.h = h;
//   bmp_dsc.data_size = w * h * 2;
//   bmp_dsc.data = (const uint8_t *)bmp_buffer;

//   lv_image_set_src(bmp_img, &bmp_dsc);
// }


// void create_bmp_screen()
// {
//   bmp_screen = lv_obj_create(NULL); // NEW SCREEN

//   lv_obj_set_style_bg_color(
//       bmp_screen,
//       lv_color_black(),
//       0);

//   lv_obj_set_style_bg_opa(
//       bmp_screen,
//       LV_OPA_COVER,
//       0);

//   bmp_img = lv_image_create(bmp_screen);

//   lv_obj_center(bmp_img);

//   // Tap anywhere to exit preview
//   lv_obj_add_event_cb(
//       bmp_screen,
//       bmp_screen_event,
//       LV_EVENT_CLICKED,
//       NULL);
// }

// void bmp_screen_event(lv_event_t *e)
// {
//   // currentScreen = SCREEN_BROWSER;

//   // lv_scr_load(file_browser_screen);
// }