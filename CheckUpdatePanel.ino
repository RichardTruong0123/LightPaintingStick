#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <Update.h>

// =====================================================
// OTA SETTINGS
// =====================================================

const char *firmwareUrl =
    "https://github.com/RichardTruong0123/LightPaintingStick-WebFlasher/raw/refs/heads/main/firmware/LightPaintingStick.ino.bin";

const char *versionUrl =
    "https://raw.githubusercontent.com/RichardTruong0123/LightPaintingStick-WebFlasher/refs/heads/main/firmware/version.txt";

// =====================================================
// CHECK UPDATE PANEL
// =====================================================

lv_obj_t *check_update_bg;
lv_obj_t *check_update_panel;

lv_obj_t *check_update_version_label;
lv_obj_t *check_update_progress;
lv_obj_t *check_update_button;
lv_obj_t *check_update_cancel_button;

enum OTAState
{
    OTA_IDLE,
    OTA_CONNECTING,
    OTA_DOWNLOADING,
    OTA_FINISHED,
    OTA_ERROR
};

OTAState otaState = OTA_IDLE;

HTTPClient otaHTTP;
Stream *otaStream = nullptr;

int otaContentLength = 0;
size_t otaBytesWritten = 0;

String latestVersion = "";

// =====================================================
// UPDATE STATE
// =====================================================

bool firmwareUpdateInProgress = false;


// =====================================================
// FETCH LATEST VERSION
// =====================================================

String fetchLatestVersion()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected");
        return "";
    }

    HTTPClient http;

    http.setFollowRedirects(
        HTTPC_STRICT_FOLLOW_REDIRECTS);

    http.begin(versionUrl);

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        latestVersion =
            http.getString();

        latestVersion.trim();

        http.end();

        Serial.print(
            "Latest Firmware Version: ");

        Serial.println(
            latestVersion);

        return latestVersion;
    }

    Serial.printf(
        "Failed to fetch version. HTTP code: %d\n",
        httpCode);

    http.end();

    return "";
}


// =====================================================
// CHECK FOR UPDATE
// =====================================================

bool checkForFirmwareUpdate()
{
    Serial.println(
        "Checking for firmware update...");

    String latestVersion =
        fetchLatestVersion();

    if (latestVersion == "")
    {
        return false;
    }

    Serial.print(
        "Current Firmware Version: ");

    Serial.println(currentVersion);

    Serial.print(
        "Latest Firmware Version: ");

    Serial.println(latestVersion);

    if (latestVersion != currentVersion)
    {
        Serial.println(
            "New firmware available");

        return true;
    }

    Serial.println(
        "Device is up to date.");

    return false;
}


// =====================================================
// UPDATE PROGRESS
// =====================================================
void updateFirmwareProgress(
    size_t written,
    size_t total)
{
    if (total == 0)
        return;

    int percent =
        (written * 100) / total;

    if (percent > 100)
        percent = 100;

    lv_bar_set_value(
        check_update_progress,
        percent,
        LV_ANIM_OFF);

    lv_refr_now(NULL);
//    lv_timer_handler();
}

// =====================================================
// START OTA UPDATE
// =====================================================
bool startOTAUpdate(
    Stream *stream,
    int contentLength)
{
    Serial.println("Initializing update...");

    if (!Update.begin(contentLength))
    {
        Serial.printf(
            "Update begin failed: %s\n",
            Update.errorString());

        return false;
    }

    size_t written = 0;

    unsigned long lastDataTime = millis();

    const unsigned long timeoutDuration = 120000;

    uint8_t buffer[1024];

    while (written < contentLength)
    {
        // ---------------------------------------------
        // Process any available firmware data
        // ---------------------------------------------

        int availableBytes = stream->available();

        if (availableBytes > 0)
        {
            size_t bytesToRead = availableBytes;

            if (bytesToRead > sizeof(buffer))
                bytesToRead = sizeof(buffer);

            size_t len =
                stream->readBytes(
                    buffer,
                    bytesToRead);

            if (len > 0)
            {
                size_t result =
                    Update.write(
                        buffer,
                        len);

                if (result != len)
                {
                    Serial.println(
                        "OTA write failed");

                    Update.abort();

                    return false;
                }

                written += len;

                // -------------------------------------
                // Calculate progress
                // -------------------------------------

                int progress =
                    (written * 100) /
                    contentLength;

                if (progress > 100)
                    progress = 100;

                // -------------------------------------
                // Update LVGL progress bar
                // -------------------------------------

                lv_bar_set_value(
                    check_update_progress,
                    progress,
                    LV_ANIM_OFF);

                // Force LVGL to process the update
                lv_timer_handler();

                Serial.printf(
                    "OTA: %d%% (%u / %d)\n",
                    progress,
                    (unsigned int)written,
                    contentLength);

                lastDataTime = millis();
            }
        }
        else
        {
            // -----------------------------------------
            // No data currently available
            // Give LVGL some CPU time
            // -----------------------------------------

            lv_timer_handler();

            delay(1);
        }

        // ---------------------------------------------
        // Timeout detection
        // ---------------------------------------------

        if (millis() - lastDataTime >
            timeoutDuration)
        {
            Serial.println(
                "OTA timeout");

            Update.abort();

            return false;
        }

        yield();
    }

    // ---------------------------------------------
    // Make sure progress reaches 100%
    // ---------------------------------------------

    lv_bar_set_value(
        check_update_progress,
        100,
        LV_ANIM_OFF);

    lv_timer_handler();

    // ---------------------------------------------
    // Finish OTA
    // ---------------------------------------------

    if (!Update.end())
    {
        Serial.printf(
            "Update end failed: %s\n",
            Update.errorString());

        return false;
    }

    Serial.println(
        "OTA update successfully completed");

    return true;
}

// =====================================================
// DOWNLOAD AND APPLY FIRMWARE
// =====================================================
void downloadAndApplyFirmware()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected");
        return;
    }

    Serial.println("Starting firmware download...");

    otaHTTP.setFollowRedirects(
        HTTPC_STRICT_FOLLOW_REDIRECTS);

    otaHTTP.begin(firmwareUrl);

    int httpCode = otaHTTP.GET();

    if (httpCode != HTTP_CODE_OK)
    {
        Serial.printf(
            "HTTP error: %d\n",
            httpCode);

        otaHTTP.end();

        otaState = OTA_ERROR;

        return;
    }

    otaContentLength =
        otaHTTP.getSize();

    if (otaContentLength <= 0)
    {
        Serial.println(
            "Invalid firmware size");

        otaHTTP.end();

        otaState = OTA_ERROR;

        return;
    }

    if (!Update.begin(otaContentLength))
    {
        Serial.printf(
            "Update.begin failed: %s\n",
            Update.errorString());

        otaHTTP.end();

        otaState = OTA_ERROR;

        return;
    }

    otaStream =
        otaHTTP.getStreamPtr();

    otaBytesWritten = 0;

    lv_bar_set_value(
        check_update_progress,
        0,
        LV_ANIM_OFF);

    otaState =
        OTA_DOWNLOADING;
}


// =====================================================
// UPDATE BUTTON EVENT
// =====================================================

void check_update_firmware_event(
    lv_event_t *e)
{
    if (firmwareUpdateInProgress)
        return;

    firmwareUpdateInProgress =
        true;

    // Disable button while updating
    lv_obj_add_state(
        check_update_button,
        LV_STATE_DISABLED);

    Serial.println(
        "Update Firmware clicked");

    downloadAndApplyFirmware();
}


// =====================================================
// CANCEL BUTTON EVENT
// =====================================================

void check_update_cancel_event(
    lv_event_t *e)
{
    if (firmwareUpdateInProgress)
        return;

    lv_obj_add_flag(
        check_update_bg,
        LV_OBJ_FLAG_HIDDEN);

    draw_menu(
        currentMenu,
        currentMenuSize,
        lv_label_get_text(title_label));
}


// =====================================================
// CREATE CHECK UPDATE PANEL
// =====================================================

void create_check_update_panel()
{
    // =================================================
    // BACKGROUND
    // =================================================

    check_update_bg =
        lv_obj_create(
            lv_screen_active());

    lv_obj_remove_style_all(
        check_update_bg);

    lv_obj_set_size(
        check_update_bg,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    lv_obj_set_style_bg_color(
        check_update_bg,
        lv_color_black(),
        0);

    lv_obj_set_style_bg_opa(
        check_update_bg,
        LV_OPA_60,
        0);

    lv_obj_add_flag(
        check_update_bg,
        LV_OBJ_FLAG_HIDDEN);


    // =================================================
    // PANEL
    // =================================================

    check_update_panel =
        lv_obj_create(
            check_update_bg);

    lv_obj_set_size(
        check_update_panel,
        220,
        200);

    lv_obj_center(
        check_update_panel);

    lv_obj_clear_flag(
        check_update_panel,
        LV_OBJ_FLAG_SCROLLABLE);


    // =================================================
    // TITLE
    // =================================================

    lv_obj_t *title =
        lv_label_create(
            check_update_panel);

    lv_label_set_text(
        title,
        "Firmware Update");

    lv_obj_align(
        title,
        LV_ALIGN_TOP_MID,
        0,
        8);

    // =================================================
    // VERSION LABEL
    // =================================================
    check_update_version_label =
        lv_label_create(check_update_panel);

    lv_label_set_text(
        check_update_version_label,
        "Current: --\nLatest: --");

    lv_obj_set_style_text_font(
        check_update_version_label,
        &lv_font_montserrat_10,
        LV_PART_MAIN);
        
    lv_obj_align(
        check_update_version_label,
        LV_ALIGN_TOP_MID,
        0,
        35);

    // =================================================
    // PROGRESS BAR
    // =================================================
    check_update_progress =
        lv_bar_create(
            check_update_panel);

    lv_obj_set_width(
        check_update_progress,
        190);

    lv_obj_set_height(
        check_update_progress,
        20);

    lv_obj_align(
        check_update_progress,
        LV_ALIGN_TOP_MID,
        0,
        85);

    lv_bar_set_range(
        check_update_progress,
        0,
        100);

    lv_bar_set_value(
        check_update_progress,
        0,
        LV_ANIM_OFF);


    // =================================================
    // UPDATE BUTTON
    // =================================================

    check_update_button =
        lv_button_create(
            check_update_panel);

    style_button(
        check_update_button);

    lv_obj_set_size(
        check_update_button,
        80,
        35);

    lv_obj_align(
        check_update_button,
        LV_ALIGN_BOTTOM_LEFT,
        10,
        -10);

    lv_obj_t *updateLabel =
        lv_label_create(
            check_update_button);

    lv_label_set_text(
        updateLabel,
        "Update");

    lv_obj_center(
        updateLabel);

    lv_obj_add_event_cb(
        check_update_button,
        check_update_firmware_event,
        LV_EVENT_CLICKED,
        NULL);


    // =================================================
    // CANCEL BUTTON
    // =================================================

    check_update_cancel_button =
        lv_button_create(
            check_update_panel);

    style_button(
        check_update_cancel_button);

    lv_obj_set_size(
        check_update_cancel_button,
        80,
        35);

    lv_obj_align(
        check_update_cancel_button,
        LV_ALIGN_BOTTOM_RIGHT,
        -10,
        -10);

    lv_obj_t *cancelLabel =
        lv_label_create(
            check_update_cancel_button);

    lv_label_set_text(
        cancelLabel,
        "Cancel");

    lv_obj_center(
        cancelLabel);

    lv_obj_add_event_cb(
        check_update_cancel_button,
        check_update_cancel_event,
        LV_EVENT_CLICKED,
        NULL);
}


// =====================================================
// SHOW CHECK UPDATE PANEL
// =====================================================

void show_check_update_panel()
{
    firmwareUpdateInProgress =
        false;

    lv_bar_set_value(
        check_update_progress,
        0,
        LV_ANIM_OFF);

    lv_obj_remove_state(
        check_update_button,
        LV_STATE_DISABLED);

    updateCheckUpdateVersionLabel();
    
    lv_obj_clear_flag(
        check_update_bg,
        LV_OBJ_FLAG_HIDDEN);

    lv_obj_move_foreground(
        check_update_bg);
}

void processOTA()
{
    if (otaState != OTA_DOWNLOADING)
        return;

    if (otaStream == nullptr)
        return;

    uint8_t buffer[1024];

    int availableBytes =
        otaStream->available();

    if (availableBytes > 0)
    {
        size_t bytesToRead =
            min(
                availableBytes,
                (int)sizeof(buffer));

        size_t len =
            otaStream->readBytes(
                buffer,
                bytesToRead);

        if (len > 0)
        {
            size_t written =
                Update.write(
                    buffer,
                    len);

            if (written != len)
            {
                Serial.println(
                    "OTA write failed");

                Update.abort();
                otaHTTP.end();

                otaState = OTA_ERROR;

                return;
            }

            otaBytesWritten += len;

            int percent =
                (otaBytesWritten * 100) /
                otaContentLength;

            if (percent > 100)
                percent = 100;

            lv_bar_set_value(
                check_update_progress,
                percent,
                LV_ANIM_OFF);

            Serial.printf(
                "OTA: %d%%\n",
                percent);
        }
    }

    if (otaBytesWritten >= otaContentLength)
    {
        Serial.println(
            "Download complete");

        if (Update.end())
        {
            otaHTTP.end();

            otaState =
                OTA_FINISHED;

            lv_bar_set_value(
                check_update_progress,
                100,
                LV_ANIM_OFF);

            Serial.println(
                "OTA update complete");

            delay(1000);

            ESP.restart();
        }
        else
        {
            Serial.printf(
                "Update.end failed: %s\n",
                Update.errorString());

            otaHTTP.end();

            otaState = OTA_ERROR;
        }
    }
}

void updateCheckUpdateVersionLabel()
{
    String text =
        "Current: " + currentVersion +
        "\nLatest:  " + latestVersion;

    lv_label_set_text(
        check_update_version_label,
        text.c_str());
}

void checkUpdateProcess()
{
    if (!isWiFiConnected())
    {
        if (!connectToWiFi())
        {
            show_message_panel(
                "Unable to connect\nto WiFi.");

            return;
        }
    }

    if (checkForFirmwareUpdate())
    {
        updateCheckUpdateVersionLabel();

        wifiSettingsForUpdate = true;

        show_check_update_panel();
    }
    else
    {
        show_message_panel(
            "No new update\navailable.");
        delay(2000);
        hide_message_panel();
    }

}