#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>

#include "driver/gpio.h"

#include "ticker.h"
#include "ic74hc595.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MATRIX_COLUMNS 16
#define MATRIX_ROWS 16

#define MATRIX_ROTATE_0 0
#define MATRIX_ROTATE_90 1
#define MATRIX_ROTATE_180 2
#define MATRIX_ROTATE_270 3

#define MATRIX_REFRESH_RATE 60

typedef struct {
        bool status;
        uint8_t intensity;
        double intensity_counter;
} pixel_t;

typedef struct {
        gpio_num_t signal_pin;
        gpio_num_t clock_pin;
        gpio_num_t latch_pin;
        uint8_t rotate;

        pixel_t pixels[MATRIX_COLUMNS][MATRIX_ROWS];

        ic74hc595_t ic74hc595;
        ticker_t refresh_ticker;
} matrix_t;

extern void matrix_init(matrix_t *matrix);

extern void matrix_clear(matrix_t *matrix);

extern void matrix_block(matrix_t *matrix, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2, bool fill);

#define matrix_point(matrix, x, y) matrix_block(matrix, x, y, x, y, true)

#define matrix_line(matrix, x1, y1, x2, y2) matrix_block(matrix, x1, y1, x2, y2, true)

extern void matrix_refresh(matrix_t *matrix);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_H */

