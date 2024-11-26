#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "driver/gpio.h"

#include "blinker.h"
#include "button.h"
#include "event.h"
#include "matrix.h"
#include "ticker.h"

#ifndef min
#define min(a, b) ((a) < (b) ? a : b)
#endif

#define BUTTON_0_PIN GPIO_NUM_2
#define BUTTON_1_PIN GPIO_NUM_4
#define BUTTON_2_PIN GPIO_NUM_6

#define MATRIX_SIGNAL_PIN GPIO_NUM_12
#define MATRIX_CLOCK_PIN GPIO_NUM_10
#define MATRIX_LATCH_PIN GPIO_NUM_8

typedef struct {
        button_t buttons[3];
        matrix_t matrix;
        uint8_t matrix_intensity;
        ticker_t ticker;
        blinker_t blinker;
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

        if (button->pin == BUTTON_2_PIN) {
                game.matrix_intensity += 10;
                if (game.matrix_intensity > 100)
                        game.matrix_intensity = 100;
        } else if (button->pin == BUTTON_1_PIN) {
                game.matrix_intensity += -10;
        }
printf("intensity %i\n", game.matrix_intensity);
}

void setup_buttons()
{
        game.buttons[0].pin = BUTTON_0_PIN;
        game.buttons[1].pin = BUTTON_1_PIN;
        game.buttons[2].pin = BUTTON_2_PIN;

        for (int i = 0; i < sizeof(game.buttons) / sizeof(game.buttons[0]); i++) {
                button_init(&game.buttons[i]);

                event_type_t button_event_type = {
                        .observer = &button_event_observer,
                        .handler = &button_event_handler,
                        .data = &game.buttons[i]
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
        game.matrix.clock_pin = MATRIX_CLOCK_PIN;
        game.matrix.signal_pin = MATRIX_SIGNAL_PIN;
        game.matrix.latch_pin = MATRIX_LATCH_PIN;
        game.matrix.rotate = MATRIX_ROTATE_0;

        matrix_init(&game.matrix);

        event_type_t matrix_event_type = {
                .observer = &event_true,
                .handler = &matrix_event_handler,
                .data = &game.matrix
        };
        event_type_register(matrix_event_type);
}

bool game_event_observer(void *data)
{
        if (data == NULL)
                return false;

        game_t *game = (game_t *) data;

        return ticker_check(&game->ticker);
}

void matrix_screensaver()
{
        static int c = 0, r = 0, c_dir = 1, r_dir = 1;
        pixel_t *pixel;
        bool status;

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
        
        if (c < 0) {
                c = 0;
                c_dir = 1;
        }

        if (c >= MATRIX_COLUMNS) {
                c = MATRIX_COLUMNS - 1;
                c_dir = -1;
        }

        pixel = &game.matrix.pixels[c][r];
        status = blinker_check(&game.blinker);
        pixel->status = status;

        r += r_dir;
}

void game_event_handler(void *data)
{
        matrix_clear(&game.matrix);

        matrix_screensaver();
}

void setup_game()
{
        setup_buttons();
        setup_matrix();
        game.matrix_intensity = 100;

        game.ticker.span = 30;
        ticker_init(&game.ticker);

        game.blinker.on_len = 100;
        blinker_init(&game.blinker);

        event_type_t game_event_type = {
                .observer = &game_event_observer,
                .handler = &game_event_handler,
                .data = &game
        };
        event_type_register(game_event_type);
}


void app_main(void)
{
        setup_game();

        event_loop();
}

