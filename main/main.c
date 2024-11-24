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

#define min(a, b) ((a) < (b) ? a : b)

#define BUTTON_0_PIN GPIO_NUM_2
#define BUTTON_1_PIN GPIO_NUM_4
#define BUTTON_2_PIN GPIO_NUM_6

button_t buttons[3];

#define MATRIX_SIGNAL_PIN GPIO_NUM_12
#define MATRIX_CLOCK_PIN GPIO_NUM_10
#define MATRIX_LATCH_PIN GPIO_NUM_8

#define MATRIX_REFRESH_RATE 60

static matrix_t matrix;

typedef struct {
        metronome_t metronome;
} game_t;

static game_t game;

bool button_event_observer(void *data)
{
        button_t *button;

        if (data == NULL)
                return 0;

        button = (button_t *) data;

        return button_status_has_changed(button);
}

void button_event_handler(void *data)
{
        button_t *button;

        if (data == NULL)
                return;

        button = (button_t *) data;

        if (button->status)
                return;
}

void setup_buttons()
{
        buttons[0].pin = BUTTON_0_PIN;
        buttons[1].pin = BUTTON_1_PIN;
        buttons[2].pin = BUTTON_2_PIN;

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
        matrix.clock_pin = MATRIX_CLOCK_PIN;
        matrix.signal_pin = MATRIX_SIGNAL_PIN;
        matrix.latch_pin = MATRIX_LATCH_PIN;
        matrix.rotate = MATRIX_ROTATE_0;

        matrix_init(&matrix);

        matrix_empty(&matrix);

        event_type_t matrix_event_type = {
                .observer = &event_true,
                .handler = &matrix_event_handler,
                .data = &matrix
        };
        event_type_register(matrix_event_type);
}

bool game_event_observer(void *data)
{
        if (data == NULL)
                return false;

        game_t *game = (game_t *) data;

        return metronome_check(&game->metronome);
}

void game_event_handler(void *data)
{
        static int c = 0, r = 0, c_dir = 1, r_dir = 1;

        if (c < 0) {
                c = 0;
                c_dir = 1;
        }

        if (c >= MATRIX_COLUMNS) {
                c = MATRIX_COLUMNS - 1;
                c_dir = -1;
        }

        if (r < 0) {
                r = 0;
                r_dir = 1;
                c += c_dir;
        }

        if (r >= MATRIX_ROWS) {
                r = MATRIX_ROWS - 1;
                r_dir = -1;
                c += c_dir;
        }
        
        matrix_empty(&matrix);
        matrix_dot_add(&matrix, c, r);

        r += r_dir;
}

void setup_game()
{
        game.metronome.tick_len = 25;
        metronome_init(&game.metronome);

        event_type_t game_event_type = {
                .observer = &game_event_observer,
                .handler = &game_event_handler,
                .data = &game
        };
        event_type_register(game_event_type);
}


void app_main(void)
{
        setup_buttons();
        setup_matrix();
        setup_game();

        event_loop();
}

