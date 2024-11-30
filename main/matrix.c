#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include "ticker.h"

#include "matrix.h"

const uint32_t matrix_columns[] = {
        0x00001000UL, 0x10000000UL, 0x00080000UL, 0x00000200UL,
        0x08000000UL, 0x00000400UL, 0x00000800UL, 0x00000100UL,
        0x00000020UL, 0x40000000UL, 0x80000000UL, 0x00000008UL,
        0x20000000UL, 0x00000004UL, 0x00000080UL, 0x00000001UL
};

const uint32_t matrix_rows[] = {
        0x00000010UL, 0x00000040UL, 0x00004000UL, 0x00000002UL,
        0x00002000UL, 0x00008000UL, 0x04000000UL, 0x02000000UL,
        0x00020000UL, 0x00010000UL, 0x00800000UL, 0x00040000UL,
        0x00200000UL, 0x00400000UL, 0x01000000UL, 0x00100000UL
};

void matrix_init(matrix_t *matrix)
{
        if (matrix == NULL)
                return;

        matrix->brightness = 100;
        matrix->prev_brightness = 100 - matrix->brightness;

        matrix->ic74hc595.signal_pin = matrix->signal_pin;
        matrix->ic74hc595.clock_pin = matrix->clock_pin;
        matrix->ic74hc595.latch_pin = matrix->latch_pin;
        matrix->ic74hc595.send_mode = IC74HC595_SEND_MODE_MSB_FIRST;

        ic74hc595_init(&matrix->ic74hc595);

        matrix_clear(matrix);
}

void matrix_clear(matrix_t *matrix)
{
        if (matrix == NULL)
                return;

        for (int i = 0; i < MATRIX_COLUMNS; i++) {
                for (int j = 0; j < MATRIX_ROWS; j++) {
                        pixel_t *pixel;

                        pixel = &matrix->pixels[i][j];
                        pixel->status = false;
                        pixel->brightness = 100;
                }
        }
}

static inline pixel_t *matrix_apply_rotation_get_pixel(matrix_t *matrix,
                                                       uint16_t column,
                                                       uint16_t row)
{
        uint16_t c, r;

        if (matrix == NULL)
                return false;

        if (column >= MATRIX_COLUMNS)
                return false;

        if (row >= MATRIX_ROWS)
                return false;

        if (matrix->rotation > 3)
                return false;

        // Portrait-landscape conversion not supported
        if (MATRIX_COLUMNS != MATRIX_ROWS) {
                if (matrix->rotation == 1 || matrix->rotation == 3) {
                        return false;
                }
        }
                
        c = column;
        r = row;

        if (matrix->rotation == 1) {
                c = row;
                r = MATRIX_COLUMNS - column - 1;

        } else if (matrix->rotation == 2) {
                c = MATRIX_COLUMNS - column - 1;
                r = MATRIX_ROWS - row - 1;

        } else if (matrix->rotation == 3) {
                c = MATRIX_ROWS - row - 1;
                r = column;
        }

        return &matrix->pixels[c][r];
}

static inline uint8_t matrix_get_pixel_brightness_duty_cycle(matrix_t *matrix,
                                                             pixel_t *pixel)
{
        double matrix_brigtness;
        double combined_brigtness;

        matrix_brightness = matrix->brightness_adj_max - matrix->brightness_adj_min;
        matrix_brightness *= matrix->brightness / 100;
        matrix_brightness += matrix->brightness_adj_min;

        combined_brightness = pixel->brightness * matrix_brightness / 100.0;
        return combined_brightness;
}

static inline bool matrix_apply_brightness(matrix_t *matrix, pixel_t *pixel)
{
        bool matrix_brightness_changed;
        bool pixel_brightness_changed;
        bool brightness_status;

        matrix_brightness_changed = false;
        if (matrix->brightness != matrix->prev_brightness) {
                matrix_brightness_changed = true;
        }

        pixel_brightness_changed = false;
        if (pixel->brightness != pixel->prev_brightness) {
                pixel_brightness_changed = true;
        }

        if (matrix_brightness_changed || pixel_brightness_changed) {
                pixel_pwm->duty_cycle = matrix_get_pixel_brightness_duty_cycle(matrix, pixel);
        }

        brightness_status = pwm_check(pixel_pwm);
        return brightness_status;
}

void matrix_refresh(matrix_t *matrix)
{
static clock_t now = 0;
static uint32_t cycles = 0;

if (ticker_now() - now > 10000) {
        printf("%f Hz\n", cycles / 10.0);
        now = ticker_now();
        cycles = 0;
}
cycles++;

        if (matrix == NULL)
                return;

        for (int i = 0; i < MATRIX_COLUMNS; i++) {
                uint32_t bits;

                bits = matrix_columns[i];
                for (int j = 0; j < MATRIX_ROWS; j++) {
                        pixel_t *pixel;
                        bool pixel_status;
                        bool brightness_status;

                        pixel = matrix_apply_rotation_get_pixel(matrix, i, j);
                        pixel_status = pixel->status;

                        brightness_status = matrix_apply_brightness(matrix, pixel);

                        if (pixel_status && brightness_status) {
                                bits = bits & ~matrix_rows[j];
                        } else {
                                bits = bits | matrix_rows[j];
                        }

                        if (pixel->brightness != pixel->prev_brightness) {
                                pixel->prev_brightness = pixel->brightness;
                        }
                }

                for (int k = 4; k > 0; k--) {
                        ic74hc595_send8bits(&matrix->ic74hc595, ((uint8_t *) &bits)[k - 1]);
                }
                ic74hc595_latch(&matrix->ic74hc595);
        }

        if (matrix->prev_brightness != matrix->brightness) {
                matrix->prev_brightness = matrix->brightness;
        }
}

void matrix_block(matrix_t *matrix, uint16_t x1, uint16_t y1,
                  uint16_t x2, uint16_t y2, bool fill)
{
        if (matrix == NULL)
                return;

        if (x1 >= MATRIX_COLUMNS)
                return;

        if (y1 >= MATRIX_ROWS)
                return;

        if (x2 >= MATRIX_COLUMNS)
                return;

        if (y2 >= MATRIX_ROWS)
                return;

        if (x1 > x2) {
                int tmp;
                tmp = x1;
                x1 = x2;
                x2 = tmp;
        }

        if (y1 > y2) {
                int tmp;
                tmp = y1;
                y1 = y2;
                y2 = tmp;
        }

        for (int i = x1; i <= x2; i++) {
                for (int j = y1; j <= y2; j++) {
                        int sides = (i == x1 || i == x2 || j == y1 || j == y2);
                        if (fill || sides) {
                                matrix->pixels[i][j].status = true;
                        }
                }
        }
}

