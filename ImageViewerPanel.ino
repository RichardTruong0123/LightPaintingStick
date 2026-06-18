lv_obj_t *image_viewer_bg = nullptr;
lv_obj_t *image_viewer_panel = nullptr;
lv_obj_t *image_viewer_image = nullptr;

lv_image_dsc_t *currentBmpImage = nullptr;

void create_image_viewer_panel()
{
    image_viewer_bg =
        lv_obj_create(lv_screen_active());

    lv_obj_remove_style_all(
        image_viewer_bg);

    lv_obj_set_size(
        image_viewer_bg,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    lv_obj_set_style_bg_color(
        image_viewer_bg,
        lv_color_black(),
        0);

    lv_obj_set_style_bg_opa(
        image_viewer_bg,
        LV_OPA_60,
        0);

    lv_obj_add_flag(
        image_viewer_bg,
        LV_OBJ_FLAG_HIDDEN);

    image_viewer_panel =
        lv_obj_create(
            image_viewer_bg);

    lv_obj_set_size(
        image_viewer_panel,
        SCREEN_WIDTH - 10,
        SCREEN_HEIGHT - 20);

    lv_obj_center(
        image_viewer_panel);

    lv_obj_set_scroll_dir(
        image_viewer_panel,
      (lv_dir_t)(LV_DIR_VER | LV_DIR_HOR));

    lv_obj_set_scrollbar_mode(
        image_viewer_panel,
        LV_SCROLLBAR_MODE_AUTO);
}

void hide_image_viewer()
{
    lv_obj_add_flag(
        image_viewer_bg,
        LV_OBJ_FLAG_HIDDEN);

    if (image_viewer_image)
    {
        lv_obj_delete(
            image_viewer_image);

        image_viewer_image = nullptr;
    }

    if (currentBmpImage)
    {
        free(
            (void*)currentBmpImage->data);

        delete currentBmpImage;

        currentBmpImage = nullptr;
    }
}

static void image_viewer_click_event(
    lv_event_t *e)
{
    hide_image_viewer();
}

void show_image_viewer(
    const char *filePath)
{
    lv_obj_clear_flag(
        image_viewer_bg,
        LV_OBJ_FLAG_HIDDEN);

    lv_obj_move_foreground(
        image_viewer_bg);

    // currentBmpImage =
    //     loadBmpToLvgl(
    //         filePath);

    if (!currentBmpImage)
    {
        hide_image_viewer();
        return;
    }

    image_viewer_image =
        lv_image_create(
            image_viewer_panel);

    lv_image_set_src(
        image_viewer_image,
        currentBmpImage);

    lv_obj_set_pos(
        image_viewer_image,
        0,
        0);

    lv_obj_add_flag(
        image_viewer_image,
        LV_OBJ_FLAG_CLICKABLE);

    lv_obj_add_event_cb(
        image_viewer_image,
        image_viewer_click_event,
        LV_EVENT_CLICKED,
        NULL);
}

