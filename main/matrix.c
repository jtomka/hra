#include "driver/gpio.h"

#include "matrix.h"

void matrix_init(matrix_t *matrix)
{
        if (matrix == NULL)
                return;

        const gpio_num_t matrix_pins[] = {
                matrix->signal_pin,
                matrix->clock_pin,
                matrix->latch_pin
        };

        for (int i = 0; i < sizeof(matrix_pins) / sizeof(matrix_pins[0]); i++) {
                gpio_set_direction(matrix_pins[i], GPIO_MODE_OUTPUT);
                gpio_set_pull_mode(matrix_pins[i], GPIO_PULLDOWN_ONLY);
                gpio_set_level(matrix_pins[i], 0);
        }
}

