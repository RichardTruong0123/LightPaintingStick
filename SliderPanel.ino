// =====================================================
// SLIDER PANEL GLOBALS
// =====================================================

lv_obj_t *modal_bg;
lv_obj_t *slider_panel;
lv_obj_t *slider_title;
lv_obj_t *slider_control;
lv_obj_t *slider_value_label;


SliderMode currentSliderMode;


void slider_value_changed(lv_event_t *e) {

  int val = lv_slider_get_value(slider_control);

  lv_label_set_text_fmt(slider_value_label,
                        "%d%%",
                        val);
}

void slider_ok_event(lv_event_t *e) {

  int val = lv_slider_get_value(slider_control);

  if (currentSliderMode == SLIDER_SPEED) {

    speedPercent = val;

    EEPROM.write(1, speedPercent);
    EEPROM.commit();

    lv_label_set_text_fmt(status_label,
                          "Speed: %d%%",
                          speedPercent);

    Serial.printf("Speed updated: %d%%\n",
                  speedPercent);
  }
  else {

    brightnessPercent = val;

    FastLED.setBrightness(
        map(brightnessPercent,
            1,
            100,
            1,
            255));

    FastLED.show();

    EEPROM.write(2, brightnessPercent);
    EEPROM.commit();

    lv_label_set_text_fmt(status_label,
                          "Brightness: %d%%",
                          brightnessPercent);

    Serial.printf("Brightness updated: %d%%\n",
                  brightnessPercent);
  }

  lv_obj_add_flag(modal_bg,
                  LV_OBJ_FLAG_HIDDEN);

  // Refresh current menu so values update
  draw_menu(currentMenu,
            currentMenuSize,
            lv_label_get_text(title_label));
                            
}

void show_slider_panel(const char *title,
                       int minVal,
                       int maxVal,
                       int currentVal,
                       SliderMode mode) {

  currentSliderMode = mode;

  lv_label_set_text(slider_title,
                    title);
  // lv_label_set_text_fmt(slider_title,
  //                   "%s: %d%%",
  //                     title,
  //                     currentVal);

  lv_label_set_text_fmt(slider_value_label,
                        "%d%%",
                        currentVal);
                        
  lv_slider_set_range(slider_control,
                      minVal,
                      maxVal);

  lv_slider_set_value(slider_control,
                      currentVal,
                      LV_ANIM_OFF);



  lv_obj_clear_flag(modal_bg,
                    LV_OBJ_FLAG_HIDDEN);

  lv_obj_move_foreground(modal_bg);
}

void create_slider_panel() {

  // =====================================
  // GREYED OUT BACKGROUND
  // =====================================

  modal_bg = lv_obj_create(lv_screen_active());

  lv_obj_set_size(modal_bg,
                  SCREEN_WIDTH,
                  SCREEN_HEIGHT);

  lv_obj_set_style_bg_color(modal_bg,
                            lv_color_black(),
                            0);

  lv_obj_set_style_bg_opa(modal_bg,
                          LV_OPA_60,
                          0);

  lv_obj_set_style_border_width(modal_bg,
                                0,
                                0);

  lv_obj_set_style_radius(modal_bg,
                          0,
                          0);

  lv_obj_clear_flag(modal_bg,
                    LV_OBJ_FLAG_SCROLLABLE);

  // =====================================
  // PANEL
  // =====================================

  slider_panel = lv_obj_create(modal_bg);

  lv_obj_set_size(slider_panel,
                  200,
                  170);

  lv_obj_center(slider_panel);

  lv_obj_clear_flag(slider_panel,
                    LV_OBJ_FLAG_SCROLLABLE);

  // =====================================
  // TITLE
  // =====================================

  slider_title = lv_label_create(slider_panel);

  lv_label_set_text(slider_title,
                    "Slider");

  lv_obj_align(slider_title,
               LV_ALIGN_TOP_MID,
               0,
               10);

  // =====================================
  // VALUE LABEL
  // =====================================

  slider_value_label = lv_label_create(slider_panel);

  lv_label_set_text(slider_value_label,
                    "0%");

  lv_obj_align(slider_value_label,
               LV_ALIGN_CENTER,
               0,
               -25);

  // =====================================
  // SLIDER
  // =====================================

  slider_control = lv_slider_create(slider_panel);

  lv_obj_set_width(slider_control,
                   160);

  lv_obj_align(slider_control,
               LV_ALIGN_CENTER,
               0,
               0);

  lv_obj_add_event_cb(slider_control,
                      slider_value_changed,
                      LV_EVENT_VALUE_CHANGED,
                      NULL);


  // =====================================
  // OK BUTTON
  // =====================================

  lv_obj_t *okBtn = lv_button_create(slider_panel);

  style_button(okBtn);

  lv_obj_set_size(okBtn,
                  80,
                  36);

  lv_obj_align(okBtn,
               LV_ALIGN_BOTTOM_MID,
               0,
               -10);

  lv_obj_t *okLbl = lv_label_create(okBtn);

  lv_label_set_text(okLbl,
                    "OK");

  lv_obj_center(okLbl);

  lv_obj_add_event_cb(okBtn,
                      slider_ok_event,
                      LV_EVENT_CLICKED,
                      NULL);

  // Hidden initially
  lv_obj_add_flag(modal_bg,
                  LV_OBJ_FLAG_HIDDEN);
}

