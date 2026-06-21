# Light Painting Stick

A touchscreen LED pattern controller for the Waveshare ESP32-S3 display using LVGL, FastLED, SD card pattern files, and CST816D touch input.

---

## Features

* 240x320 TFT touchscreen user interface
* LVGL v9 graphical interface
* CST816D capacitive touch support
* FastLED LED controller
* SD card file browser
* BMP file preview
* BMP-to-LED pattern playback
* Color palettes
* Overlay patterns
* Color temperature correction
* Adjustable:

  * Speed
  * Brightness
  * Countdown timer
  * Direction
  * Repeat count
  * Number of LEDs
* Long-press BMP preview
* Touch-to-stop run screen
* Scrollable file explorer
* Modal dialogs
* Startup pattern selection

---

# Hardware

## Waveshare ESP32-S3 LCD

* ESP32-S3
* 240x320 ST7789 display
* CST816D capacitive touch
* MicroSD card

---

# Pin Configuration

## TFT

```cpp
#define TFT_MISO 40
#define TFT_MOSI 38
#define TFT_SCLK 39
#define TFT_CS   45
#define TFT_DC   42
#define TFT_RST  -1
```

## Touch

```cpp
#define TOUCH_SDA 48
#define TOUCH_SCL 47
#define TOUCH_RST 13
#define TOUCH_INT 14
#define CST816D_ADDR 0x15
```

## LED Strip

```cpp
#define LED_PIN     2
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB
```

---

# Required Libraries

Install these libraries from the Arduino Library Manager.

| Library  | Version  |
| -------- | -------- |
| LVGL     | 9.x      |
| TFT_eSPI | Latest   |
| FastLED  | Latest   |
| SD       | Built-in |
| SPI      | Built-in |
| Wire     | Built-in |

---

# Library Setup

This project requires custom configuration files for both LVGL and TFT_eSPI.

---

## LVGL Setup

1. Install the LVGL library using the Arduino Library Manager.

2. Locate the LVGL library folder:

### Windows

```text
Documents\Arduino\libraries\lvgl
```

3. Copy the provided `lv_conf.h` file into:

```text
Documents\Arduino\libraries\lvgl\
```

4. Open:

```text
lv_conf.h
```

Verify the following settings:

```cpp
#define LV_USE_LOG                0
#define LV_COLOR_DEPTH            16
#define LV_MEM_SIZE               (64U * 1024U)
```

For ESP32-S3 with PSRAM you may increase memory:

```cpp
#define LV_MEM_SIZE               (128U * 1024U)
```

---

## TFT_eSPI Setup

1. Install the TFT_eSPI library.

2. Locate:

```text
Documents\Arduino\libraries\TFT_eSPI\
```

3. Replace the following files with the project versions:

```text
User_Setup.h
User_Setup_Select.h
```

or alternatively create a custom setup file.

---

### TFT_eSPI User_Setup.h

```cpp
#define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_MISO 40
#define TFT_MOSI 38
#define TFT_SCLK 39

#define TFT_CS   45
#define TFT_DC   42
#define TFT_RST  -1

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8

#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000
```

---

## ESP32 Board Settings

Arduino IDE:

| Setting          | Value              |
| ---------------- | ------------------ |
| Board            | ESP32S3 Dev Module |
| USB CDC On Boot  | Enabled            |
| Flash Size       | 16MB               |
| PSRAM            | Enabled            |
| Partition Scheme | Huge APP           |
| Upload Speed     | 921600             |

---

## Enable PSRAM

The project uses PSRAM for:

* LVGL buffers
* BMP image loading
* File browser memory
* Large image previews

Verify PSRAM is enabled:

```cpp
Serial.printf(
    "PSRAM: %d bytes\n",
    ESP.getPsramSize());
```

---

## First Build

1. Install all required libraries.
2. Copy `lv_conf.h`.
3. Copy `User_Setup.h`.
4. Enable PSRAM.
5. Compile and upload.

---

# Features

## Patterns

### Solid Colors

* White
* Red
* Green
* Blue
* Yellow
* Cyan
* Magenta
* Orange

### Color Palettes

* Rainbow
* Rainbow Stripes
* Rainbow Pride
* RB Vertical
* RB Diagonal

### Overlays

* Ribbon 1
* Ribbon 2
* Ribbon 3
* Checker 8
* Checker 12
* Checker 16
* Stripe 3
* Stripe 4
* Stripe 5
* Stripe 8
* Stripe 12

---

# File Browser

* SD card browsing
* Directories first
* BMP files only
* Long press to preview BMP
* Scrollable file list

---

# Run Screen

When a pattern starts:

* LEDs begin output
* Countdown timer support
* Touch anywhere to stop
* Return to menu automatically

---

# Color Temperature

Supports FastLED color correction:

* UncorrectedColor
* Candle
* Tungsten40W
* Tungsten100W
* Halogen
* CarbonArc
* HighNoonSun
* DirectSunlight
* OvercastSky
* ClearBlueSky
* WarmFluorescent
* StandardFluorescent
* CoolWhiteFluorescent
* FullSpectrumFluorescent
* GrowLightFluorescent

---

# Building

Arduino IDE:

1. Install ESP32 board package.
2. Select ESP32-S3 board.
3. Enable PSRAM.
4. Set Flash Mode according to your board.
5. Upload.

---

# Future Improvements

* Animated BMP playback
* GIF support
* Presets
* EEPROM settings
* WiFi control
* OTA updates
* Playlist support

---

# License

MIT License
