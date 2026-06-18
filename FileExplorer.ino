#include <vector>
#include <algorithm>

struct FileEntry {
  String name;
  bool isDirectory;
};


static std::vector<FileEntry> entries;
bool longPressHandled = false;
int compareFileEntries(const void *a,
                       const void *b) {

  const FileEntry *fa =
      (const FileEntry *)a;

  const FileEntry *fb =
      (const FileEntry *)b;

  // Directories first
  if (fa->isDirectory != fb->isDirectory) {

    return fb->isDirectory -
           fa->isDirectory;
  }

  // Alphabetical
  return strcasecmp(fa->name.c_str(),
                    fb->name.c_str());
}

void file_item_event(lv_event_t *e) {

  if (longPressHandled)
  {
      longPressHandled = false;
      return;
  }
  
  lv_obj_t *btn =
      (lv_obj_t *)lv_event_get_target(e);

  lv_obj_t *label =
      lv_obj_get_child(btn, 1);

  const char *name =
      lv_label_get_text(label);

  // =====================================
  // PARENT DIRECTORY
  // =====================================

  if (strcmp(name, "..") == 0) {

    int pos =
      currentPath.lastIndexOf('/');

    if (pos > 0) {

      currentPath =
        currentPath.substring(0, pos);
    }
    else {

      currentPath = "/";
    }

    populate_file_list(
      currentPath.c_str());

    return;
  }

  String fullPath;

  if (currentPath == "/") {

    fullPath = "/" + String(name);
  }
  else {

    fullPath =
      currentPath +
      "/" +
      String(name);
  }

  File f =
      SD.open(fullPath.c_str());

  if (!f) {

    Serial.printf("Cannot open %s\n",
                  fullPath.c_str());

    return;
  } 

  // =====================================
  // DIRECTORY
  // =====================================

  if (f.isDirectory()) {

    f.close();

    populate_file_list(
      fullPath.c_str());

    return;
  }

  // =====================================
  // FILE
  // =====================================
  show_message_panel(
      "Loading...");

  Serial.printf("Selected file: %s\n",
                fullPath.c_str());

  lv_label_set_text_fmt(status_label,
                        "File: %s",
                        name);

  f.close();

  // TODO:
  // load pattern file
   bmp_filename = fullPath.c_str();

  if (loadBmpFrameBuffer(bmp_filename))
    selectedPattern = PATTERN_FILE;
  else {
      show_message_panel(
      "Loading... \nUnsuccessful.");
      delay(1500); //display unsuccessful for 1.5 seconds
      selectedPattern = PATTERN_NONE;

  }

  hide_message_panel();

  //after file is selected, hide browser
  hide_file_browser();

}

void file_item_long_press_event(lv_event_t *e)
{
    Serial.println("LONG PRESS DETECTED");
    longPressHandled = true;
    lv_obj_t *btn =
        (lv_obj_t *)lv_event_get_target(e);

    lv_obj_t *label =
        lv_obj_get_child(btn, 1);

    const char *name =
        lv_label_get_text(label);

    String fullPath;

    if (currentPath == "/")
        fullPath = "/" + String(name);
    else
        fullPath = currentPath + "/" + String(name);

    File f = SD.open(fullPath);

    if (!f) {
        Serial.printf("FAILED TO OPEN: %s\n",
                fullPath.c_str());
        return;
    }
    Serial.println("FILE OPENED");
    if (f.isDirectory()) {
        f.close();
        return;
    }

    f.close();

    String lower = fullPath;
    lower.toLowerCase();

    if (!lower.endsWith(".bmp"))
        return;

    show_bmp_preview(fullPath.c_str());
   // show_image_viewer(fullPath.c_str());
    while (bmpPreviewActive) {

          if (readTouch())
          {
              Serial.println("Exit Preview");

              bmpPreviewActive = false;

              lv_obj_clear_flag(file_browser_bg,
                                LV_OBJ_FLAG_HIDDEN);

              lv_timer_handler();
          }

    }
}

// void file_item_long_press_event(lv_event_t *e)
// {
//   const char *path = ...;

//   if (String(path).endsWith(".bmp"))
//   {
//     show_bmp_preview(path);
//   }
// }

void show_file_browser() {

  //fileBrowserVisible = true;
  populate_file_list("/");
  lv_obj_clear_flag(file_browser_bg,
                    LV_OBJ_FLAG_HIDDEN);

  lv_obj_move_foreground(file_browser_bg);
}

void hide_file_browser() {

  fileBrowserVisible = false;

  lv_obj_add_flag(file_browser_bg,
                  LV_OBJ_FLAG_HIDDEN);
}

void file_browser_touch_event(lv_event_t *e) {

  hide_file_browser();
}

void create_file_browser() {

  // =====================================
  // OVERLAY
  // =====================================

  file_browser_bg =
      lv_obj_create(lv_screen_active());

  lv_obj_remove_style_all(file_browser_bg);

  lv_obj_set_size(file_browser_bg,
                  SCREEN_WIDTH,
                  SCREEN_HEIGHT);

  lv_obj_set_style_bg_color(file_browser_bg,
                            lv_color_black(),
                            0);

  lv_obj_set_style_bg_opa(file_browser_bg,
                          LV_OPA_COVER,
                          0);

  lv_obj_add_event_cb(file_browser_bg,
                      file_browser_touch_event,
                      LV_EVENT_LONG_PRESSED,
                      NULL);
  // =====================================
  // FILE BROWSER CONTAINER
  // =====================================

  file_browser_container =
      lv_obj_create(file_browser_bg);

  lv_obj_remove_style_all(file_browser_container);

  lv_obj_set_size(file_browser_container,
                  SCREEN_WIDTH,
                  SCREEN_HEIGHT);

  lv_obj_align(file_browser_container,
               LV_ALIGN_TOP_LEFT,
               0,
               0);

  lv_obj_set_style_bg_color(file_browser_container,
                            lv_color_black(),
                            0);

  lv_obj_set_style_bg_opa(file_browser_container,
                          LV_OPA_COVER,
                          0);

  // =====================================
  // FILE LIST
  // =====================================

  file_list =
      lv_list_create(file_browser_container);

  lv_obj_set_size(file_list,
                  SCREEN_WIDTH,
                  SCREEN_HEIGHT);

  lv_obj_align(file_list,
               LV_ALIGN_TOP_LEFT,
               0,
               0);

  lv_obj_set_style_radius(file_list,
                          0,
                          0);

  lv_obj_set_style_border_width(file_list,
                                0,
                                0);

  lv_obj_set_style_pad_all(file_list,
                           0,
                           0);

  lv_obj_set_scroll_dir(file_list,
                        LV_DIR_VER);

  lv_obj_add_flag(file_list,
                  LV_OBJ_FLAG_SCROLL_MOMENTUM);

  lv_obj_add_flag(file_list,
                  LV_OBJ_FLAG_SCROLL_ELASTIC);

  // =====================================
  // LOAD FILES FROM SD CARD
  // =====================================

  File root = SD.open("/");

  if (root) {

    File file = root.openNextFile();

    while (file) {

      if (!file.isDirectory()) {

        lv_obj_t *btn =
            lv_list_add_button(
                file_list,
                LV_SYMBOL_FILE,
                file.name());

        lv_obj_add_event_cb(btn,
                            file_item_event,
                            LV_EVENT_CLICKED,
                            NULL);
      }

      file = root.openNextFile();
    }

    root.close();
  }

  // hidden initially
  lv_obj_add_flag(file_browser_bg,
                  LV_OBJ_FLAG_HIDDEN);
}

void populate_file_list(const char *path) {

  currentPath = path;

  lv_obj_clean(file_list);

  File dir = SD.open(path);

  if (!dir || !dir.isDirectory()) {

    Serial.printf("Cannot open: %s\n",
                  path);

    return;
  }

  // =====================================
  // PARENT DIRECTORY
  // =====================================

  if (strcmp(path, "/") != 0) {

    lv_obj_t *btn =
      lv_list_add_button(file_list,
                         LV_SYMBOL_LEFT,
                         "..");

    lv_obj_add_event_cb(btn,
                        file_item_event,
                        LV_EVENT_CLICKED,
                        NULL);
  }

  // =====================================
  // DIRECTORY CONTENTS
  // =====================================
  entries.clear();

  File file = dir.openNextFile();

  while (file)
  {
      String name = file.name();

      if (file.isDirectory())
      {
          entries.push_back({
              name,
              true
          });
      }
      else
      {
          String lowerName = name;
          lowerName.toLowerCase();

          if (lowerName.endsWith(".bmp"))
          {
              entries.push_back({
                  name,
                  false
              });
          }
      }

      file = dir.openNextFile();
  }

  std::sort(
      entries.begin(),
      entries.end(),
      [](const FileEntry& a,
        const FileEntry& b)
      {
        // Directories first
        if (a.isDirectory != b.isDirectory)
          return a.isDirectory > b.isDirectory;

        // Alphabetical
        return strcasecmp(
                  a.name.c_str(),
                  b.name.c_str()) < 0;
      });

  for (const auto& entry : entries) {

    lv_obj_t *btn;

    if (entry.isDirectory) {

      btn = lv_list_add_button(
          file_list,
          LV_SYMBOL_DIRECTORY,
          entry.name.c_str());
    }
    else {

      btn = lv_list_add_button(
          file_list,
          LV_SYMBOL_FILE,
          entry.name.c_str());
    }

lv_obj_add_event_cb(btn,
                    file_item_event,
                    LV_EVENT_CLICKED,
                    NULL);

lv_obj_add_event_cb(btn,
                    file_item_long_press_event,
                    LV_EVENT_LONG_PRESSED,
                    NULL);
  }
  dir.close();

  lv_label_set_text_fmt(status_label,
                        "%s",
                        path);
}


void create_bmp_preview()
{
    bmp_preview_bg =
        lv_obj_create(lv_screen_active());

    lv_obj_remove_style_all(bmp_preview_bg);

    lv_obj_set_size(bmp_preview_bg,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT);

    lv_obj_set_style_bg_color(
        bmp_preview_bg,
        lv_color_black(),
        0);

    lv_obj_set_style_bg_opa(
        bmp_preview_bg,
        LV_OPA_COVER,
        0);

    bmp_preview_img =
        lv_image_create(bmp_preview_bg);

    lv_obj_center(bmp_preview_img);

    lv_obj_add_flag(
        bmp_preview_bg,
        LV_OBJ_FLAG_HIDDEN);

    // tap preview to close
    lv_obj_add_event_cb(
        bmp_preview_bg,
        bmp_preview_close_event,
        LV_EVENT_CLICKED,
        NULL);
}

void show_bmp_preview(const char *filename)
{
    Serial.printf("Preview: %s\n",
                  filename);
                  
    bmpPreviewActive = true;

    lv_obj_add_flag(
        file_browser_bg,
        LV_OBJ_FLAG_HIDDEN);

    tft.fillScreen(TFT_BLACK);

    drawBmp(filename, 0, 0);
}

// void show_bmp_preview_lvgl(const char *path)
// {
//   currentScreen = SCREEN_BMP_PREVIEW;

//   lv_scr_load(bmp_screen);

//   loadBmpToLVGL(path);
// }

void bmp_preview_close_event(lv_event_t *e)
{
    lv_obj_add_flag(
        bmp_preview_bg,
        LV_OBJ_FLAG_HIDDEN);

    draw_menu(currentMenu,
              currentMenuSize,
              lv_label_get_text(title_label));
}

