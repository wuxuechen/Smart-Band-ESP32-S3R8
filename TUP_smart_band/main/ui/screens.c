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

static void event_handler_checked_cb_setting_settings_wifi_switch(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_wifi_switch(e);
    }
}

static void event_handler_checked_cb_setting_setttings_bt_switch(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    if (lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        action_bt_switch(e);
    }
}

void create_screen_home() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.home = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xff212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(obj, &img_bg, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // home_date
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.home_date = obj;
            lv_obj_set_pos(obj, 22, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Sep 12 Fr");
        }
        {
            // home_time
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.home_time = obj;
            lv_obj_set_pos(obj, 54, 163);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "04:04:04");
        }
        {
            // home_weather
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.home_weather = obj;
            lv_obj_set_pos(obj, -1, 84);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff6a2ef3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Windy");
        }
        {
            // home_weather_img
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.home_weather_img = obj;
            lv_obj_set_pos(obj, 0, 36);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_weather_small);
        }
    }
    
    tick_screen_home();
}

void tick_screen_home() {
}

void create_screen_calendar() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.calendar = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // calendar_calendar
            lv_obj_t *obj = lv_calendar_create(parent_obj);
            objects.calendar_calendar = obj;
            lv_obj_set_pos(obj, 3, 40);
            lv_obj_set_size(obj, 237, 240);
            lv_calendar_header_arrow_create(obj);
            lv_calendar_set_today_date(obj, 2022, 11, 1);
            lv_calendar_set_showed_date(obj, 2022, 11);
        }
        {
            // calendar_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.calendar_label = obj;
            lv_obj_set_pos(obj, 72, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Calendar");
        }
    }
    
    tick_screen_calendar();
}

void tick_screen_calendar() {
}

void create_screen_feedback() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.feedback = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // keyboard
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard = obj;
            lv_obj_set_pos(obj, 0, 119);
            lv_obj_set_size(obj, 240, 161);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // feedback_textarea
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.feedback_textarea = obj;
            lv_obj_set_pos(obj, 0, 31);
            lv_obj_set_size(obj, 240, 78);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
        }
        {
            // feedback_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.feedback_label = obj;
            lv_obj_set_pos(obj, 38, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "User Feedback");
        }
    }
    lv_keyboard_set_textarea(objects.keyboard, objects.feedback_textarea);
    
    tick_screen_feedback();
}

void tick_screen_feedback() {
}

void create_screen_weather() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.weather = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 72, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Weather");
        }
        {
            // weather_current
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.weather_current = obj;
            lv_obj_set_pos(obj, 0, 24);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_cloud);
        }
        {
            // weather_current_tmp
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.weather_current_tmp = obj;
            lv_obj_set_pos(obj, 145, 24);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_temperature);
        }
        {
            // weather_current_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.weather_current_label = obj;
            lv_obj_set_pos(obj, 0, 79);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfff5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Cloudy");
        }
        {
            // weather_current_tmp_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.weather_current_tmp_label = obj;
            lv_obj_set_pos(obj, 145, 79);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffde960d), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "39°C");
        }
        {
            // weather_today
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.weather_today = obj;
            lv_obj_set_pos(obj, 0, 109);
            lv_obj_set_size(obj, 240, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff25a535), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Today: 39.9 °C-39.9 °C\nPatchy rain nearby");
        }
        {
            // weather_tomorrow
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.weather_tomorrow = obj;
            lv_obj_set_pos(obj, 0, 162);
            lv_obj_set_size(obj, 240, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff25a535), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Tomorrow: 39.9 °C-39.9 °C\nLight rain shower");
        }
        {
            // weather_after
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.weather_after = obj;
            lv_obj_set_pos(obj, 0, 214);
            lv_obj_set_size(obj, 240, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff25a535), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "After:39.9 °C-39.9 °C\nThunderstorm rain nearby");
        }
    }
    
    tick_screen_weather();
}

void tick_screen_weather() {
}

void create_screen_activity() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.activity = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 79, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Activity");
        }
        {
            lv_obj_t *obj = lv_img_create(parent_obj);
            lv_obj_set_pos(obj, 0, 30);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_run);
        }
        {
            // activity_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.activity_label = obj;
            lv_obj_set_pos(obj, 84, 24);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff9fa0ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "    acc     gyr\nx +0.00 +0.00\nx +0.00 +0.00\nz +0.00 +0.00");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 37, 141);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffd562a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Stride ");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj3 = obj;
            lv_obj_set_pos(obj, 12, 183);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffd4e725), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Cadence");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj4 = obj;
            lv_obj_set_pos(obj, 55, 225);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff3c2afd), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Pace");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj5 = obj;
            lv_obj_set_pos(obj, 47, 256);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffd2a75), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Steps");
        }
        {
            // activity_stride
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.activity_stride = obj;
            lv_obj_set_pos(obj, 122, 141);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffd562a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0.00");
        }
        {
            // activity_cadence
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.activity_cadence = obj;
            lv_obj_set_pos(obj, 121, 183);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffd4e725), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0.00");
        }
        {
            // activity_pace
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.activity_pace = obj;
            lv_obj_set_pos(obj, 122, 225);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff3c2afd), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0.00");
        }
        {
            // activity_steps
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.activity_steps = obj;
            lv_obj_set_pos(obj, 122, 256);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffd2a75), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0.00");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj6 = obj;
            lv_obj_set_pos(obj, 156, 122);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffd562a), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "m");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj7 = obj;
            lv_obj_set_pos(obj, 148, 165);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffd4e725), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "steps/min");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj8 = obj;
            lv_obj_set_pos(obj, 156, 207);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff3c2afd), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "min/km");
        }
    }
    
    tick_screen_activity();
}

void tick_screen_activity() {
}

void create_screen_activity_rank() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.activity_rank = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj9 = obj;
            lv_obj_set_pos(obj, 91, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Rank");
        }
    }
    
    tick_screen_activity_rank();
}

void tick_screen_activity_rank() {
}

void create_screen_coach() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.coach = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // coach_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.coach_label = obj;
            lv_obj_set_pos(obj, 82, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_label_set_text(obj, "Squats");
        }
        {
            lv_obj_t *obj = lv_img_create(parent_obj);
            lv_obj_set_pos(obj, 86, 58);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        }
        {
            // coach_img
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.coach_img = obj;
            lv_obj_set_pos(obj, 21, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_squats);
        }
        {
            // coach_last_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.coach_last_btn = obj;
            lv_obj_set_pos(obj, 0, 239);
            lv_obj_set_size(obj, 121, 40);
            lv_obj_add_event_cb(obj, action_coach_last_btn_pressed, LV_EVENT_PRESSED, (void *)0);
            lv_obj_add_state(obj, LV_STATE_DISABLED);
            lv_obj_set_style_bg_img_src(obj, &img_arrow_l, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // coach_next_btn
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.coach_next_btn = obj;
            lv_obj_set_pos(obj, 121, 239);
            lv_obj_set_size(obj, 119, 40);
            lv_obj_add_event_cb(obj, action_coach_next_btn_pressed, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_bg_img_src(obj, &img_arrow_r, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    
    tick_screen_coach();
}

void tick_screen_coach() {
}

void create_screen_setting() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.setting = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 240, 280);
    lv_obj_add_event_cb(obj, action_swipe_event_cb, LV_EVENT_GESTURE, (void *)0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj10 = obj;
            lv_obj_set_pos(obj, 74, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Settings");
        }
        {
            // settings_wifi_switch
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.settings_wifi_switch = obj;
            lv_obj_set_pos(obj, 172, 125);
            lv_obj_set_size(obj, 50, 25);
            lv_obj_add_event_cb(obj, action_wifi_switch, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj11 = obj;
            lv_obj_set_pos(obj, 18, 126);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "WiFi");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj12 = obj;
            lv_obj_set_pos(obj, 18, 171);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Bluetooth");
        }
        {
            // setttings_bt_switch
            lv_obj_t *obj = lv_switch_create(parent_obj);
            objects.setttings_bt_switch = obj;
            lv_obj_set_pos(obj, 172, 171);
            lv_obj_set_size(obj, 50, 25);
            lv_obj_add_event_cb(obj, action_bt_switch, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_add_state(obj, LV_STATE_CHECKED);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.obj13 = obj;
            lv_obj_set_pos(obj, 18, 76);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "CPU °C");
        }
        {
            // setttings_tmp_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setttings_tmp_label = obj;
            lv_obj_set_pos(obj, 172, 76);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0.00");
        }
        {
            // settings_version
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.settings_version = obj;
            lv_obj_set_pos(obj, 77, 206);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "V.25.1.01");
        }
        {
            // settings_update
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.settings_update = obj;
            lv_obj_set_pos(obj, 97, 230);
            lv_obj_set_size(obj, 47, 50);
            lv_obj_set_style_bg_img_src(obj, &img_update, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0f0552), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // setttings_username
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.setttings_username = obj;
            lv_obj_set_pos(obj, 81, 32);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xfffcff00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Joseph");
        }
    }
    
    tick_screen_setting();
}

void tick_screen_setting() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_home,
    tick_screen_calendar,
    tick_screen_feedback,
    tick_screen_weather,
    tick_screen_activity,
    tick_screen_activity_rank,
    tick_screen_coach,
    tick_screen_setting,
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
    
    create_screen_home();
    create_screen_calendar();
    create_screen_feedback();
    create_screen_weather();
    create_screen_activity();
    create_screen_activity_rank();
    create_screen_coach();
    create_screen_setting();
}
