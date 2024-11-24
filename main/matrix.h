#ifndef MATRIX_H
#define MATRIX_H

#include "driver/gpio.h"

#include "ic74hc595.h"
#include "metronome.h"

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
        gpio_num_t signal_pin;
        gpio_num_t clock_pin;
        gpio_num_t latch_pin;
        int rotate;

        uint8_t content[MATRIX_COLUMNS][MATRIX_ROWS];

        ic74hc595_t ic74hc595;
        metronome_t refresh_clock;
} matrix_t;

extern void matrix_init(matrix_t *matrix);

extern void matrix_dot_add(matrix_t *matrix, int column, int row);

extern void matrix_dot_remove(matrix_t *matrix, int column, int row);

extern void matrix_empty(matrix_t *matrix);

void matrix_refresh(matrix_t *matrix);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_H */

