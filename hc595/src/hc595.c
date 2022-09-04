/**
 * hc595: shift -> storage -> en
 */
#include "../interface/hc595_hal_interface.h"
#include "../include/hc595_hal.h"
#include "../include/hc595.h"

static void hc595_shift_clk_once(hc595_handle_t handle);
static void hc595_storage_clk_once(hc595_handle_t handle);

void hc595_init(hc595_handle_t handle)
{
    hc595_hal_init(handle);
}

void hc595_write_byte(hc595_handle_t handle, uint8_t data)
{
    for (int i = 0; i < 8; i++) {
        if (data & 0x80) {
            hc595_hal_set_io_level(handle, HC595_IO_DIN, HC595_IO_LEVEL_H);
        }
        else {
            hc595_hal_set_io_level(handle, HC595_IO_DIN, HC595_IO_LEVEL_L);
        }
        data <<= 1;
        hc595_shift_clk_once(handle);
    }
    hc595_storage_clk_once(handle);
}

void hc595_write_bytes(hc595_handle_t handle, uint8_t *data, int data_len)
{
    uint8_t temp;
    for (int n = 0; n < data_len; n++) {
        temp = data[n];
        for (int i = 0; i < 8; i++) {
            if (temp & 0x80)
                hc595_hal_set_io_level(handle, HC595_IO_DIN, HC595_IO_LEVEL_H);
            else
                hc595_hal_set_io_level(handle, HC595_IO_DIN, HC595_IO_LEVEL_L);
            temp <<= 1;
            hc595_shift_clk_once(handle);
        }
    }
    hc595_storage_clk_once(handle);
}

void hc595_reset_shift_reg(hc595_handle_t handle)
{
    // Reset low
    if (hc595_hal_set_io_level(handle, HC595_IO_RST, HC595_IO_LEVEL_L)) {
        return;
    }
    // Output_en low
    if (!hc595_hal_check_output_remain(handle)) {
        hc595_hal_set_io_level(handle, HC595_IO_OEN, HC595_IO_LEVEL_H);
    }
    hc595_hal_delay(handle);
    // Restore Reset and Output_en
    hc595_hal_set_io_level(handle, HC595_IO_RST, HC595_IO_LEVEL_H);
    if (!hc595_hal_check_output_remain(handle)) {
        hc595_hal_set_io_level(handle, HC595_IO_OEN, HC595_IO_LEVEL_L);
    }
    hc595_hal_delay(handle);
}

void hc595_reset_storage_reg(hc595_handle_t handle)
{
    // Reset low
    if (hc595_hal_set_io_level(handle, HC595_IO_RST, HC595_IO_LEVEL_L)) {
        return;
    }
    // Storage clk
    hc595_storage_clk_once(handle);
    hc595_hal_delay(handle);
    // Restore Reset
    hc595_hal_set_io_level(handle, HC595_IO_RST, HC595_IO_LEVEL_H);
    hc595_hal_delay(handle);
}

void hc595_enable_output(hc595_handle_t handle, int flag)
{
    hc595_io_level_t level = flag ? HC595_IO_LEVEL_H : HC595_IO_LEVEL_L;
    hc595_hal_set_io_level(handle, HC595_IO_OEN, level);
}

static void hc595_shift_clk_once(hc595_handle_t handle)
{
    hc595_hal_set_io_level(handle, HC595_IO_SCLK, HC595_IO_LEVEL_H);
    hc595_hal_delay(handle);
    hc595_hal_set_io_level(handle, HC595_IO_SCLK, HC595_IO_LEVEL_L);
}

static void hc595_storage_clk_once(hc595_handle_t handle)
{
    hc595_hal_set_io_level(handle, HC595_IO_RCLK, HC595_IO_LEVEL_H);
    hc595_hal_delay(handle);
    hc595_hal_set_io_level(handle, HC595_IO_RCLK, HC595_IO_LEVEL_L);
}
