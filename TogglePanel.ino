// =====================================================
// TOGGLE PANEL GLOBALS
// =====================================================

lv_obj_t *toggle_modal_bg;
lv_obj_t *toggle_panel;
lv_obj_t *toggle_title;
lv_obj_t *toggle_switch;

// bool flipVerticalEnabled = false;
// bool autoIncrEnabled = false;

// enum ToggleMode {
//   TOGGLE_FLIP_VERTICAL,
//   TOGGLE_AUTO_INCR
// };

ToggleMode currentToggleMode;


void toggle_ok_event(lv_event_t *e) {

  bool enabled =
      lv_obj_has_state(toggle_switch,
                       LV_STATE_CHECKED);

  // =====================================
  // FLIP VERTICAL
  // =====================================

  if (currentToggleMode ==
      TOGGLE_FLIP_VERTICAL) {

    flipVerticalEnabled = enabled;

    EEPROM.write(5, flipVerticalEnabled);
    EEPROM.commit();

    lv_label_set_text_fmt(
        status_label,
        "Flip Vertical: %s",
        enabled ? "ON" : "OFF");

    Serial.printf(
        "Flip Vertical: %s\n",
        enabled ? "ON" : "OFF");
  }

  // =====================================
  // AUTO INCR
  // =====================================

  else {

    autoIncrEnabled = enabled;

    lv_label_set_text_fmt(
        status_label,
        "Auto Incr: %s",
        enabled ? "ON" : "OFF");

    EEPROM.write(6, autoIncrEnabled);
    EEPROM.commit();

    Serial.printf(
        "Auto Incr: %s\n",
        enabled ? "ON" : "OFF");
  }

  // refresh menu values
  draw_menu(currentMenu,
            currentMenuSize,
            lv_label_get_text(title_label));

  // hide modal
  lv_obj_add_flag(toggle_modal_bg,
                  LV_OBJ_FLAG_HIDDEN);
}

void show_toggle_panel(const char *title,
                       bool currentValue,
                       ToggleMode mode) {

  currentToggleMode = mode;

  lv_label_set_text(toggle_title,
                    title);

  if (currentValue) {

    lv_obj_add_state(toggle_switch,
                     LV_STATE_CHECKED);

  } else {

    lv_obj_clear_state(toggle_switch,
                       LV_STATE_CHECKED);
  }

  lv_obj_clear_flag(toggle_modal_bg,
                    LV_OBJ_FLAG_HIDDEN);

  lv_obj_move_foreground(toggle_modal_bg);
}

void create_toggle_panel() {

  // =====================================
  // DARKENED BACKGROUND
  // =====================================

  toggle_modal_bg =
      lv_obj_create(lv_screen_active());

  lv_obj_remove_style_all(toggle_modal_bg);

  lv_obj_set_size(toggle_modal_bg,
                  SCREEN_WIDTH,
                  SCREEN_HEIGHT);

  lv_obj_set_style_bg_color(toggle_modal_bg,
                            lv_color_black(),
                            0);

  lv_obj_set_style_bg_opa(toggle_modal_bg,
                          LV_OPA_60,
                          0);

  // =====================================
  // PANEL
  // =====================================

  toggle_panel =
      lv_obj_create(toggle_modal_bg);

  lv_obj_set_size(toggle_panel,
                  200,
                  140);

  lv_obj_center(toggle_panel);

  lv_obj_clear_flag(toggle_panel,
                    LV_OBJ_FLAG_SCROLLABLE);

  // =====================================
  // TITLE
  // =====================================

  toggle_title =
      lv_label_create(toggle_panel);

  lv_label_set_text(toggle_title,
                    "Toggle");

  lv_obj_align(toggle_title,
               LV_ALIGN_TOP_MID,
               0,
               10);

  // =====================================
  // SWITCH
  // =====================================

  toggle_switch =
      lv_switch_create(toggle_panel);

  lv_obj_align(toggle_switch,
               LV_ALIGN_CENTER,
               0,
               -10);

  // =====================================
  // OK BUTTON
  // =====================================

  lv_obj_t *okBtn =
      lv_button_create(toggle_panel);

  style_button(okBtn);

  lv_obj_set_size(okBtn,
                  80,
                  36);

  lv_obj_align(okBtn,
               LV_ALIGN_BOTTOM_MID,
               0,
               -10);

  lv_obj_t *okLbl =
      lv_label_create(okBtn);

  lv_label_set_text(okLbl,
                    "OK");

  lv_obj_center(okLbl);

  lv_obj_add_event_cb(okBtn,
                      toggle_ok_event,
                      LV_EVENT_CLICKED,
                      NULL);

  // hidden initially
  lv_obj_add_flag(toggle_modal_bg,
                  LV_OBJ_FLAG_HIDDEN);
}