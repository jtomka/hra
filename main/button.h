#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUTTON_STATUS_RELEASE 0
#define BUTTON_STATUS_PRESS 1
#define BUTTON_STATUS_HOLD 2

typedef struct {
        gpio_num_t pin;

        struct {
                uint16_t init;
                uint16_t repeat;
        } hold_time;

        uint8_t status;
        clock_t timestamp;
} button_t;

extern bool button_check(button_t *button);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H */

