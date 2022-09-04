#ifndef HC595_HAL_H
#define HC595_HAL_H

#include <stdbool.h>
#include "../interface/hc595_hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hc595_hal_t *hc595_handle_t;

int hc595_hal_init(hc595_handle_t handle);
int hc595_hal_deinit(hc595_handle_t handle);
int hc595_hal_set_io_level(hc595_handle_t handle, hc595_io_t io, hc595_io_level_t level);
int hc595_hal_delay(hc595_handle_t handle);
bool hc595_hal_check_output_remain(hc595_handle_t handle);

typedef struct {
    int din_gpio_num;                   /*!< -1 if not used */
    int rst_gpio_num;
    int sclk_gpio_num;
    int rclk_gpio_num;
    int oen_gpio_num;
    struct {
        unsigned int output_remain;
    } flags;
} hc595_config_t;

int hc595_hal_new_esp(hc595_config_t *config, hc595_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif