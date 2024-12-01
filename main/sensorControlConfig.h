#ifndef SENSOR_CONTROL_CONFIG
#define SENSOR_CONTROL_CONFIG

#include "lvgl.h"

//Setting LCD moduke
#define LCD_HOST               SPI2_HOST
#define LCD_PIXEL_CLOCK_HZ     (50 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL

//Setting LCD pins
#define PIN_NUM_SCLK           18
#define PIN_NUM_MOSI           23
#define PIN_NUM_MISO           19
#define PIN_NUM_LCD_DC         5
#define PIN_NUM_LCD_RST        21
#define PIN_NUM_LCD_CS         4
#define PIN_NUM_TOUCH_CS       15 
#define PIN_NUM_BK_LIGHT       2

//Setting LCD resolution
#define LCD_H_RES              240
#define LCD_V_RES              320

#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8

//LVGL config
#define LVGL_DRAW_BUF_LINES    20 
#define LVGL_TICK_PERIOD_MS    2
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1
#define LVGL_TASK_STACK_SIZE   (8 * 1024)
#define LVGL_TASK_PRIORITY     2

#define LVGL_ANIM_DELAY        200

//Sensor config
#define DHT11_TASK_STACK_SIZE   2048
#define MMA8451_TASK_STACK_SIZE 2048
#define DHT11_TASK_PRIORITY       1
#define MMA8451_TASK_PRIORITY     1
#define DHT11_READ_RATE_MS      1000
#define MMA8451_I2C_PORT          0

//Setting keypad pins
#define PIN_PAD_UP      GPIO_NUM_32
#define PIN_PAD_RIGHT   GPIO_NUM_33
#define PIN_PAD_DOWN    GPIO_NUM_25
#define PIN_PAD_LEFT    GPIO_NUM_26
#define PIN_PAD_ENTER   GPIO_NUM_27
#define MAP_PAD         ((1ULL << PIN_PAD_UP) | (1ULL << PIN_PAD_RIGHT) | (1ULL << PIN_PAD_DOWN) | (1ULL << PIN_PAD_LEFT) | (1ULL << PIN_PAD_ENTER))

//Sensor pins
#define DHT11_SENSOR_PIN    GPIO_NUM_14
#define MMA8451_SDA_PIN     GPIO_NUM_16
#define MMA8451_SCL_PIN     GPIO_NUM_17

//Setting Active indev
#define USE_KEYPAD      1
#define USE_TOUCHSCREEN 0


#endif