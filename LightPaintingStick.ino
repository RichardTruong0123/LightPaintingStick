#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <FastLED.h>

#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

#include <Globals.h>

//#define DEBUG
//#define DISABLE_FASTLED

/* =========================================================
   DISPLAY SETTINGS
   ========================================================= */

#define SCREEN_ROTATION 0

#if (SCREEN_ROTATION == 0 || SCREEN_ROTATION == 2)
  #define SCREEN_WIDTH  240
  #define SCREEN_HEIGHT 320
#else
  #define SCREEN_WIDTH  320
  #define SCREEN_HEIGHT 240
#endif

/* =========================================================
   CST816D TOUCH SETTINGS
   ========================================================= */

#define TOUCH_SDA 48
#define TOUCH_SCL 47
#define TOUCH_RST 13
#define TOUCH_INT 14

#define CST816D_ADDR 0x15

#define CST816D_FINGER_NUM 0x02
#define CST816D_XPOS_H     0x03
#define CST816D_XPOS_L     0x04
#define CST816D_YPOS_H     0x05
#define CST816D_YPOS_L     0x06

// =====================================================
// FASTLED CONFIGURATION
// =====================================================

#define LED_PIN       2
#define CHIPSET       WS2812B
#define COLOR_ORDER   GRB

#define MAX_LED_COUNT 200

CRGB leds[MAX_LED_COUNT];

//CRGBPalette16 currentPalette;
//
// // runtime LED count from menu
// int NUM_LEDS = 144;

// CRGB color = CRGB::Black;


/* =========================================================
   GLOBALS
   ========================================================= */

TFT_eSPI tft = TFT_eSPI();

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

lv_obj_t *menu_container;
lv_obj_t *title_label;
lv_obj_t *version_label;
lv_obj_t *status_label;

lv_obj_t *bottom_bar;
lv_obj_t *status_bar;
lv_obj_t *back_button;
lv_obj_t *start_button;
lv_obj_t *run_screen;
lv_obj_t *run_label;

// =====================================================
// FILE EXPLORER
// =====================================================
lv_obj_t *file_browser_bg;
lv_obj_t *file_browser_container;
lv_obj_t *file_list;

lv_obj_t *bmp_preview_bg;
lv_obj_t *bmp_preview_img;

// lv_obj_t *bmp_screen;
// lv_obj_t *bmp_img;

SPIClass sdSPI(FSPI);
SPIClass hspiSPI(HSPI);

bool fileBrowserVisible = false;

String currentPath = "/";

bool ledsRunning = false;

static const lv_color_t UI_BG         = lv_color_hex(0x424242);
static const lv_color_t UI_SURFACE    = lv_color_hex(0xededd5);
static const lv_color_t UI_HIGHLIGHT  = lv_color_hex(0x3B82F6);
static const lv_color_t UI_TEXT       = lv_color_hex(0xFFFFFF);
static const lv_color_t UI_TEXT_DIM   = lv_color_hex(0xAAAAAA);
static const lv_color_t UI_BORDER     = lv_color_hex(0x616161);

static const lv_color_t UI_MENU_BUTTON   = lv_color_hex(0X5181b4);
static const lv_color_t UI_MENU_BAR     = lv_color_hex(0x5181b4);
static const lv_color_t UI_BOTTOM_BAR     = lv_color_hex(0x616161);


//int speedPercent = 75;
//int brightnessPercent = 50;

// enum AppScreen {
//   SCREEN_MENU,
//   SCREEN_BROWSER,
//   SCREEN_BMP_PREVIEW
// };

// AppScreen currentScreen = SCREEN_MENU;

enum AppMode
{
    MODE_MENU,
    MODE_FILE_BROWSER,
    MODE_RUNNING
};
AppMode currentMode = MODE_MENU;
uint32_t patternStartTime = 0;

enum SliderMode {
  SLIDER_SPEED,
  SLIDER_BRIGHTNESS
};

//bool flipVerticalEnabled = false;
//bool autoIncrEnabled = false;

enum ToggleMode {
  TOGGLE_FLIP_VERTICAL,
  TOGGLE_AUTO_INCR
};

String countdownValue = "Off";
String directionValue = "Left to Right";
String repeatValue = "Forever";
String numLedsValue = "144";

// enum DropdownMode {
//   DROPDOWN_COUNTDOWN,
//   DROPDOWN_DIRECTION,
//   DROPDOWN_REPEAT,
//   DROPDOWN_NUM_LEDS
// };

enum DropdownType
{
    DROPDOWN_NONE,
    DROPDOWN_COUNTDOWN,
    DROPDOWN_DIRECTION,
    DROPDOWN_REPEAT,
    DROPDOWN_NUMLEDS
};
DropdownType activeDropdown = DROPDOWN_NONE;


// =====================================================
// PATTERN SELECTION
// =====================================================

enum SelectedPattern {
  PATTERN_NONE,
  PATTERN_SOLID_COLORS,
  PATTERN_COLOR_PALETTE,
  PATTERN_CONFETTI,
  PATTERN_DEMO,
  PATTERN_FILE
};

SelectedPattern selectedPattern =
    PATTERN_NONE;

String selectedSolidColor = "";

/* =========================================================
   MENU DATA
   ========================================================= */

struct MenuItem {
  const char *name;
  const MenuItem *children;
  uint8_t childCount;
};

void menu_click_event(lv_event_t *e);

/* ---------------- PATTERNS ---------------- */

const MenuItem solidColors[] = {
  {"White", NULL, 0},
  {"Red", NULL, 0},
  {"Green", NULL, 0},
  {"Blue", NULL, 0},
  {"Yellow", NULL, 0},
  {"Cyan", NULL, 0},
  {"Magenta", NULL, 0},
  {"Orange", NULL, 0}
};

const MenuItem palettes[] = {
  {"Rainbow", NULL, 0},
  {"Rainbow Stripes", NULL, 0},
  {"Rainbow Pride", NULL, 0},
  {"RB Vertical", NULL, 0},
  {"RB Diagonal", NULL, 0}
};

const MenuItem overlays[] = {
  {"No Overlay", NULL, 0},
  {"Ribbon 1", NULL, 0},
  {"Ribbon 2", NULL, 0},
  {"Ribbon 3", NULL, 0},
  {"Checker 8", NULL, 0},
  {"Checker 12", NULL, 0},
  {"Checker 16", NULL, 0},
  {"Stripe 3", NULL, 0},
  {"Stripe 4", NULL, 0},
  {"Stripe 5", NULL, 0},
  {"Stripe 8", NULL, 0},
  {"Stripe 12", NULL, 0}
};

const MenuItem patternsMenu[] = {
  {"Solid Colors", solidColors, sizeof(solidColors) / sizeof(MenuItem)},
  {"Color Palette", palettes, sizeof(palettes) / sizeof(MenuItem)},
  {"Overlay", overlays, sizeof(overlays) / sizeof(MenuItem)},
  {"Pattern Files", NULL, 0},
  {"Demo Patterns", NULL, 0}
};

/* ---------------- ADVANCED ---------------- */

const MenuItem advancedSettings[] = {
  {"Direction", NULL, 0},
  {"Flip Vertical", NULL, 0},
  {"Auto Incr", NULL, 0},
  {"Repeat", NULL, 0},
  {"NUM LEDS", NULL, 0},
  {"Startup Pattern", NULL, 0}
};

/* ---------------- COLOR TEMP ---------------- */

const MenuItem colorTempMenu[] = {
  {"No Correction", NULL, 0},
  {"Candle", NULL, 0},
  {"Tungsten 40W", NULL, 0},
  {"Tungsten 100W", NULL, 0},
  {"Halogen", NULL, 0},
  {"Carbon Arc", NULL, 0},
  {"High Noon Sun", NULL, 0},
  {"Direct Sunlight", NULL, 0},
  {"Overcast Sky", NULL, 0},
  {"Clear Blue Sky", NULL, 0},
  {"Warm Fluorescent", NULL, 0},
  {"Standard FLUOR", NULL, 0},
  {"Cool White FLUOR", NULL, 0},
  {"Full Spect FLUOR", NULL, 0},
  {"Grow Light FLUOR", NULL, 0}
};

/* ---------------- ROOT ---------------- */

const MenuItem rootMenu[] = {
  {"Patterns", patternsMenu, sizeof(patternsMenu) / sizeof(MenuItem)},
  {"Speed", NULL, 0},
  {"Brightness", NULL, 0},
  {"Countdown", NULL, 0},
  {"Advanced Settings", advancedSettings, sizeof(advancedSettings) / sizeof(MenuItem)},
  {"Color Temperature", colorTempMenu, sizeof(colorTempMenu) / sizeof(MenuItem)}
};

void SetVersion(const char *version)
{
    appVersion = version;

    if (version_label)
    {
        lv_label_set_text(
            version_label,
            appVersion.c_str());
    }
}

/* =========================================================
   MENU NAVIGATION
   ========================================================= */

const MenuItem *menuStack[10];
uint8_t stackIndex = 0;

const MenuItem *currentMenu = rootMenu;
uint8_t currentMenuSize = sizeof(rootMenu) / sizeof(MenuItem);

/* =========================================================
   I2C FUNCTIONS
   ========================================================= */

uint8_t readRegister(uint8_t reg) {

  Wire.beginTransmission(CST816D_ADDR);
  Wire.write(reg);

  if (Wire.endTransmission(false) != 0) {
    return 0;
  }

  Wire.requestFrom(CST816D_ADDR, (uint8_t)1);

  if (Wire.available()) {
    return Wire.read();
  }

  return 0;
}

/* =========================================================
   TOUCH READ
   ========================================================= */

bool readTouch(uint16_t &x, uint16_t &y) {

  uint8_t fingers = readRegister(CST816D_FINGER_NUM);

  if (fingers == 0) {
    return false;
  }

  uint8_t xh = readRegister(CST816D_XPOS_H);
  uint8_t xl = readRegister(CST816D_XPOS_L);

  uint8_t yh = readRegister(CST816D_YPOS_H);
  uint8_t yl = readRegister(CST816D_YPOS_L);

  x = ((xh & 0x0F) << 8) | xl;
  y = ((yh & 0x0F) << 8) | yl;

  return true;
}

/* =========================================================
   LVGL DISPLAY FLUSH
   ========================================================= */

void my_disp_flush(lv_display_t *disp,
                   const lv_area_t *area,
                   uint8_t *px_map) {

  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();

  tft.setAddrWindow(area->x1, area->y1, w, h);

  tft.pushColors((uint16_t *)px_map, w * h, true);

  tft.endWrite();

  lv_display_flush_ready(disp);
}

/* =========================================================
   LVGL TOUCH CALLBACK
   ========================================================= */

void my_touchpad_read(lv_indev_t *indev,
                      lv_indev_data_t *data) {

  uint16_t x, y;

    // if (bmpPreviewActive)
    // {
    //     if (digitalRead(TOUCH_INT) == LOW)
    //     {
    //         Serial.println("Exit Preview");

    //         bmpPreviewActive = false;

    //         lv_obj_clear_flag(file_browser_bg,
    //                           LV_OBJ_FLAG_HIDDEN);

    //         lv_timer_handler();
    //     }

    //     data->state =
    //         LV_INDEV_STATE_RELEASED;

    //     return;
    // }

  if (readTouch(x, y)) {

    data->point.x = x;
    data->point.y = y;

    data->state = LV_INDEV_STATE_PRESSED;

  } else {

    data->state = LV_INDEV_STATE_RELEASED;
  }
}

/* =========================================================
   MENU DRAW
   ========================================================= */

void clear_menu() {
  lv_obj_clean(menu_container);
}

void draw_menu(const MenuItem *menu,
               uint8_t count,
               const char *title) {

  clear_menu();

  lv_label_set_text(title_label, title);

  // SHOW/HIDE FIXED BACK BUTTON + BOTTOM BAR
  // =====================================
  // ROOT MENU
  // =====================================

  if (stackIndex == 0) {

    // show bottom bar only if pattern selected
    if (selectedPattern !=
        PATTERN_NONE) {

      lv_obj_clear_flag(bottom_bar,
                        LV_OBJ_FLAG_HIDDEN);

      lv_obj_add_flag(back_button,
                      LV_OBJ_FLAG_HIDDEN);

      lv_obj_clear_flag(start_button,
                        LV_OBJ_FLAG_HIDDEN);

      // Menu area smaller when bottom bar visible
      lv_obj_set_height(menu_container,
                        SCREEN_HEIGHT - 73 - 12);
    }
    else {

      lv_obj_add_flag(bottom_bar,
                      LV_OBJ_FLAG_HIDDEN);

      // Expand menu to full height
      lv_obj_set_height(menu_container,
                        SCREEN_HEIGHT - 18 - 12);
    }
  }
  // =====================================
  // SUBMENU
  // =====================================
  else {
    lv_obj_clear_flag(bottom_bar,
                      LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_flag(back_button,
                      LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_flag(start_button,
                    LV_OBJ_FLAG_HIDDEN);

    lv_obj_set_height(menu_container,
                      SCREEN_HEIGHT - 85);
  }

  // MENU ITEMS
  for (int i = 0; i < count; i++) {

    lv_obj_t *btn = lv_button_create(menu_container);
    style_menu_button(btn);

    lv_obj_set_width(btn, lv_pct(100));
    lv_obj_set_height(btn, 50);

    // =====================================
    // LEFT LABEL
    // =====================================

    lv_obj_t *leftLabel = lv_label_create(btn);

    String leftText = menu[i].name;

    // submenu arrow
    if (menu[i].childCount > 0) {
      leftText += "  >";
    }

    lv_label_set_text(leftLabel,
                      leftText.c_str());

    lv_obj_align(leftLabel,
                LV_ALIGN_LEFT_MID,
                10,
                0);

    // =====================================
    // RIGHT VALUE LABEL
    // =====================================

    if (strcmp(menu[i].name, "Speed") == 0 ||
        strcmp(menu[i].name, "Brightness") == 0 ||
        strcmp(menu[i].name, "Flip Vertical") == 0 ||
        strcmp(menu[i].name, "Auto Incr") == 0 ||
        strcmp(menu[i].name, "Countdown") == 0 ||
        strcmp(menu[i].name, "Direction") == 0 ||
        strcmp(menu[i].name, "Repeat") == 0 ||
        strcmp(menu[i].name, "NUM LEDS") == 0) {

      lv_obj_t *rightLabel = lv_label_create(btn);

      String valueText;

      if (strcmp(menu[i].name,
                "Speed") == 0) {

        valueText =
            String(speedPercent) + "%";
      }
      else if (strcmp(menu[i].name,
                      "Brightness") == 0) {

        valueText =
            String(brightnessPercent) + "%";
      }
      else if (strcmp(menu[i].name,
                      "Flip Vertical") == 0) {

        valueText =
            flipVerticalEnabled
                ? "ON"
                : "OFF";
      }
      else if (strcmp(menu[i].name,
                      "Countdown") == 0) {

        valueText = countdownText[countdownIndex];
      }
      else if (strcmp(menu[i].name,
                      "Direction") == 0) {

        valueText = directionText[directionIndex];
      }
      else if (strcmp(menu[i].name,
                      "Repeat") == 0) {

        valueText = repeatText[repeatIndex];
      }
      else if (strcmp(menu[i].name,
                      "NUM LEDS") == 0) {

        valueText = String(numLedValues[numLedsIndex]);
      }
      else {

        valueText =
            autoIncrEnabled
                ? "ON"
                : "OFF";
      }

      lv_label_set_text(rightLabel,
                        valueText.c_str());

      lv_obj_align(rightLabel,
                  LV_ALIGN_RIGHT_MID,
                  -10,
                  0);
    }
    lv_obj_add_event_cb(btn,
                        menu_click_event,
                        LV_EVENT_CLICKED,
                        (void *)&menu[i]);
  }
}

void navigate_back(){

  // =====================================
  // GO BACK ONE MENU LEVEL
  // =====================================

  if (stackIndex > 0) {

    stackIndex--;

    if (stackIndex == 0) {

      currentMenu = rootMenu;
      currentMenuSize =
          sizeof(rootMenu) /
          sizeof(MenuItem);

      draw_menu(currentMenu,
                currentMenuSize,
                "Light Painting Stick");
    }
    else {

      const MenuItem *parent =
          menuStack[stackIndex - 1];

      currentMenu =
          parent->children;

      currentMenuSize =
          parent->childCount;

      draw_menu(currentMenu,
                currentMenuSize,
                parent->name);
    }
  }
}


/* =========================================================
   MENU CLICK EVENT
   ========================================================= */

void menu_click_event(lv_event_t *e) {

  void *ptr = lv_event_get_user_data(e);

  // BACK BUTTON
  if ((intptr_t)ptr == -1) {

    if (stackIndex > 0) {

      stackIndex--;

      if (stackIndex == 0) {

        currentMenu = rootMenu;
        currentMenuSize = sizeof(rootMenu) / sizeof(MenuItem);

        draw_menu(currentMenu,
                  currentMenuSize,
                  "Light Painting Stick");
      }
      else {

        const MenuItem *parent = menuStack[stackIndex - 1];

        currentMenu = parent->children;
        currentMenuSize = parent->childCount;

        draw_menu(currentMenu,
                  currentMenuSize,
                  parent->name);
      }
    }

    return;
  }

  MenuItem *item = (MenuItem *)ptr;

  Serial.printf("Selected: %s\n", item->name);

  // SUBMENU
  if (item->childCount > 0) {

    menuStack[stackIndex++] = item;

    currentMenu = item->children;
    currentMenuSize = item->childCount;

    draw_menu(currentMenu,
              currentMenuSize,
              item->name);
  }
  else {

    // =====================================
    // SPEED PANEL
    // =====================================

    if (strcmp(item->name, "Speed") == 0) {

      show_slider_panel("Speed",
                        25,
                        100,
                        speedPercent,
                        SLIDER_SPEED);

      return;
    }

    // =====================================
    // BRIGHTNESS PANEL
    // =====================================

    if (strcmp(item->name, "Brightness") == 0) {

      show_slider_panel("Brightness",
                        10,
                        100,
                        brightnessPercent,
                        SLIDER_BRIGHTNESS);

      return;
    }

    // =====================================
    // FLIP VERTICAL
    // =====================================

    if (strcmp(item->name,
              "Flip Vertical") == 0) {

      show_toggle_panel(
          "Flip Vertical",
          flipVerticalEnabled,
          TOGGLE_FLIP_VERTICAL);

      return;
    }

    // =====================================
    // AUTO INCR
    // =====================================

    if (strcmp(item->name,
              "Auto Incr") == 0) {

      show_toggle_panel(
          "Auto Incr",
          autoIncrEnabled,
          TOGGLE_AUTO_INCR);

      return;
    }


    // =====================================
    // DROPDOWN LIST
    // =====================================
    if(strcmp(item->name, "Countdown") == 0)
    {
        show_dropdown_panel(
            DROPDOWN_COUNTDOWN);
    }

    if(strcmp(item->name, "Direction") == 0)
    {
        show_dropdown_panel(
            DROPDOWN_DIRECTION);
    }

    if(strcmp(item->name, "Repeat") == 0)
    {
        show_dropdown_panel(
            DROPDOWN_REPEAT);
    }

    if(strcmp(item->name, "NUM LEDS") == 0)
    {
        show_dropdown_panel(
            DROPDOWN_NUMLEDS);
    }

    // =====================================
    // SOLID COLORS
    // =====================================

    if (currentMenu == solidColors) {
      SetSolidColor(item->name);

      navigate_back();

      return;
    }

    // =====================================
    // COLOR PALETTES
    // =====================================

    if (currentMenu == palettes) {
      SetColorPalette(item->name);

      navigate_back();

      return;
    }

    // =====================================
    // OVERLAY
    // =====================================

    if (currentMenu == overlays) {
      SetOverlay(item->name);

      navigate_back();

      return;
    }

    // =====================================
    // COLOR TEMP
    // =====================================
    if (currentMenu == colorTempMenu)
    {
        SetColorTemperature(item->name);

        navigate_back();

        return;
    }

    // =====================================
    // PATTERN FILES
    // =====================================
    if (strcmp(item->name,
              "Pattern Files") == 0) {
      // Check if an SD card is available
      if (!sdCardAvailable)
      {
          Serial.println("No SD card detected");

          show_message_panel("No SD card.");

          // Optional: hide message automatically after 2 seconds
          delay(2000);
          hide_message_panel();

          return;
      }

      // SD card is available
      Serial.println("SD card detected");

      show_file_browser();
      return;
    }
    
    // =====================================
    // DEMO
    // =====================================
    if (strcmp(item->name, "Demo Patterns") == 0) {
      demoSetup();
      Serial.println("Demo Patterns selected");
      selectedPattern = PATTERN_DEMO;
      navigate_back();

      return;
    }

    // =====================================
    // OTHER ACTIONS
    // =====================================

    Serial.printf("ACTION -> %s\n",
                  item->name);

    lv_label_set_text(status_label,
                      item->name);
                      
  }
}


void style_menu_button(lv_obj_t *btn) {

  lv_obj_set_style_bg_color(btn,
                            UI_MENU_BUTTON,
                            LV_PART_MAIN);

  lv_obj_set_style_bg_color(btn,
                            UI_MENU_BUTTON,
                            LV_STATE_PRESSED);

  lv_obj_set_style_border_width(btn,
                                0,
                                LV_PART_MAIN);

  lv_obj_set_style_radius(btn,
                          6,
                          LV_PART_MAIN);

  lv_obj_set_style_text_color(btn,
                              lv_color_white(),
                              LV_PART_MAIN);
}

void style_button(lv_obj_t *btn) {

  lv_obj_set_style_bg_color(btn,
                            lv_palette_main(LV_PALETTE_AMBER),
                            LV_PART_MAIN);

  lv_obj_set_style_bg_color(btn,
                            lv_palette_darken(LV_PALETTE_AMBER, 2),
                            LV_STATE_PRESSED);

  lv_obj_set_style_border_width(btn,
                                0,
                                LV_PART_MAIN);

  lv_obj_set_style_radius(btn,
                          6,
                          LV_PART_MAIN);

  lv_obj_set_style_text_color(btn,
                              lv_color_white(),
                              LV_PART_MAIN);
}

void start_button_event(lv_event_t *e) {

  Serial.println("START");

  currentMode = MODE_RUNNING;

  if (countdownSeconds > 0)
  {
      countdownActive = true;

      countdownRemaining =
          countdownSeconds;

      countdownStartMillis =
          millis();
  }
  else
  {
      countdownActive = false;
  }

  patternStartTime = millis();

  // =====================================
  // SHOW RUN SCREEN
  // =====================================

  lv_obj_clear_flag(run_screen,
                    LV_OBJ_FLAG_HIDDEN);

  lv_obj_move_foreground(run_screen);
}

void stop_leds();

void run_screen_touch_event(lv_event_t *e) {

  stop_leds();
  currentMode = MODE_MENU;
}

void create_run_screen() {

  // =====================================
  // FULL SCREEN OVERLAY
  // =====================================

  run_screen =
      lv_obj_create(lv_screen_active());

  lv_obj_remove_style_all(run_screen);

  lv_obj_set_size(run_screen,
                  SCREEN_WIDTH,
                  SCREEN_HEIGHT);

  lv_obj_set_style_bg_color(run_screen,
                            lv_color_black(),
                            0);

  lv_obj_set_style_bg_opa(run_screen,
                          LV_OPA_COVER,
                          0);

  lv_obj_add_flag(run_screen,
                  LV_OBJ_FLAG_CLICKABLE);

  lv_obj_add_event_cb(run_screen,
                      run_screen_touch_event,
                      LV_EVENT_CLICKED,
                      NULL);

  // =====================================
  // MESSAGE LABEL
  // =====================================

  run_label =
      lv_label_create(run_screen);

  if (countdownSeconds > 0)
  {
      lv_label_set_text_fmt(
          run_label,
          "Starting in %d",
          countdownSeconds);
  }
  else
  {
    lv_label_set_text(
        run_label,
        "To stop,\n"
        "touch anywhere\n"
        "on the screen.");
  }

  lv_obj_set_style_text_color(
      run_label,
      lv_palette_main(LV_PALETTE_RED),
      0);

  lv_obj_set_style_text_align(
      run_label,
      LV_TEXT_ALIGN_CENTER,
      0);

  lv_obj_set_style_text_font(
      run_label,
      &lv_font_montserrat_20,
      0);

  lv_obj_center(run_label);

  // hidden initially
  lv_obj_add_flag(run_screen,
                  LV_OBJ_FLAG_HIDDEN);
}

void stop_leds() {

  Serial.println("STOP LEDS");

#ifndef DISABLE_FASTLED

  fill_solid(leds,
             NUM_LEDS,
             CRGB::Black);

  FastLED.show();

#endif

  ledsRunning = false;

  lv_obj_add_flag(run_screen,
                  LV_OBJ_FLAG_HIDDEN);

  lv_label_set_text(status_label,
                    "Stopped");

  // redraw menu UI
  draw_menu(currentMenu,
            currentMenuSize,
            lv_label_get_text(title_label));
}

/* =========================================================
   CREATE UI
   ========================================================= */

void create_ui() {

  lv_obj_set_style_bg_color(lv_screen_active(),
                            UI_BG,
                            0);

  /* =====================================
    TITLE BAR
    ===================================== */

  lv_obj_t *title_bar = lv_obj_create(lv_screen_active());

  lv_obj_remove_style_all(title_bar);

  lv_obj_set_size(title_bar,
                  SCREEN_WIDTH,
                  12);

  lv_obj_align(title_bar,
              LV_ALIGN_TOP_LEFT,
              0,
              0);

  lv_obj_set_style_bg_color(title_bar,
                            UI_MENU_BAR,
                            0);

  lv_obj_set_style_bg_opa(title_bar,
                          LV_OPA_COVER,
                          0);

  /* =====================================
    TITLE LABEL
    ===================================== */

  title_label = lv_label_create(title_bar);

  lv_label_set_text(title_label,
                    "Light Painting Stick");

  // Small font for 12px bar
  lv_obj_set_style_text_font(title_label,
                            &lv_font_montserrat_10,
                            0);

  lv_obj_set_style_text_color(title_label,
                              lv_color_white(),
                              0);
  lv_obj_align(title_label,
              LV_ALIGN_LEFT_MID,
              4,
              0);
              
  // Version label (left side)
  version_label =
      lv_label_create(title_bar);

  lv_label_set_text(
      version_label,
      appVersion.c_str());

  // Small font for 12px bar
  lv_obj_set_style_text_font(version_label,
                            &lv_font_montserrat_10,
                            0);

  lv_obj_set_style_text_color(version_label,
                              lv_color_white(),
                              0);

  lv_obj_align(
      version_label,
      LV_ALIGN_RIGHT_MID,
      -2,
      0);
      
  /* =====================================
     SCROLLABLE MENU AREA
     ===================================== */

  menu_container = lv_obj_create(lv_screen_active());

  lv_obj_set_size(menu_container,
                  SCREEN_WIDTH - 10,
                  SCREEN_HEIGHT - 100);

  lv_obj_align(menu_container,
               LV_ALIGN_TOP_MID,
               0,
               14);

  lv_obj_set_scroll_dir(menu_container,
                        LV_DIR_VER);

  lv_obj_set_flex_flow(menu_container,
                       LV_FLEX_FLOW_COLUMN);

  lv_obj_set_flex_align(menu_container,
                        LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_START);

  lv_obj_set_style_pad_row(menu_container,
                           8,
                           0);

  lv_obj_add_flag(menu_container,
                  LV_OBJ_FLAG_SCROLL_MOMENTUM);

  lv_obj_add_flag(menu_container,
                  LV_OBJ_FLAG_SCROLL_ELASTIC);

  // lv_obj_set_style_bg_color(menu_container,
  //                           lv_palette_main(LV_PALETTE_GREY),
  //                           LV_PART_MAIN);

  lv_obj_set_style_border_color(menu_container, UI_BORDER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(menu_container,
                            UI_SURFACE,
                            0);

  /* =====================================
     FIXED BOTTOM BAR
     ===================================== */

  bottom_bar = lv_obj_create(lv_screen_active());

  lv_obj_set_size(bottom_bar,
                  SCREEN_WIDTH,
                  55);

  lv_obj_align(bottom_bar,
               LV_ALIGN_BOTTOM_MID,
               0,
               -12);

  lv_obj_clear_flag(bottom_bar,
                    LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_style_radius(bottom_bar,
                          0,
                          0);

  lv_obj_set_style_pad_all(bottom_bar,
                           8,
                           0);
                           
  lv_obj_set_style_border_color(bottom_bar, UI_BOTTOM_BAR, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bottom_bar,
                            UI_BOTTOM_BAR,
                            0);
  // Hidden on root menu
  lv_obj_add_flag(bottom_bar,
                  LV_OBJ_FLAG_HIDDEN);
  /* =====================================
     BACK BUTTON
     ===================================== */

  back_button = lv_button_create(bottom_bar);
    style_button(back_button);

  lv_obj_set_size(back_button,
                  100,
                  38);

  lv_obj_align(back_button,
               LV_ALIGN_LEFT_MID,
               0,
               0);

  lv_obj_t *back_label = lv_label_create(back_button);

  lv_label_set_text(back_label, LV_SYMBOL_LEFT " Back");

  lv_obj_center(back_label);

  lv_obj_add_event_cb(back_button,
                      menu_click_event,
                      LV_EVENT_CLICKED,
                      (void *)-1);

  // Hide initially
  lv_obj_add_flag(back_button,
                  LV_OBJ_FLAG_HIDDEN);

  /* =====================================
    START BUTTON
    ===================================== */

  start_button = lv_button_create(bottom_bar);

  style_button(start_button);

  lv_obj_set_size(start_button,
                  100,
                  38);

  lv_obj_align(start_button,
              LV_ALIGN_RIGHT_MID,
              -4,
              0);

  lv_obj_t *start_label =
      lv_label_create(start_button);

  lv_label_set_text(start_label,
                    LV_SYMBOL_PLAY " Start");

  lv_obj_center(start_label);

  lv_obj_add_event_cb(start_button,
                      start_button_event,
                      LV_EVENT_CLICKED,
                      NULL);

  // hidden initially
  lv_obj_add_flag(start_button,
                  LV_OBJ_FLAG_HIDDEN);

  /* =====================================
    STATUS BAR
    ===================================== */

  status_bar = lv_obj_create(lv_screen_active());

  lv_obj_set_size(status_bar,
                  SCREEN_WIDTH,
                  12);

  lv_obj_align(status_bar,
              LV_ALIGN_BOTTOM_MID,
              0,
              0);

  lv_obj_clear_flag(status_bar,
                    LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_style_radius(status_bar,
                          0,
                          0);

  lv_obj_set_style_border_width(status_bar,
                                0,
                                0);

  lv_obj_set_style_pad_all(status_bar,
                          0,
                          0);

  lv_obj_set_style_bg_color(status_bar,
                            UI_MENU_BAR,
                            0);

  /* =====================================
    STATUS LABEL
    ===================================== */

  status_label = lv_label_create(status_bar);

  lv_label_set_text(status_label,
                    "Ready");

  lv_obj_set_style_text_font(status_label,
                            &lv_font_montserrat_10,
                            0);

  lv_obj_set_style_text_color(status_label,
                              lv_color_white(),
                              0);

  lv_obj_align(status_label,
              LV_ALIGN_LEFT_MID,
              4,
              0);

  /* =====================================
     DRAW ROOT MENU
     ===================================== */

  draw_menu(rootMenu,
            sizeof(rootMenu) / sizeof(MenuItem),
            "Light Painting Stick");
}

/* =========================================================
   SETUP
   ========================================================= */

void setup() {
  SetVersion("v2.0.1");
  Serial.begin(115200);
   pinMode(TFT_CS, OUTPUT);
   digitalWrite(TFT_CS, HIGH);

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  delay(500);

  Serial.println("Booting...");
  psram();
  Serial.printf("numLEDs: %d\n", NUM_LEDS);

  EEPROM_read();
  Serial.printf("numLEDs: %d\n", NUM_LEDS);

  /* ---- DISPLAY ---- */

  tft.begin();

  tft.setRotation(SCREEN_ROTATION);

  tft.fillScreen(TFT_BLACK);

  if (!SD.begin(SD_CS, tft.getSPIinstance(), 20000000)) { 
    sdCardAvailable = false;
    Serial.println("SD Card Mount Failed");
  }
  else {
    sdCardAvailable = true;
    Serial.println("SD Card Mounted");
  }

    Serial.println("SD Card Ready");

  /* =====================================
    FASTLED INIT
    ===================================== */
  #ifndef DISABLE_FASTLED

  FastLED.addLeds<
      CHIPSET,
      LED_PIN,
      COLOR_ORDER>(
          leds,
          MAX_LED_COUNT)
      .setCorrection(TypicalSMD5050);
  // FastLED.setDither(0);

  // limit my draw to 5A at 5v of power draw
  FastLED.setMaxPowerInVoltsAndMilliamps(5,5000);   
  
  FastLED.setBrightness(
      map(brightnessPercent, 1, 100, 1, 255));

  FastLED.setTemperature(_colorTemperature);

  fill_solid(leds,
            MAX_LED_COUNT,
            CRGB::Black);

  fill_solid(currentPalette,
            16,
            CRGB::Black);

  FastLED.show();

  #endif

  /* ---- TOUCH ---- */

  Wire.begin(TOUCH_SDA, TOUCH_SCL);

  pinMode(TOUCH_RST, OUTPUT);
  pinMode(TOUCH_INT, INPUT_PULLUP);

  digitalWrite(TOUCH_RST, LOW);
  delay(50);

  digitalWrite(TOUCH_RST, HIGH);
  delay(50);

  Wire.beginTransmission(CST816D_ADDR);

  if (Wire.endTransmission() == 0) {

    Serial.println("CST816D detected");

  } else {

    Serial.println("CST816D NOT detected");
  }

  /* ---- LVGL ---- */

  lv_init();

  lv_display_t *disp =
      lv_display_create(SCREEN_WIDTH,
                        SCREEN_HEIGHT);

  lv_display_set_buffers(
      disp,
      draw_buf,
      NULL,
      sizeof(draw_buf),
      LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_display_set_flush_cb(disp,
                          my_disp_flush);

  lv_indev_t *indev = lv_indev_create();

  lv_indev_set_type(indev,
                    LV_INDEV_TYPE_POINTER);

  lv_indev_set_read_cb(indev,
                       my_touchpad_read);


  create_ui();
  create_slider_panel();
  create_toggle_panel();
  create_dropdown_panel();
  create_run_screen();
  create_file_browser();
  create_bmp_preview();
  create_message_panel();
  create_image_viewer_panel();
   
  Serial.println("Ready");

 //    delay(10000);

}

void processCountdown()
{
    if (!countdownActive)
        return;

    uint32_t elapsed =
        (millis() - countdownStartMillis)
            / 1000;

    uint8_t newRemaining =
        countdownSeconds - elapsed;

    if (newRemaining != countdownRemaining)
    {
        countdownRemaining =
            newRemaining;

        lv_label_set_text_fmt(
            run_label,
            "Starting in %d",
            countdownRemaining);
    }

    if (elapsed >= countdownSeconds)
    {
        countdownActive = false;

        lv_label_set_text(
          run_label,
          "To stop,\n"
          "touch anywhere\n"
          "on the screen.");
    }
}


/* =========================================================
   LOOP
   ========================================================= */

void loop() {

  static uint32_t last_tick = millis();

  lv_tick_inc(millis() - last_tick);

  last_tick = millis();

  //if (!bmpPreviewActive)
    lv_timer_handler();
      
  if(currentMode == MODE_RUNNING)
  {
      if(countdownActive)
      {
          processCountdown();
      }
      else
      {
          runPatternTask();
      }
  }
}