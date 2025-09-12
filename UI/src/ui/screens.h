#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *home;
    lv_obj_t *calendar;
    lv_obj_t *feedback;
    lv_obj_t *weather;
    lv_obj_t *activity;
    lv_obj_t *activity_rank;
    lv_obj_t *ota;
    lv_obj_t *setting;
    lv_obj_t *home_date;
    lv_obj_t *home_time;
    lv_obj_t *home_weather;
    lv_obj_t *calendar_label;
    lv_obj_t *keyboard;
    lv_obj_t *feedback_textarea;
    lv_obj_t *feedback_label;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *obj3;
    lv_obj_t *update_btn;
    lv_obj_t *obj4;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_HOME = 1,
    SCREEN_ID_CALENDAR = 2,
    SCREEN_ID_FEEDBACK = 3,
    SCREEN_ID_WEATHER = 4,
    SCREEN_ID_ACTIVITY = 5,
    SCREEN_ID_ACTIVITY_RANK = 6,
    SCREEN_ID_OTA = 7,
    SCREEN_ID_SETTING = 8,
};

void create_screen_home();
void tick_screen_home();

void create_screen_calendar();
void tick_screen_calendar();

void create_screen_feedback();
void tick_screen_feedback();

void create_screen_weather();
void tick_screen_weather();

void create_screen_activity();
void tick_screen_activity();

void create_screen_activity_rank();
void tick_screen_activity_rank();

void create_screen_ota();
void tick_screen_ota();

void create_screen_setting();
void tick_screen_setting();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/