#ifndef SENSORCONTROL_H
#define SENSORCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


#define LCD_HOST  SPI2_HOST

#define LCD_PIXEL_CLOCK_HZ     (50 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define PIN_NUM_SCLK           18
#define PIN_NUM_MOSI           23
#define PIN_NUM_MISO           19
#define PIN_NUM_LCD_DC         5
#define PIN_NUM_LCD_RST        21
#define PIN_NUM_LCD_CS         4
#define PIN_NUM_TOUCH_CS       15 
#define PIN_NUM_BK_LIGHT       2

#define LCD_H_RES              240
#define LCD_V_RES              320

#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8

#define LVGL_DRAW_BUF_LINES    20 
#define LVGL_TICK_PERIOD_MS    2
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1
#define LVGL_TASK_STACK_SIZE   (8 * 1024)
#define LVGL_TASK_PRIORITY     2

#define LVGL_ANIM_DELAY        200

void sensorControl(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
