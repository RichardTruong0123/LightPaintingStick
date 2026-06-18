// // =====================================================
// // DROPDOWN PANEL GLOBALS
// // =====================================================

// lv_obj_t *dropdown_modal_bg;
// lv_obj_t *dropdown_panel;
// lv_obj_t *dropdown_title;
// lv_obj_t *dropdown_control;

// // String countdownValue = "Off";
// // String directionValue = "Left to Right";
// // String repeatValue = "Forever";

// // enum DropdownMode {
// //   DROPDOWN_COUNTDOWN,
// //   DROPDOWN_DIRECTION,
// //   DROPDOWN_REPEAT
// // };

// DropdownMode currentDropdownMode;

// void dropdown_ok_event(lv_event_t *e) {

//   char selected[64];

//   lv_dropdown_get_selected_str(
//       dropdown_control,
//       selected,
//       sizeof(selected));

//   // =====================================
//   // COUNTDOWN
//   // =====================================

//   if (currentDropdownMode ==
//       DROPDOWN_COUNTDOWN) {

//     countdownValue = selected;

//     // EEPROM.write(3, countdownValue);
//     // EEPROM.commit();

//     lv_label_set_text_fmt(
//         status_label,
//         "Countdown: %s",
//         selected);
//   }

//   // =====================================
//   // DIRECTION
//   // =====================================

//   else if (currentDropdownMode ==
//            DROPDOWN_DIRECTION) {

//     directionValue = selected;

//     lv_label_set_text_fmt(
//         status_label,
//         "Direction: %s",
//         selected);
//   }

//   // =====================================
//   // NUM LEDS
//   // =====================================

//   else if (currentDropdownMode ==
//           DROPDOWN_NUM_LEDS) {

//     numLedsValue = selected;
//     NUM_LEDS = atoi(selected);
    
//     lv_label_set_text_fmt(
//         status_label,
//         "NUM LEDS: %s",
//         selected);
//   }
  
//   // =====================================
//   // REPEAT
//   // =====================================

//   else {

//     repeatValue = selected;

//     lv_label_set_text_fmt(
//         status_label,
//         "Repeat: %s",
//         selected);
//   }

//   // refresh menu values
//   draw_menu(currentMenu,
//             currentMenuSize,
//             lv_label_get_text(title_label));

//   // hide modal
//   lv_obj_add_flag(dropdown_modal_bg,
//                   LV_OBJ_FLAG_HIDDEN);
// }

// void show_dropdown_panel(
//     const char *title,
//     const char *options,
//     const char *currentValue,
//     DropdownMode mode) {

//   currentDropdownMode = mode;

//   lv_label_set_text(dropdown_title,
//                     title);

//   lv_dropdown_set_options(
//       dropdown_control,
//       options);

//   // =====================================
//   // SET CURRENT SELECTION
//   // LVGL 9 COMPATIBLE
//   // =====================================

//   String opts = options;

//   int optionIndex = 0;
//   int start = 0;

//   while (true) {

//     int end = opts.indexOf('\n', start);

//     String item;

//     if (end == -1) {
//       item = opts.substring(start);
//     } else {
//       item = opts.substring(start, end);
//     }

//     item.trim();

//     if (item.equals(currentValue)) {

//       lv_dropdown_set_selected(
//           dropdown_control,
//           optionIndex);

//       break;
//     }

//     if (end == -1) {
//       break;
//     }

//     start = end + 1;
//     optionIndex++;
//   }

//   lv_obj_clear_flag(dropdown_modal_bg,
//                     LV_OBJ_FLAG_HIDDEN);

//   lv_obj_move_foreground(
//       dropdown_modal_bg);
// }

// void create_dropdown_panel() {

//   // =====================================
//   // DARKENED BACKGROUND
//   // =====================================

//   dropdown_modal_bg =
//       lv_obj_create(lv_screen_active());

//   lv_obj_remove_style_all(
//       dropdown_modal_bg);

//   lv_obj_set_size(dropdown_modal_bg,
//                   SCREEN_WIDTH,
//                   SCREEN_HEIGHT);

//   lv_obj_set_style_bg_color(
//       dropdown_modal_bg,
//       lv_color_black(),
//       0);

//   lv_obj_set_style_bg_opa(
//       dropdown_modal_bg,
//       LV_OPA_60,
//       0);

//   // =====================================
//   // PANEL
//   // =====================================

//   dropdown_panel =
//       lv_obj_create(dropdown_modal_bg);

//   lv_obj_set_size(dropdown_panel,
//                   210,
//                   150);

//   lv_obj_center(dropdown_panel);

//   lv_obj_clear_flag(dropdown_panel,
//                     LV_OBJ_FLAG_SCROLLABLE);

//   // =====================================
//   // TITLE
//   // =====================================

//   dropdown_title =
//       lv_label_create(dropdown_panel);

//   lv_label_set_text(dropdown_title,
//                     "Dropdown");

//   lv_obj_align(dropdown_title,
//                LV_ALIGN_TOP_MID,
//                0,
//                10);

//   // =====================================
//   // DROPDOWN
//   // =====================================

//   dropdown_control =
//       lv_dropdown_create(dropdown_panel);

//   lv_obj_set_width(dropdown_control,
//                    170);

//   lv_obj_align(dropdown_control,
//                LV_ALIGN_CENTER,
//                0,
//                -10);

//   // =====================================
//   // OK BUTTON
//   // =====================================

//   lv_obj_t *okBtn =
//       lv_button_create(dropdown_panel);

//   style_button(okBtn);

//   lv_obj_set_size(okBtn,
//                   80,
//                   36);

//   lv_obj_align(okBtn,
//                LV_ALIGN_BOTTOM_MID,
//                0,
//                -10);

//   lv_obj_t *okLbl =
//       lv_label_create(okBtn);

//   lv_label_set_text(okLbl,
//                     "OK");

//   lv_obj_center(okLbl);

//   lv_obj_add_event_cb(okBtn,
//                       dropdown_ok_event,
//                       LV_EVENT_CLICKED,
//                       NULL);

//   // hidden initially
//   lv_obj_add_flag(dropdown_modal_bg,
//                   LV_OBJ_FLAG_HIDDEN);
// }






lv_obj_t *dropdown_bg;
lv_obj_t *dropdown_panel;
lv_obj_t *dropdown_title;
lv_obj_t *dropdown;

void create_dropdown_panel()
{
    dropdown_bg = lv_obj_create(lv_screen_active());

    lv_obj_remove_style_all(dropdown_bg);

    lv_obj_set_size(
        dropdown_bg,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    lv_obj_set_style_bg_color(
        dropdown_bg,
        lv_palette_main(LV_PALETTE_GREY),
        0);

    lv_obj_set_style_bg_opa(
        dropdown_bg,
        LV_OPA_60,
        0);

    lv_obj_add_flag(
        dropdown_bg,
        LV_OBJ_FLAG_HIDDEN);

    dropdown_panel =
        lv_obj_create(dropdown_bg);

    lv_obj_set_size(
        dropdown_panel,
        220,
        140);

    lv_obj_center(dropdown_panel);

    dropdown_title =
        lv_label_create(dropdown_panel);

    lv_obj_align(
        dropdown_title,
        LV_ALIGN_TOP_MID,
        0,
        10);

    dropdown =
        lv_dropdown_create(dropdown_panel);

    lv_obj_set_width(
        dropdown,
        180);

    lv_obj_align(
        dropdown,
        LV_ALIGN_CENTER,
        0,
        -10);

    lv_obj_t *okBtn =
        lv_button_create(dropdown_panel);

    style_button(okBtn);


    lv_obj_set_size(
        okBtn,
        80,
        35);

    lv_obj_align(
        okBtn,
        LV_ALIGN_BOTTOM_MID,
        0,
        -10);

    lv_obj_add_event_cb(
        okBtn,
        dropdown_ok_event,
        LV_EVENT_CLICKED,
        NULL);

    lv_obj_t *lbl =
        lv_label_create(okBtn);

    lv_label_set_text(lbl, "OK");

    lv_obj_center(lbl);
}

void show_dropdown_panel(
    DropdownType type)
{
    activeDropdown = type;

    switch(type)
    {
        case DROPDOWN_COUNTDOWN:

            lv_label_set_text(
                dropdown_title,
                "Countdown");

            lv_dropdown_set_options(
                dropdown,
                countdownOptions);

            lv_dropdown_set_selected(
                dropdown,
                countdownIndex);

            break;


        case DROPDOWN_DIRECTION:

            lv_label_set_text(
                dropdown_title,
                "Direction");

            lv_dropdown_set_options(
                dropdown,
                directionOptions);

            lv_dropdown_set_selected(
                dropdown,
                directionIndex);

            break;


        case DROPDOWN_REPEAT:

            lv_label_set_text(
                dropdown_title,
                "Repeat");

            lv_dropdown_set_options(
                dropdown,
                repeatOptions);

            lv_dropdown_set_selected(
                dropdown,
                repeatIndex);

            break;


        case DROPDOWN_NUMLEDS:

            lv_label_set_text(
                dropdown_title,
                "NUM LEDS");

            lv_dropdown_set_options(
                dropdown,
                numLedOptions);

            lv_dropdown_set_selected(
                dropdown,
                numLedsIndex);

            break;

        default:
            return;
    }

    lv_obj_clear_flag(
        dropdown_bg,
        LV_OBJ_FLAG_HIDDEN);

    lv_obj_move_foreground(
        dropdown_bg);
}

void dropdown_ok_event(
    lv_event_t *e)
{
 uint32_t index =
    lv_dropdown_get_selected(
        dropdown);

    switch(activeDropdown)
    {
        case DROPDOWN_COUNTDOWN:

            countdownIndex = index;

            EEPROM.write(3, index);
            EEPROM.commit();

            countdownSeconds =
                countdownValues[index];

            break;

        case DROPDOWN_DIRECTION:

            directionIndex = index;
            EEPROM.write(4, index);
            EEPROM.commit();

            break;

        case DROPDOWN_REPEAT:

            repeatIndex = index;
            EEPROM.write(7, index);
            EEPROM.commit();

            repeat = index+1;
            break;

        case DROPDOWN_NUMLEDS:

            numLedsIndex = index;
            EEPROM.write(8, index);
            EEPROM.commit();

            NUM_LEDS =
                numLedValues[index];

            break;
    }

    // refresh menu values
    draw_menu(currentMenu,
                currentMenuSize,
                lv_label_get_text(title_label));

    lv_obj_add_flag(
        dropdown_bg,
        LV_OBJ_FLAG_HIDDEN);

    activeDropdown =
        DROPDOWN_NONE;
}