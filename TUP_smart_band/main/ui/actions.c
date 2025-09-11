#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>
#include <stdio.h>
#include "core/lv_disp.h"
#include "core/lv_event.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "misc/lv_area.h"
#include "screens.h"

#ifdef __cplusplus
extern "C" {
#endif

void action_btn_hello_clicked(lv_event_t *e) {
	lv_event_code_t code = lv_event_get_code(e);
	printf("Button was clicked!!!!!!!!!!!!!!%d\n", code);
    
    if(code == LV_EVENT_CLICKED) {
        lv_label_set_text(objects.label_hello, "Text changed!");
    }
    if(code == LV_EVENT_PRESSED){
		lv_label_set_text(objects.label_hello, "pressed!");
	}
}

void action_swipe_event_cb(lv_event_t * e){
	lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
	lv_obj_t *current = lv_scr_act(); 
	if(current == objects.main){
		if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.calendar, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
	}
	if(current == objects.calendar){
		if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.message, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
	}
	if(current == objects.message){
		if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	if(dir == LV_DIR_RIGHT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.calendar, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    	}
	}
    
}

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/