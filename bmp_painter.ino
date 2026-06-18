/*
   ESP32 NeoPixel Light Painter
   Reads 24-bit BMP images from SD card
   Streams columns to a NeoPixel strip.

   Board: ESP32
*/

#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <FastLED.h>

#define LED_PIN     2
// #define TRIGGER     4
// #define POT_PIN     34


#define SD_CS 41
#define TFT_CS   45
int sck = 39;
int miso = 40;
int mosi = 38;
int cs = 41;


//#define NUM_LEDS      200
#define BUFFER_SIZE 200*3   // minimun of 3*NUM_LEDS

// Adafruit_NeoPixel strip(N_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

//CRGB leds[NUM_LEDS];

File bmpFile;

uint8_t sdBuf[BUFFER_SIZE];
//uint8_t brightness = 200;

uint16_t nFrames = 0;
uint16_t currentFrame = 0;

uint16_t linesPerSec = 700;

static const uint8_t PROGMEM
  dither[16][16] = { // 16x16 ordered dither array
    {   0,128, 32,160,  8,136, 40,168,  2,130, 34,162, 10,138, 42,170 },
    { 192, 64,224, 96,200, 72,232,104,194, 66,226, 98,202, 74,234,106 },
    {  48,176, 16,144, 56,184, 24,152, 50,178, 18,146, 58,186, 26,154 },
    { 240,112,208, 80,248,120,216, 88,242,114,210, 82,250,122,218, 90 },
    {  12,140, 44,172,  4,132, 36,164, 14,142, 46,174,  6,134, 38,166 },
    { 204, 76,236,108,196, 68,228,100,206, 78,238,110,198, 70,230,102 },
    {  60,188, 28,156, 52,180, 20,148, 62,190, 30,158, 54,182, 22,150 },
    { 252,124,220, 92,244,116,212, 84,254,126,222, 94,246,118,214, 86 },
    {   3,131, 35,163, 11,139, 43,171,  1,129, 33,161,  9,137, 41,169 },
    { 195, 67,227, 99,203, 75,235,107,193, 65,225, 97,201, 73,233,105 },
    {  51,179, 19,147, 59,187, 27,155, 49,177, 17,145, 57,185, 25,153 },
    { 243,115,211, 83,251,123,219, 91,241,113,209, 81,249,121,217, 89 },
    {  15,143, 47,175,  7,135, 39,167, 13,141, 45,173,  5,133, 37,165 },
    { 207, 79,239,111,199, 71,231,103,205, 77,237,109,197, 69,229,101 },
    {  63,191, 31,159, 55,183, 23,151, 61,189, 29,157, 53,181, 21,149 },
    { 255,127,223, 95,247,119,215, 87,253,125,221, 93,245,117,213, 85 } },
  gammaTable[256] = { // Brightness ramp for LEDs
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,
      2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,
      6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10, 10, 11,
     11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18,
     18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27,
     28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 36, 36, 37, 38, 39,
     40, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53,
     54, 55, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71,
     72, 74, 75, 76, 77, 79, 80, 81, 83, 84, 85, 87, 88, 89, 91, 92,
     94, 95, 97, 98, 99,101,103,104,106,107,109,110,112,113,115,117,
    118,120,122,123,125,127,129,130,132,134,136,138,139,141,143,145,
    147,149,151,153,155,157,159,161,163,165,167,169,171,173,175,177,
    179,182,184,186,188,190,193,195,197,200,202,204,207,209,211,214,
    216,219,221,223,226,228,231,234,236,239,241,244,247,249,252,255 },
  bump[] = { // Brightnes++ dither probabilities
      0,  1,  2,  3,  5,  7,  9, 11, 14, 17, 20, 24, 29, 34, 40, 46,
     53, 60, 69, 78, 87, 98,109,121,134,148,163,178,195,213,231,251,
     16, 38, 61, 85,110,136,164,192,222,253, 31, 64,100,136,174,213,
    253, 40, 84,128,175,223, 17, 68,120,174,230, 32, 91,152,214, 23,
     89,156,225, 41,114,189, 10, 89,169,251, 80,166,253, 88,180, 19,
    114,212, 57,159,  8,114,222, 78,190, 50,167, 31,152, 21,147, 20,
    150, 28,163, 45,185, 72,217,109,  3,155, 54,211,115, 22,186, 98,
     12,184,103, 25,205,132, 62,250,185,123, 64,  7,209,158,109, 64,
     21,236,199,165,134,106, 80, 58, 38, 21,  7,252,244,239,237,238,
    243,250,  6, 19, 36, 55, 78,104,134,166,202,240, 27, 73,121,173,
    228, 31, 93,158,226, 43,118,196, 23,108,197, 34,129,228, 76,182,
     36,149, 11,131,254,127,  2,137, 20,161, 52,201, 98,254,159, 68,
    236,152, 72,250,178,109, 44,238,181,127, 78, 32,245,207,173,143,
    117, 95, 77, 62, 52, 46, 44, 45, 51, 61, 75, 93,115,141,172,206,
    245, 33, 80,131,186,246, 55,123,195, 17, 97,183, 17,111,209, 56,
    163, 19,135,254,124,252,131, 13,155, 47,198, 98,  3,167, 81,  0 };




void error(const char *msg)
{
  Serial.println(msg);
  while (1);
}

bool openFrame(String filename)
{
  // char filename[20];
  // sprintf(filename, "/skulls.bmp", frame);

  bmpFile = SD.open(filename);

  if (!bmpFile)
    return false;

  Serial.print("Opened ");
  Serial.println(filename);

  return true;
}

void showColumn(uint8_t *col)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    int p = i * 3;

    // uint8_t b = col[p];
    // uint8_t g = col[p + 1];
    // uint8_t r = col[p + 2];

    uint8_t r = gamma8(col[p + 2]);
    uint8_t g = gamma8(col[p + 1]);
    uint8_t b = gamma8(col[p]);

    leds[i].r = r;
    leds[i].g = g;
    leds[i].b = b;
  }
   #ifndef DISABLE_FASTLED
     FastLED.show();
   #endif
}

void showColumn2(uint8_t *col)
{
    int i = 0;

    // Process blocks of 4 LEDs
    for (; i <= NUM_LEDS - 4; i += 4)
    {
        leds[i+0].setRGB(gammaTable[col[2]],  gammaTable[col[1]],  gammaTable[col[0]]);
        leds[i+1].setRGB(gammaTable[col[5]],  gammaTable[col[4]],  gammaTable[col[3]]);
        leds[i+2].setRGB(gammaTable[col[8]],  gammaTable[col[7]],  gammaTable[col[6]]);
        leds[i+3].setRGB(gammaTable[col[11]], gammaTable[col[10]], gammaTable[col[9]]);

        col += 12;
    }

    // Handle remainder LEDs (0–3)
    for (; i < NUM_LEDS; i++)
    {
        uint8_t b = *col++;
        uint8_t g = *col++;
        uint8_t r = *col++;

        leds[i].setRGB(gammaTable[r], gammaTable[g], gammaTable[b]);
    }
  #ifndef DISABLE_FASTLED
     FastLED.show();
  #endif
}


void bmp_paint(String filename)
{
  if (!openFrame(filename))
    return;

  bmpFile.seek(54); // skip BMP header
  int i = 0;

  LEDS.setBrightness(map(brightnessPercent, 1, 100, 1, 255));

  uint32_t frameInterval =
      map(speedPercent,
          25,
          100,
          200,
          10);

  while (bmpFile.available())
  {
    int readBytes = bmpFile.read(sdBuf, NUM_LEDS * 3);

    if (readBytes <= 0)
      break;

    showColumn2(sdBuf);
   // delayMicroseconds(1000000 / linesPerSec);
    delay(frameInterval);
    Check();

  }

  bmpFile.close();
  firing = false;
  Serial.println("BMP Painter done.");
}

inline uint8_t gamma8(uint8_t x)
{
    return pgm_read_byte(&gammaTable[x]);
}

// uint8_t gamma8(uint8_t x)
// {
//   return pow(x / 255.0, 2.2) * 255;  //calculation
// }

// void bmp_painter_setup()
// {
//   Serial.begin(115200);

//   //pinMode(TRIGGER, INPUT_PULLUP);

//   FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
//   FastLED.setBrightness(255);
//   FastLED.clear();
//   FastLED.show();


//   // Serial.println("Initializing SD...");

//   //  pinMode(TFT_CS, OUTPUT);
//   // digitalWrite(TFT_CS, HIGH);

//   // pinMode(SD_CS, OUTPUT);
//   // digitalWrite(SD_CS, LOW);
  
//   SPI.begin(sck, miso, mosi, cs);

//   if (!SD.begin(cs)) {
//     Serial.println("SD Card Mount Failed");
//     return;
//   }

//   Serial.println("Ready");
// }
