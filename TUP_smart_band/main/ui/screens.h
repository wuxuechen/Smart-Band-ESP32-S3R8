#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *calendar;
    lv_obj_t *message;
    lv_obj_t *label_hello;
    lv_obj_t *btn_hello;
    lv_obj_t *calendar_label;
    lv_obj_t *keyboard;
    lv_obj_t *message_textarea;
    lv_obj_t *message_label;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_CALENDAR = 2,
    SCREEN_ID_MESSAGE = 3,
};

void create_screen_main();
void tick_screen_main();

void create_screen_calendar();
void tick_screen_calendar();

void create_screen_message();
void tick_screen_message();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/
