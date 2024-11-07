#include "sensorControlDevice.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
extern const char *TAG;
static uint32_t lvgl_keypad_get_key(void);



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
                // act_key = LV_KEY_UP;
                act_key = LV_KEY_PREV;
                break;
            case 2:
                // act_key = LV_KEY_DOWN;
                act_key = LV_KEY_NEXT;
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