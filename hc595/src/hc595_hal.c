#include "../interface/hc595_hal_interface.h"
#include "../include/hc595_hal.h"

int hc595_hal_init(hc595_handle_t handle)
{
    if (handle->init) {
        return handle->init(handle);
    }

    return -1;
}

int hc595_hal_deinit(hc595_handle_t handle)
{
    if (handle->deinit) {
        return handle->deinit(handle);
    }

    return -1;
}

int hc595_hal_set_io_level(hc595_handle_t handle, hc595_io_t io, hc595_io_level_t level)
{
    if (handle->set_level) {
        return handle->set_level(handle, io, level);
    }

    return -1;
}

int hc595_hal_delay(hc595_handle_t handle)
{
    if (handle->delay) {
        return handle->delay(handle);
    }

    return -1;
}

bool hc595_hal_check_output_remain(hc595_handle_t handle)
{
    if (handle->check_output_remain) {
        return handle->check_output_remain(handle);
    }

    return -1;
}