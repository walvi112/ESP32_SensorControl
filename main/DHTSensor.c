#include <stdio.h>
#include "rom/ets_sys.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "DHTSensor.h"

#define CHECK_TIMEOUT(x)  if(!x) return false;

extern const char *TAG;
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

static bool waitState(DHT11Struct *dht11, uint8_t state, uint32_t delayUs)
{
    size_t count = 0;
    while(gpio_get_level(dht11->gpio_pin) != state)
    {
        if (count >= delayUs)
        {
            taskEXIT_CRITICAL(&mux);
            ESP_LOGE(TAG, "Read DHT11 timeout");
            return false;
        }
        ets_delay_us(2);
        count += 2;
    }
    return true;
}

void DHT_Init(DHT11Struct *dht11)
{
    dht11->SensorConf->pin_bit_mask = (1ULL << (dht11->gpio_pin)),
    dht11->SensorConf->mode = GPIO_MODE_OUTPUT_OD;
    dht11->SensorConf->pull_down_en = GPIO_PULLDOWN_DISABLE;
    dht11->SensorConf->pull_up_en = GPIO_PULLUP_DISABLE;
    dht11->SensorConf->intr_type = GPIO_INTR_DISABLE;
    ESP_ERROR_CHECK(gpio_config(dht11->SensorConf));
}

bool DHT_Read(DHT11Struct *dht11)
{
    uint8_t data[5] = {0};
    gpio_set_direction(dht11->gpio_pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dht11->gpio_pin, 0);
    taskENTER_CRITICAL(&mux);
    ets_delay_us(19000);
    gpio_set_direction(dht11->gpio_pin, GPIO_MODE_INPUT);
    CHECK_TIMEOUT(waitState(dht11, 0, 40));
    CHECK_TIMEOUT(waitState(dht11, 1, 80));
    CHECK_TIMEOUT(waitState(dht11, 0, 80));
    for(size_t i = 0; i < 5; i++)
    {
        for (int j = 7; j >= 0; j--)
        {
            CHECK_TIMEOUT(waitState(dht11, 1, 60));
            ets_delay_us(30);
            if(gpio_get_level(dht11->gpio_pin))
            {
                data[i] |= (1U << j);
                CHECK_TIMEOUT(waitState(dht11, 0, 50));
            }
        }
    }
    if (data[0] + data[1] + data[2] + data[3] == data[4])
    {
        dht11->humidity = data[0] + data[1]/10;
        dht11->temperature = data[2] + data[3]/10;
        taskEXIT_CRITICAL(&mux);
        return true;
    }
    else 
    {
        taskEXIT_CRITICAL(&mux);
        ESP_LOGE(TAG, "Checksum DHT11 error");
        return false;
    }
}