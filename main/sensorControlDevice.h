#ifndef SENSOR_CONTROL_DEVICE_H
#define SENSOR_CONTROL_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#define PIN_PAD_UP      GPIO_NUM_32
#define PIN_PAD_RIGHT   GPIO_NUM_33
#define PIN_PAD_DOWN    GPIO_NUM_25
#define PIN_PAD_LEFT    GPIO_NUM_26
#define PIN_PAD_ENTER   GPIO_NUM_27
#define MAP_PAD         ((1ULL << PIN_PAD_UP) | (1ULL << PIN_PAD_RIGHT) | (1ULL << PIN_PAD_DOWN) | (1ULL << PIN_PAD_LEFT) | (1ULL << PIN_PAD_ENTER))

void lvgl_keypad_init(void);
void lvgl_keypad_read(lv_indev_t * indev_drv, lv_indev_data_t * data);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif