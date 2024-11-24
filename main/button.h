#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        gpio_num_t pin;
        int status;
        int timestamp;
        int prev_status;
        int prev_timestamp;
} button_t;

extern void button_init(button_t *button);
extern bool button_status_has_changed(button_t *button);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H */

