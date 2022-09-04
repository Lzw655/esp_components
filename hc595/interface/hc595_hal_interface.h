#ifndef HC595_HAL_INTERFACE_H
#define HC595_HAL_INTERFACE_H

#include <stdbool.h>
#include "hc595_hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hc595_hal_t hc595_hal_t;

struct hc595_hal_t {
    int (* init)(hc595_hal_t *handle);
    int (* deinit)(hc595_hal_t *handle);
    int (* set_level)(hc595_hal_t *handle, hc595_io_t io, hc595_io_level_t level);
    int (* delay)(hc595_hal_t *handle);
    bool (* check_output_remain)(hc595_hal_t *handle);
};

#ifdef __cplusplus
}
#endif

#endif