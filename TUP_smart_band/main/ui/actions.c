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
        lv_scr_load_anim(objects.ota, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
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
        lv_scr_load_anim(objects.ota, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
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

	if(current == objects.ota){
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