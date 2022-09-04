#ifndef HC595_HAL_TYPES_H
#define HC595_HAL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HC595_IO_DIN = 0,
    HC595_IO_RST,
    HC595_IO_SCLK,
    HC595_IO_RCLK,
    HC595_IO_OEN,
} hc595_io_t;

typedef enum {
    HC595_IO_LEVEL_H = 0,
    HC595_IO_LEVEL_L,
} hc595_io_level_t;


#ifdef __cplusplus
}
#endif

#endif