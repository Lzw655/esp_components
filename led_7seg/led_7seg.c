#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hc595_hal.h"
#include "hc595.h"
#include "led_7seg.h"

typedef struct led_7seg_t led_7seg_t;

struct led_7seg_t {
    hc595_handle_t hc595_handle;
    TimerHandle_t blink_timer;
    uint8_t bits_num;
    uint8_t refresh_period_per_bit;
    uint8_t blink_bits_mask;
    bool bink_enable;
    uint8_t refresh_bits_mask;
    uint8_t display_array[];
};

static char *TAG = "led_7seg";
static const uint8_t display_char[] = {
//   0     1     2     3     4     5     6     7     8     9     A     b     C     d     E     F     -
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x8C, 0xBF, 0xC6, 0xA1, 0x86, 0xFF, 0xBF, 0xFF
};

static void refresh_task(void *param);
static void blink_timer_cb(TimerHandle_t xTimer);

void led_7seg_init(led_7seg_config_t *config, led_7seg_handle_t *handle)
{
    // Create led_7seg module
    led_7seg_t *module = (led_7seg_t *)malloc(sizeof(led_7seg_t) + config->bits_num);

    // Create hc595 device
    hc595_config_t hc_config = {
        .din_gpio_num = config->din_gpio_num,
        .rst_gpio_num = -1,
        .sclk_gpio_num = config->sclk_gpio_num,
        .rclk_gpio_num = config->rclk_gpio_num,
        .oen_gpio_num = -1,
        .flags.output_remain = 1,
    };
    hc595_hal_new_esp(&hc_config, &module->hc595_handle);
    hc595_init(module->hc595_handle);

    // Init module
    module->bits_num = config->bits_num;
    module->blink_bits_mask = 0;
    module->refresh_bits_mask = 0xff;
    module->refresh_period_per_bit = config->refresh_period_per_bit;
    xTaskCreatePinnedToCore(
        refresh_task, "led_7seg", 2048, (void *)module, config->blink.task_priority, NULL, config->blink.task_core
    );
    module->blink_timer = NULL;
    if (config->blink.enable) {
        module->blink_timer = xTimerCreate(
            "", pdMS_TO_TICKS(config->blink.period / 2), pdTRUE, (void *)module, blink_timer_cb
        );
        module->bink_enable = false;
    }
    *handle = module;

    ESP_LOGI(TAG, "Finish init");
}

void led_7seg_set_display_int(int num, led_7seg_handle_t handle)
{
    uint8_t temp;
    uint8_t *display_array = handle->display_array;
    uint8_t bits_num = handle->bits_num;

    for (int i = 0; i < bits_num; i++) {
        temp = num % 10;
        display_array[i] = display_char[temp];
        num /= 10;
    }
}

void led_7seg_blink(uint8_t bit_mask, bool en, led_7seg_handle_t handle)
{
    uint8_t old_mask = handle->blink_bits_mask;
    if (en) {
        handle->blink_bits_mask |= bit_mask;
    }
    else {
        handle->blink_bits_mask &= ~bit_mask;
    }
    // Start blink
    if (!old_mask && handle->blink_bits_mask && !handle->bink_enable) {
        handle->bink_enable = true;
        xTimerStart(handle->blink_timer, portMAX_DELAY);
    }
    // Stop blink
    else if (old_mask && !handle->blink_bits_mask) {
        if (handle->bink_enable) {
            handle->bink_enable = false;
            xTimerStop(handle->blink_timer, portMAX_DELAY);
        }
        handle->refresh_bits_mask = 0xff;
    }
}

static void refresh_task(void *param)
{
    ESP_LOGI(TAG, "refresh_task start");

    led_7seg_handle_t handle = (led_7seg_handle_t)param;
    hc595_handle_t hc_handle = handle->hc595_handle;
    uint8_t *display_array;
    uint8_t bits_num;
    uint8_t refresh_bits_mask;
    uint8_t period;
    TickType_t tick;
    uint8_t temp[2];

    for (;;) {
        display_array = handle->display_array;
        bits_num = handle->bits_num;
        refresh_bits_mask = handle->refresh_bits_mask;
        period = handle->refresh_period_per_bit;

        // Refresh all bits
        for (int i = 0; i < bits_num; i++) {
            tick = xTaskGetTickCount();
            temp[1] = 1 << i;
            if (refresh_bits_mask & 0x1) {
                temp[0] = display_array[i];
            }
            else {
                temp[0] = display_char[17];
            }
            refresh_bits_mask >>= 1;
            hc595_write_bytes(hc_handle, temp, 2);
            xTaskDelayUntil(&tick, pdMS_TO_TICKS(period));
        }
    }
}

static void blink_timer_cb(TimerHandle_t xTimer)
{
    led_7seg_handle_t handle = (led_7seg_handle_t)pvTimerGetTimerID(xTimer);
    uint8_t bits_num = handle->bits_num;
    uint8_t blink_bits_mask = handle->blink_bits_mask;
    uint8_t refresh_bits_mask = handle->refresh_bits_mask;
    for (int i = 0; i < bits_num; i++) {
        if (blink_bits_mask & 0x1) {
            refresh_bits_mask ^= (1 << i);
        }
        blink_bits_mask >>= 1;
    }
    handle->refresh_bits_mask = refresh_bits_mask;
}
