#ifndef SENSOR_CONTROL_DEVICE_H
#define SENSOR_CONTROL_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sensorControlConfig.h"

extern lv_indev_t *keypad;
extern lv_indev_t *touch_screen;

void lvgl_keypad_init(void);
void lvgl_keypad_read(lv_indev_t * indev_drv, lv_indev_data_t * data);
void lvgl_touch_cb(lv_indev_t * indev_drv, lv_indev_data_t * data);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif