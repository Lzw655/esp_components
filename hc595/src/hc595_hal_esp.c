#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "driver/gpio.h"
#include "../interface/hc595_hal_interface.h"
#include "../include/hc595_hal.h"
#include "../include/hc595.h"

typedef struct {
    hc595_hal_t base;
    hc595_config_t config;
} hc595_hal_esp_t;

static int init(hc595_handle_t handle);
static int deinit(hc595_handle_t handle);
static int set_level(hc595_handle_t handle, hc595_io_t io, hc595_io_level_t level);
static int delay(hc595_handle_t handle);
static bool check_output_remain(hc595_handle_t handle);

int hc595_hal_new_esp(hc595_config_t *config, hc595_handle_t *handle)
{
    hc595_hal_esp_t *dev = (hc595_hal_esp_t *)malloc(sizeof(hc595_hal_esp_t));
    if (dev == NULL) {
        return -1;
    }

    dev->base.init = init;
    dev->base.deinit = deinit;
    dev->base.set_level = set_level;
    dev->base.delay = delay;
    dev->base.check_output_remain = check_output_remain;
    memcpy(&(dev->config), config, sizeof(hc595_config_t));

    *handle = &(dev->base);

    return 0;
}

static int init(hc595_handle_t handle)
{
    hc595_hal_esp_t *dev = (hc595_hal_esp_t *)handle;
    uint64_t gpio_pin_mask = 0;
    /* Init necessary gpio */
    gpio_pin_mask = (1ULL << (dev->config.din_gpio_num)) | (1ULL << (dev->config.sclk_gpio_num)) |
                    (1ULL << (dev->config.rclk_gpio_num));

    /* Init unnecessary gpio */
    if (dev->config.rst_gpio_num != -1) {
        gpio_pin_mask |= (1ULL << dev->config.rst_gpio_num);
    }
    if (dev->config.oen_gpio_num != -1) {
        gpio_pin_mask |= (1ULL << dev->config.oen_gpio_num);
    }

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = gpio_pin_mask;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    esp_err_t err = gpio_config(&io_conf);

    gpio_set_level(dev->config.din_gpio_num, 0);
    gpio_set_level(dev->config.sclk_gpio_num, 0);
    gpio_set_level(dev->config.rclk_gpio_num, 0);
    if (dev->config.rst_gpio_num != -1) {
        gpio_set_level(dev->config.rst_gpio_num, 1);
    }
    if (dev->config.oen_gpio_num != -1) {
        if (dev->config.flags.output_remain) {
            gpio_set_level(dev->config.rst_gpio_num, 0);
        }
        else {
            gpio_set_level(dev->config.rst_gpio_num, 1);
        }
    }

    return (err != ESP_OK) ? -1 : 0;
}

static int deinit(hc595_handle_t handle)
{

    return 0;
}

static int set_level(hc595_handle_t handle, hc595_io_t io, hc595_io_level_t level)
{
    hc595_hal_esp_t *dev = (hc595_hal_esp_t *)handle;
    int gpio_num, gpio_level;
    switch (io) {
        case HC595_IO_DIN:
            gpio_num = dev->config.din_gpio_num;
            break;
        case HC595_IO_RST:
            gpio_num = dev->config.rst_gpio_num;
            break;
        case HC595_IO_SCLK:
            gpio_num = dev->config.sclk_gpio_num;
            break;
        case HC595_IO_RCLK:
            gpio_num = dev->config.rclk_gpio_num;
            break;
        case HC595_IO_OEN:
            gpio_num = dev->config.oen_gpio_num;
            break;
        default:
            return -1;
    }
    if (gpio_num < 0) {
        return -1;
    }
    gpio_level = (level == HC595_IO_LEVEL_L) ? 0 : 1;
    gpio_set_level(gpio_num, gpio_level);

    return 0;
}

static int delay(hc595_handle_t handle)
{
    esp_rom_delay_us(1);

    return 0;
}

static bool check_output_remain(hc595_handle_t handle)
{
    hc595_hal_esp_t *dev = (hc595_hal_esp_t *)handle;

    return dev->config.flags.output_remain;
}
