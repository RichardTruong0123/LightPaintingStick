#ifndef GLOBALS_H
#define GLOBALS_H

String currentVersion = "2.0.3";
String appVersion = "v" + currentVersion;

// =====================================================
// SD CARD PINS
// =====================================================

#define SD_CS    41
#define SD_MOSI  38
#define SD_MISO  40
#define SD_SCK   39
//#define TFT_CS   45

bool sdCardAvailable = false;

// #define EEPROM_SIZE 32   // must be >= highest address used (ony 7 is used)
// =====================================================
// WIFI SETTINGS
// =====================================================

#define WIFI_SSID_MAX_LEN      32
#define WIFI_PASSWORD_MAX_LEN  64

char wifiSSID[WIFI_SSID_MAX_LEN] = "";
char wifiPassword[WIFI_PASSWORD_MAX_LEN] = "";

bool connectToWiFi();
bool isWiFiConnected();
bool wifiSettingsForUpdate = false;
volatile bool checkUpdateRequested = false;


extern int speedPercent = 80;
extern int brightnessPercent = 80;
extern int countDownTimer = 0;
extern bool autoIncrEnabled = false;
extern int imageDirection = 0;
extern bool flipVerticalEnabled = false;
extern int repeat = 0;

// runtime LED count from menu
//int NUM_LEDS = 144;

CRGB color = CRGB::Black;
String selectedOverlay = "";

// Define the Blank function
void Blank() {
  // This function does nothing
}

struct _Pattern {
  void (*foreground_fp)() = Blank;  //function pointer to the foreground layer
  void (*overlay_fp)() = Blank;     // Function pointer to the overlay layer
  void (*delay_fp)() = Blank;       // Function pointer for delay
  String value;                     // String value to display the combined layers
};

struct _Pattern Pattern;

void (*OverlayFunctionPointer)() = Blank;  //function pointer for Overlay


// Define the array of leds
//CRGB leds[NUM_LEDS];
ColorTemperature _colorTemperature = UncorrectedTemperature;
CRGBPalette16 currentPalette;
TBlendType currentBlending;


// // How many leds in your strip?
// #define LED_PIN 19
// #define COLOR_ORDER GRB
// //#define COLOR_ORDER RGB
// #define CHIPSET WS2812B
// #define NUM_LEDS 144
bool bmpPreviewActive = false;

boolean firing = false;
long previousMillis = 0;
long previousMillis2 = 0;

bool countdownActive = false;
uint32_t countdownStartMillis = 0;
uint8_t countdownRemaining = 0;

String bmp_filename = "";
bool bmpPlaybackFinished = false;

// Countdown

const char *countdownOptions =
    "Off\n"
    "1 Sec\n"
    "2 Sec\n"
    "3 Sec\n"
    "4 Sec\n"
    "5 Sec";

const uint8_t countdownValues[] =
{
    0,1,2,3,4,5
};

uint8_t countdownIndex = 0;
uint8_t countdownSeconds = 0;


// Direction

const char *directionOptions =
    "Left to Right\n"
    "Right to Left";

uint8_t directionIndex = 0;


// Repeat

const char *repeatOptions =
    "1X\n"
    "2X\n"
    "3X\n"
    "4X\n"
    "5X\n"
    "6X\n"
    "7X\n"
    "8X\n"
    "9X\n"
    "Forever";

uint8_t repeatIndex = 9;


// NUM LEDS

const char *numLedOptions =
    "144\n"
    "160\n"
    "198\n"
    "200";


uint8_t numLedsIndex = 0;
uint16_t NUM_LEDS = 200;

const char* countdownText[] =
{
    "Off",
    "1 Sec",
    "2 Sec",
    "3 Sec",
    "4 Sec",
    "5 Sec"
};

const char* directionText[] =
{
    "L->R",
    "R->L"
};

const char* repeatText[] =
{
    "1X",
    "2X",
    "3X",
    "4X",
    "5X",
    "6X",
    "7X",
    "8X",
    "9X",
    "Forever"
};

const uint16_t numLedValues[] =
{
    144,
    160,
    198,
    200
};
#endif
