#include "sensorControlDevice.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_touch_xpt2046.h"

extern const char *TAG;
static uint32_t lvgl_keypad_get_key(void);

lv_indev_t *keypad = NULL;
lv_indev_t *touch_screen = NULL;

void lvgl_keypad_init(void)
{
    gpio_config_t keypad_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = MAP_PAD,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&keypad_conf));
}

static uint32_t lvgl_keypad_get_key(void)
{
    uint32_t key_state =  0;
    if (!gpio_get_level(PIN_PAD_UP))
        key_state = 1;
    else if (!gpio_get_level(PIN_PAD_DOWN))
        key_state = 2;
    else if (!gpio_get_level(PIN_PAD_LEFT))
        key_state = 3;
    else if (!gpio_get_level(PIN_PAD_RIGHT))
        key_state = 4;
    else if (!gpio_get_level(PIN_PAD_ENTER))
        key_state = 5;

    return key_state;
}

void lvgl_keypad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = lvgl_keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PRESSED;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
            case 1:
                act_key = LV_KEY_UP;
                break;
            case 2:
                act_key = LV_KEY_DOWN;
                break;
            case 3:
                act_key = LV_KEY_LEFT;
                break;
            case 4:
                act_key = LV_KEY_RIGHT;
                break;
            case 5:
                act_key = LV_KEY_ENTER;
                break;
        }

        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->key = last_key;
}

void lvgl_touch_cb(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;

    esp_lcd_touch_handle_t touch_pad = lv_indev_get_user_data(indev_drv);
    ESP_ERROR_CHECK(esp_lcd_touch_read_data(touch_pad));

    /* Get coordinates */
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(touch_pad, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);

    if (touchpad_pressed && touchpad_cnt > 0) {
        data->point.x = touchpad_x[0];
        data->point.y = touchpad_y[0];
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

