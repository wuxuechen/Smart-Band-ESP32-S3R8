#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"

#ifdef __cplusplus
extern "C" {
#endif

void action_btn_hello_clicked(lv_event_t *e) {
	printf("%s", "Button was clicked!!!!!!!!!!!!!!\n");
	 ESP_LOGD("button:", "Text c1");
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
    if(code == LV_EVENT_CLICKED) {
        
        lv_label_set_text(label, "Text changed!");
    }
    lv_label_set_text(label, "Text c2");
    ESP_LOGD("button:", "Text c2");
}

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/