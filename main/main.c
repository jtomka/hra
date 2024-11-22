#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "matrix.h"
#include "button.h"
#include "event.h"

#define BUTTON_0_PIN GPIO_NUM_2
#define BUTTON_1_PIN GPIO_NUM_4
#define BUTTON_2_PIN GPIO_NUM_6

#define MATRIX_SIGNAL_PIN GPIO_NUM_12
#define MATRIX_CLOCK_PIN GPIO_NUM_10
#define MATRIX_LATCH_PIN GPIO_NUM_8

button_t buttons[] = {
        { .pin = BUTTON_0_PIN },
        { .pin = BUTTON_1_PIN },
        { .pin = BUTTON_2_PIN }
};

bool timer_event_observer(void *data)
{
        usleep(10000);
        return false;
}

bool buttons_event_observer(void *data)
{
        button_t *button;

        if (data == NULL)
                return 0;

        button = (button_t *) data;

        return button_status_has_changed(button);
}

void buttons_event_handler(void *data)
{
        button_t *button;
        char *status_string = "";
        long after_ms = 0;

        if (data == NULL)
                return;

        button = (button_t *) data;

        status_string = button->status ? "pressed" : "released";
        after_ms = button->timestamp - button->prev_timestamp;

        printf("Button [GPIO%d] %s after %ldms\n", button->pin, status_string, after_ms);
}

void app_main(void)
{
        event_type_t timer_event_type = {
                .observer = &timer_event_observer,
                .handler = NULL,
                .data = NULL
        };
        event_type_register(timer_event_type);

        for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
                button_init(&buttons[i]);

                event_type_t button_event_type = {
                        .observer = &buttons_event_observer,
                        .handler = &buttons_event_handler,
                        .data = &buttons[i]
                };
                event_type_register(button_event_type);
        }


        matrix_t matrix = {
                .signal_pin = MATRIX_SIGNAL_PIN,
                .clock_pin = MATRIX_CLOCK_PIN,
                .latch_pin = MATRIX_LATCH_PIN,
                .width = 16,
                .height = 16
        };
        matrix_init(&matrix);

        event_loop();
}

