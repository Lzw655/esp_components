#ifndef HC595_H
#define HC595_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hc595_hal_t *hc595_handle_t;

void hc595_init(hc595_handle_t handle);
void hc595_write_byte(hc595_handle_t handle, uint8_t data);
void hc595_write_bytes(hc595_handle_t handle, uint8_t *data, int data_len);
void hc595_reset_shift_reg(hc595_handle_t handle);
void hc595_reset_storage_reg(hc595_handle_t handle);
void hc595_enable_output(hc595_handle_t handle, int flag);

#ifdef __cplusplus
}
#endif

#endif