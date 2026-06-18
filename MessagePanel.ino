lv_obj_t *message_bg = nullptr;
lv_obj_t *message_panel = nullptr;
lv_obj_t *message_label = nullptr;

void create_message_panel()
{
    // Greyed-out background
    message_bg = lv_obj_create(lv_screen_active());

    lv_obj_remove_style_all(message_bg);

    lv_obj_set_size(
        message_bg,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    lv_obj_set_style_bg_color(
        message_bg,
        lv_color_black(),
        0);

    lv_obj_set_style_bg_opa(
        message_bg,
        LV_OPA_50,
        0);

    lv_obj_add_flag(
        message_bg,
        LV_OBJ_FLAG_HIDDEN);

    // Center panel
    message_panel =
        lv_obj_create(message_bg);

    lv_obj_set_size(
        message_panel,
        180,
        80);

    lv_obj_center(message_panel);

    // Message label
    message_label =
        lv_label_create(message_panel);

    lv_label_set_text(
        message_label,
        "Loading...");

    lv_obj_center(message_label);
}

void show_message_panel(
    const char *message)
{
    lv_label_set_text(
        message_label,
        message);

    lv_obj_clear_flag(
        message_bg,
        LV_OBJ_FLAG_HIDDEN);

    lv_obj_move_foreground(
        message_bg);

    //lv_timer_handler();   // force immediate draw
    lv_refr_now(NULL);    // LVGL 9
}

void hide_message_panel()
{
    lv_obj_add_flag(
        message_bg,
        LV_OBJ_FLAG_HIDDEN);
}

