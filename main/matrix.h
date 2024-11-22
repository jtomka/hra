#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        gpio_num_t signal_pin;
        gpio_num_t clock_pin;
        gpio_num_t latch_pin;
        int width;
        int height;
} matrix_t;

extern void matrix_init(matrix_t *matrix);

#ifdef __cplusplus
}
#endif
