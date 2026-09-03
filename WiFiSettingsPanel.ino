// =====================================================
// WIFI SETTINGS PANEL
// =====================================================

lv_obj_t *wifi_settings_bg;
lv_obj_t *wifi_settings_panel;

lv_obj_t *wifi_ssid_label;
lv_obj_t *wifi_ssid_textarea;

lv_obj_t *wifi_password_label;
lv_obj_t *wifi_password_textarea;

lv_obj_t *wifi_ok_button;

lv_obj_t *wifi_keyboard;

// =====================================================
// OK BUTTON EVENT
// =====================================================
void wifi_settings_ok_event(lv_event_t *e)
{
    const char *ssidText =
        lv_textarea_get_text(wifi_ssid_textarea);

    const char *passwordText =
        lv_textarea_get_text(wifi_password_textarea);

    Serial.print("SSID textarea = [");
    Serial.print(ssidText);
    Serial.println("]");

    Serial.print("Password textarea = [");
    Serial.print(passwordText);
    Serial.println("]");


    // Copy to global variables
    strlcpy(
        wifiSSID,
        ssidText,
        sizeof(wifiSSID));

    strlcpy(
        wifiPassword,
        passwordText,
        sizeof(wifiPassword));

    Serial.print("Global SSID = [");
    Serial.print(wifiSSID);
    Serial.println("]");

    Serial.print("Global Password = [");
    Serial.print(wifiPassword);
    Serial.println("]");

    // Save global WiFi settings
    EEPROM_writeWiFi();

    // Reconnect using new credentials
    WiFi.disconnect(true);
    delay(100);
    connectToWiFi();
    
    // Hide keyboard
    lv_obj_add_flag(
        wifi_keyboard,
        LV_OBJ_FLAG_HIDDEN);

    lv_keyboard_set_textarea(
        wifi_keyboard,
        NULL);

    // Hide WiFi settings panel
    lv_obj_add_flag(
        wifi_settings_bg,
        LV_OBJ_FLAG_HIDDEN);

    // Return to menu
    draw_menu(
        currentMenu,
        currentMenuSize,
        lv_label_get_text(title_label));
}

// =====================================================
// WIFI TEXTBOX FOCUS EVENT
// =====================================================

void wifi_textarea_focus_event(lv_event_t *e)
{
    lv_obj_t *textarea =
        (lv_obj_t *)lv_event_get_target(e);

    if (lv_event_get_code(e) == LV_EVENT_FOCUSED)
    {
        // Tell keyboard which textarea to edit
        lv_keyboard_set_textarea(
            wifi_keyboard,
            textarea);

        // Show keyboard
        lv_obj_clear_flag(
            wifi_keyboard,
            LV_OBJ_FLAG_HIDDEN);

        lv_obj_move_foreground(
            wifi_keyboard);
    }

    if (lv_event_get_code(e) == LV_EVENT_DEFOCUSED)
    {
        // Don't immediately hide it here.
        // This allows the user to tap between
        // SSID and Password fields.
    }
}

// =====================================================
// KEYBOARD DONE EVENT
// =====================================================
void wifi_keyboard_event(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_READY)
    {
        // Hide keyboard
        lv_obj_add_flag(
            wifi_keyboard,
            LV_OBJ_FLAG_HIDDEN);

        // Remove keyboard from textarea
        lv_keyboard_set_textarea(
            wifi_keyboard,
            NULL);
    }
}

// =====================================================
// CREATE WIFI SETTINGS PANEL
// =====================================================

void create_wifi_settings_panel()
{
    // =================================================
    // BACKGROUND
    // =================================================

    wifi_settings_bg =
        lv_obj_create(lv_screen_active());

    lv_obj_remove_style_all(
        wifi_settings_bg);

    lv_obj_set_size(
        wifi_settings_bg,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    lv_obj_set_style_bg_color(
        wifi_settings_bg,
        lv_color_black(),
        0);

    lv_obj_set_style_bg_opa(
        wifi_settings_bg,
        LV_OPA_60,
        0);

    lv_obj_add_flag(
        wifi_settings_bg,
        LV_OBJ_FLAG_HIDDEN);


    // =================================================
    // PANEL
    // =================================================

    wifi_settings_panel =
        lv_obj_create(wifi_settings_bg);

    lv_obj_set_size(
        wifi_settings_panel,
        220,
        230);

    lv_obj_center(
        wifi_settings_panel);

    lv_obj_clear_flag(
        wifi_settings_panel,
        LV_OBJ_FLAG_SCROLLABLE);


    // =================================================
    // SSID LABEL
    // =================================================

    wifi_ssid_label =
        lv_label_create(wifi_settings_panel);

    lv_label_set_text(
        wifi_ssid_label,
        "SSID");

    lv_obj_align(
        wifi_ssid_label,
        LV_ALIGN_TOP_LEFT,
        10,
        10);


    // =================================================
    // SSID TEXT BOX
    // =================================================

    wifi_ssid_textarea =
        lv_textarea_create(wifi_settings_panel);

    lv_obj_set_width(
        wifi_ssid_textarea,
        200);

    lv_obj_set_height(
        wifi_ssid_textarea,
        38);

    lv_obj_align(
        wifi_ssid_textarea,
        LV_ALIGN_TOP_MID,
        0,
        30);

    lv_textarea_set_one_line(
        wifi_ssid_textarea,
        true);

    // Placeholder
    lv_textarea_set_placeholder_text(
        wifi_ssid_textarea,
        "Enter SSID");

    // Load saved SSID
    lv_textarea_set_text(
        wifi_ssid_textarea,
        wifiSSID);

    // Focus event
    lv_obj_add_event_cb(
        wifi_ssid_textarea,
        wifi_textarea_focus_event,
        LV_EVENT_FOCUSED,
        NULL);

    // =================================================
    // PASSWORD LABEL
    // =================================================

    wifi_password_label =
        lv_label_create(wifi_settings_panel);

    lv_label_set_text(
        wifi_password_label,
        "Password");

    lv_obj_align(
        wifi_password_label,
        LV_ALIGN_TOP_LEFT,
        10,
        75);


    // =================================================
    // PASSWORD TEXT BOX
    // =================================================

    wifi_password_textarea =
        lv_textarea_create(wifi_settings_panel);

    lv_obj_set_width(
        wifi_password_textarea,
        200);

    lv_obj_set_height(
        wifi_password_textarea,
        38);

    lv_obj_align(
        wifi_password_textarea,
        LV_ALIGN_TOP_MID,
        0,
        95);

    lv_textarea_set_one_line(
        wifi_password_textarea,
        true);

    lv_textarea_set_password_mode(
        wifi_password_textarea,
        true);

    // Placeholder
    lv_textarea_set_placeholder_text(
        wifi_password_textarea,
        "Enter Password");

    // Load saved password
    lv_textarea_set_text(
        wifi_password_textarea,
        wifiPassword);

    // Focus event
    lv_obj_add_event_cb(
        wifi_password_textarea,
        wifi_textarea_focus_event,
        LV_EVENT_FOCUSED,
        NULL);

    // =================================================
    // OK BUTTON
    // =================================================

    wifi_ok_button =
        lv_button_create(wifi_settings_panel);

    style_button(
        wifi_ok_button);

    lv_obj_set_size(
        wifi_ok_button,
        80,
        35);

    lv_obj_align(
        wifi_ok_button,
        LV_ALIGN_BOTTOM_MID,
        0,
        -8);

    lv_obj_t *ok_label =
        lv_label_create(wifi_ok_button);

    lv_label_set_text(
        ok_label,
        "OK");

    lv_obj_center(ok_label);

    lv_obj_add_event_cb(
        wifi_ok_button,
        wifi_settings_ok_event,
        LV_EVENT_CLICKED,
        NULL);

    // =================================================
    // KEYBOARD
    // =================================================

    wifi_keyboard =
        lv_keyboard_create(wifi_settings_bg);

    lv_obj_set_size(
        wifi_keyboard,
        SCREEN_WIDTH,
        125);

    lv_obj_align(
        wifi_keyboard,
        LV_ALIGN_BOTTOM_MID,
        0,
        0);

    // Keyboard DONE / ENTER event
    lv_obj_add_event_cb(
        wifi_keyboard,
        wifi_keyboard_event,
        LV_EVENT_READY,
        NULL);
        
    // Hide initially
    lv_obj_add_flag(
        wifi_keyboard,
        LV_OBJ_FLAG_HIDDEN);
        
}


// =====================================================
// SHOW WIFI SETTINGS PANEL
// =====================================================

void show_wifi_settings_panel()
{
    lv_obj_clear_flag(
        wifi_settings_bg,
        LV_OBJ_FLAG_HIDDEN);

    lv_obj_move_foreground(
        wifi_settings_bg);

    lv_obj_move_foreground(
        wifi_settings_panel);
}