#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_bg;
extern const lv_img_dsc_t img_run;
extern const lv_img_dsc_t img_update;
extern const lv_img_dsc_t img_weather_small;
extern const lv_img_dsc_t img_squats;
extern const lv_img_dsc_t img_pushups;
extern const lv_img_dsc_t img_lunges;
extern const lv_img_dsc_t img_burbees;
extern const lv_img_dsc_t img_mountain_climb;
extern const lv_img_dsc_t img_jump_rope;
extern const lv_img_dsc_t img_glute_bridges;
extern const lv_img_dsc_t img_superman;
extern const lv_img_dsc_t img_plankholds;
extern const lv_img_dsc_t img_alternating_bird_dogs;
extern const lv_img_dsc_t img_cloud;
extern const lv_img_dsc_t img_rain;
extern const lv_img_dsc_t img_sun;
extern const lv_img_dsc_t img_temperature;
extern const lv_img_dsc_t img_arrow_l;
extern const lv_img_dsc_t img_arrow_r;
extern const lv_img_dsc_t img_u1;
extern const lv_img_dsc_t img_u2;
extern const lv_img_dsc_t img_u3;
extern const lv_img_dsc_t img_u4;
extern const lv_img_dsc_t img_u5;
extern const lv_img_dsc_t img_u6;
extern const lv_img_dsc_t img_u7;
extern const lv_img_dsc_t img_u8;
extern const lv_img_dsc_t img_u9;
extern const lv_img_dsc_t img_u10;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[30];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/
