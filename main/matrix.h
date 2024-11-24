#include "driver/gpio.h"

#include "ic74hc595.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MATRIX_COLUMNS 16
#define MATRIX_ROWS 16

typedef struct {
        gpio_num_t signal_pin;
        gpio_num_t clock_pin;
        gpio_num_t latch_pin;

        ic74hc595_t *ic74hc595;

        uint8_t content[MATRIX_COLUMNS][MATRIX_ROWS];
} matrix_t;

extern void matrix_init(matrix_t *matrix);

extern void matrix_dot_add(matrix_t *matrix, int column, int row);

extern void matrix_dot_remove(matrix_t *matrix, int column, int row);

extern void matrix_empty(matrix_t *matrix);

void matrix_refresh(matrix_t *matrix);

#ifdef __cplusplus
}
#endif
