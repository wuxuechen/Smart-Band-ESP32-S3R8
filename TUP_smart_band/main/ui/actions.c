#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>
#include <stdbool.h>
#include <stdio.h>
#include "core/lv_disp.h"
#include "core/lv_event.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "misc/lv_area.h"
#include "screens.h"
#include "widgets/lv_label.h"
#include "wifi_connect.h"
#include "gatt_server_service_table.h"
#include "ui/images.h"

#ifdef __cplusplus
extern "C" {
#endif

static int cursor_sports = 0;    
const ext_img_desc_t sports_images[10] = {
    { "Squats", &img_squats },
    { "Pushups", &img_pushups },
    { "Lunges", &img_lunges },
    { "Burbees", &img_burbees },
    { "Mountain Climb", &img_mountain_climb },
    { "Jump Rope", &img_jump_rope },
    { "Glute Bridges", &img_glute_bridges },
    { "Superman", &img_superman },
    { "Plank Holds", &img_plankholds },
    { "Alternate Bird-Dog", &img_alternating_bird_dogs },
};

void action_btn_hello_clicked(lv_event_t *e) {
}

void set_coach(int i){
	if(i==0){
		lv_obj_add_state(objects.coach_last_btn, LV_STATE_DISABLED);   // Disable
		lv_obj_clear_state(objects.coach_next_btn, LV_STATE_DISABLED); // Enable again
	}else if(i==9){
		lv_obj_add_state(objects.coach_next_btn, LV_STATE_DISABLED);   // Disable
		lv_obj_clear_state(objects.coach_last_btn, LV_STATE_DISABLED); // Enable again
	}else{
		lv_obj_clear_state(objects.coach_last_btn, LV_STATE_DISABLED); // Enable again
		lv_obj_clear_state(objects.coach_next_btn, LV_STATE_DISABLED); // Enable again
	}
	lv_label_set_text(objects.coach_label, sports_images[i].name);
	lv_obj_align(objects.coach_label, LV_ALIGN_TOP_MID, 0, 2);
	lv_img_set_src(objects.coach_img, sports_images[i].img_dsc);
}

extern void action_coach_last_btn_pressed(lv_event_t * e){
	if(cursor_sports <= 0){
		return;
	}
	cursor_sports--;
	set_coach(cursor_sports);
}
extern void action_coach_next_btn_pressed(lv_event_t * e){
	if(cursor_sports >= 9){
		return;
	}
	cursor_sports++;
	set_coach(cursor_sports);
}

extern void action_bt_switch(lv_event_t * e){
	lv_obj_t *ta = lv_event_get_target(e);
    if (lv_obj_has_state(ta, LV_STATE_CHECKED)) {
        ESP_LOGI("BT", "Switch ON, Enable bluetooth");
        bt_switch(true);
    }else{
		ESP_LOGI("BT", "Switch OFF, Disable bluetooth");
		bt_switch(false);
	}
}


extern void action_wifi_switch(lv_event_t * e) {
    lv_obj_t * sw = lv_event_get_target(e);

    wifi_cmd_t cmd = lv_obj_has_state(sw, LV_STATE_CHECKED) ?
                        WIFI_CMD_ENABLE : WIFI_CMD_DISABLE;

    // just post message, return immediately
    xQueueSend(wifi_cmd_queue, &cmd, 0);

    ESP_LOGI("LVGL", "Switch toggled, queued WiFi action");
}

void action_swipe_event_cb(lv_event_t * e){
	lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
	lv_obj_t *current = lv_scr_act(); 
	if(current == objects.home){
		if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.activity, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
    	if(dir == LV_DIR_RIGHT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.calendar, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    	}
    	if(dir == LV_DIR_BOTTOM) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.setting, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, false);
    	}
    	
	}
	
	if(current == objects.setting){
    	if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.home, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	
	}
	
	if(current == objects.calendar){
    	if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.activity, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
    	if(dir == LV_DIR_RIGHT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.coach, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    	}
    	if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.home, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	if(dir == LV_DIR_BOTTOM) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.setting, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, false);
    	}
	}
	
	if(current == objects.activity){

    	if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.activity_rank, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
    	if(dir == LV_DIR_RIGHT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.calendar, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    	}
    	if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.home, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	if(dir == LV_DIR_BOTTOM) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.setting, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, false);
    	}
	}

	if(current == objects.activity_rank){

    	if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.weather, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
    	if(dir == LV_DIR_RIGHT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.activity, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    	}
    	if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.home, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	if(dir == LV_DIR_BOTTOM) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.setting, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, false);
    	}
	}
	
	if(current == objects.weather){
    	if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.feedback, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
    	if(dir == LV_DIR_RIGHT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.activity_rank, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    	}
    	if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.home, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	if(dir == LV_DIR_BOTTOM) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.setting, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, false);
    	}
	}
	if(current == objects.feedback){
    	if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.coach, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
    	if(dir == LV_DIR_RIGHT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.activity_rank, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    	}
    	if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.home, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	if(dir == LV_DIR_BOTTOM) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.setting, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, false);
    	}
	}

	if(current == objects.coach){
    	if(dir == LV_DIR_LEFT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.calendar, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    	}
    	if(dir == LV_DIR_RIGHT) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.feedback, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
    	}
    	if(dir == LV_DIR_TOP) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.home, LV_SCR_LOAD_ANIM_OVER_TOP, 300, 0, false);
    	}
    	if(dir == LV_DIR_BOTTOM) {
        // Load Calendar screen with animation
        lv_scr_load_anim(objects.setting, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, false);
    	}
	}
}

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/