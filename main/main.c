#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "button.h"
#include "event.h"
#include "metronome.h"
#include "matrix.h"

//#include "ic74hc595.h"

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

matrix_t matrix = {
        .clock_pin = MATRIX_CLOCK_PIN,
        .signal_pin = MATRIX_SIGNAL_PIN,
        .latch_pin = MATRIX_LATCH_PIN
};

metronome_t metronome = {
        .clock = 0,
        .tick_len = 10
};

bool button_event_observer(void *data)
{
        button_t *button;

        if (data == NULL)
                return 0;

        button = (button_t *) data;

        return button_status_has_changed(button);
}

void shift_two_bytes(uint8_t two[2])
{
        uint16_t one = (uint16_t) two[0] << 8 | two[1];

        if (one == 0) {
                one = 1U;
        } else {
                one = one << 1;
                if (one == 0)
                        one = 1U;
        }
        two[0] = one >> 8;
        two[1] = one & 0xffU;
}

void button_event_handler(void *data)
{
        button_t *button;

        if (data == NULL)
                return;

        button = (button_t *) data;

        static uint8_t column[2] = { 0U, 0U };
        static uint8_t row[2] = { 0U, 0U };

        if (button->status)
                return;

        if (button->pin == BUTTON_1_PIN) {
                shift_two_bytes(column);
        }

        if (button->pin == BUTTON_2_PIN) {
                shift_two_bytes(row);
        }

        ic74hc595_send8bits(matrix.ic74hc595, row[0]);
        ic74hc595_send8bits(matrix.ic74hc595, row[1]);
        ic74hc595_send8bits(matrix.ic74hc595, column[0]);
        ic74hc595_send8bits(matrix.ic74hc595, column[1]);
        ic74hc595_latch(matrix.ic74hc595);
}

bool metronome_event_observer(void *data)
{
        metronome_t *metronome;

        if (data == NULL)
                return false;

        metronome = (metronome_t *) data;

        return metronome_status_update(metronome);
}

void metronome_event_handler(void *data)
{
        usleep(10000);
}

void setup_metronome()
{
        metronome_init(&metronome);

        event_type_t metronome_event_type = {
                .observer = &metronome_event_observer,
                .handler = &metronome_event_handler,
                .data = &metronome
        };
        event_type_register(metronome_event_type);
}

void setup_buttons()
{
        for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
                button_init(&buttons[i]);

                event_type_t button_event_type = {
                        .observer = &button_event_observer,
                        .handler = &button_event_handler,
                        .data = &buttons[i]
                };
                event_type_register(button_event_type);
        }
}

bool matrix_event_observer(void *data)
{
        return true;
}

void matrix_event_handler(void *data)
{
        matrix_t *matrix;

        if (data == NULL)
                return;

        matrix = (matrix_t *) data;

        matrix_refresh(matrix);
}

void setup_matrix()
{
        matrix_init(&matrix);

        matrix_empty(&matrix);

        event_type_t matrix_event_type = {
                .observer = &matrix_event_observer,
                .handler = &matrix_event_handler,
                .data = &matrix
        };
        event_type_register(matrix_event_type);
}

void app_main(void)
{
        setup_buttons();
        setup_matrix();
        setup_metronome();

        event_loop();
}

