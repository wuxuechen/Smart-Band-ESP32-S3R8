#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    {
        lv_obj_t *parent_obj = obj;
        {
            // label_hello
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_hello = obj;
            lv_obj_set_pos(obj, 81, 71);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Hello, world!");
        }
        {
            // btn_hello
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_hello = obj;
            lv_obj_set_pos(obj, 70, 167);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_add_event_cb(obj, action_btn_hello_clicked, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Button");
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_calendar() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.calendar = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_calendar_create(parent_obj);
            lv_obj_set_pos(obj, 5, 32);
            lv_obj_set_size(obj, 230, 240);
            lv_calendar_header_arrow_create(obj);
            lv_calendar_set_today_date(obj, 2022, 11, 1);
            lv_calendar_set_showed_date(obj, 2022, 11);
        }
        {
            // calendar_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.calendar_label = obj;
            lv_obj_set_pos(obj, 88, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Calendar");
        }
    }
    
    tick_screen_calendar();
}

void tick_screen_calendar() {
}

void create_screen_message() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.message = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    {
        lv_obj_t *parent_obj = obj;
        {
            // keyboard
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard = obj;
            lv_obj_set_pos(obj, 0, 119);
            lv_obj_set_size(obj, 240, 161);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // message_textarea
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.message_textarea = obj;
            lv_obj_set_pos(obj, 0, 31);
            lv_obj_set_size(obj, 240, 78);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
        }
        {
            // message_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.message_label = obj;
            lv_obj_set_pos(obj, 44, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Message with Phone");
        }
    }
    lv_keyboard_set_textarea(objects.keyboard, objects.message_textarea);
    
    tick_screen_message();
}

void tick_screen_message() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_calendar,
    tick_screen_message,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_calendar();
    create_screen_message();
}
