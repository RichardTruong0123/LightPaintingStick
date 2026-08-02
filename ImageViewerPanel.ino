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

    lv_obj_set_style_bg_color(
        image_viewer_panel,
        lv_color_black(),
        0);    
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

    if (currentBmpImage)  //free previous image from memory
    {
        free(
            (void *)currentBmpImage->data);

        delete currentBmpImage;

        currentBmpImage = nullptr;
    }

    currentBmpImage =
        loadBmpToLvgl(
            filePath);

    if (!currentBmpImage)
    {
        Serial.println("BMP load failed");
        return;
    }

    Serial.printf("W=%d H=%d\n",
                currentBmpImage->header.w,
                currentBmpImage->header.h);
    Serial.printf("Size %u\n",
                currentBmpImage->data_size);

    if (!currentBmpImage)
    {
        hide_image_viewer();
        return;
    }
    
    Serial.println("image viewer");

    image_viewer_image =
        lv_image_create(
            image_viewer_panel);

    lv_image_set_src(
        image_viewer_image,
        currentBmpImage);

    Serial.printf("Object size: %d x %d\n",
                lv_obj_get_width(image_viewer_image),
                lv_obj_get_height(image_viewer_image));

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


lv_image_dsc_t *
loadBmpToLvgl(const char *path)
{
    File bmp = SD.open(path);

    if (!bmp)
    {
        Serial.println("BMP open failed");
        return nullptr;
    }

    if (read16(bmp) != 0x4D42)
    {
        bmp.close();
        return nullptr;
    }

    read32(bmp);
    read32(bmp);

    uint32_t imageOffset =
        read32(bmp);

    read32(bmp);

    int32_t width =
        read32(bmp);

    int32_t height =
        read32(bmp);

    if (read16(bmp) != 1)
    {
        bmp.close();
        return nullptr;
    }

    uint16_t bpp =
        read16(bmp);

    uint32_t compression =
        read32(bmp);

    if (bpp != 24 ||
        compression != 0)
    {
        Serial.println(
            "Only 24-bit BMP supported");

        bmp.close();
        return nullptr;
    }

    bool flip = true;

    if (height < 0)
    {
        height = -height;
        flip = false;
    }

    uint32_t pixels =
        width * height;

    uint16_t *buffer =
        (uint16_t *)ps_malloc(
            pixels * 2);

Serial.printf("Free PSRAM = %u\n", ESP.getFreePsram());

    if (!buffer)
    {
        Serial.println(
            "Out of PSRAM");

        bmp.close();
        return nullptr;
    }

    uint32_t rowSize =
        ((width * 3 + 3) & ~3);

    uint8_t *row =
        (uint8_t *)malloc(rowSize);

    bmp.seek(imageOffset);

    for (int y = 0; y < height; y++)
    {
        bmp.read(row, rowSize);

        int dstY =
            flip ?
            (height - y - 1)
            : y;

        for (int x = 0; x < width; x++)
        {
            uint8_t b =
                row[x * 3];

            uint8_t g =
                row[x * 3 + 1];

            uint8_t r =
                row[x * 3 + 2];

            buffer[
                dstY * width + x]
                =
                ((r & 0xF8) << 8) |
                ((g & 0xFC) << 3) |
                (b >> 3);
        }
    }

    free(row);

    bmp.close();

    lv_image_dsc_t *img =
        new lv_image_dsc_t;

    img->header.magic =
        LV_IMAGE_HEADER_MAGIC;

    img->header.cf =
        LV_COLOR_FORMAT_RGB565;
    
    img->header.flags = 0;
    
    img->header.w =
        width;

    img->header.h =
        height;

    img->data_size =
        pixels * 2;

    img->data =
        (const uint8_t *)buffer;

    return img;
}