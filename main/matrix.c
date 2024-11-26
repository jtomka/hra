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

        matrix->refresh_ticker.span = 1000 / MATRIX_REFRESH_RATE;
        ticker_init(&matrix->refresh_ticker);

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
                        matrix->pixels[i][j].status = false;
                        matrix->pixels[i][j].intensity = 100;
                }
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

bool matrix_apply_rotation(matrix_t *matrix, uint16_t column, uint16_t row)
{
        int c, r;
        int rotate;

        if (matrix == NULL)
                return false;

        if (column >= MATRIX_COLUMNS)
                return false;

        if (row >= MATRIX_ROWS)
                return false;

        rotate = matrix->rotate % 4;

        // Portrait-landscape conversion not supported
        if (MATRIX_COLUMNS != MATRIX_ROWS) {
                if (rotate % 2 != 0) {
                        return false;
                }
        }
                
        c = column;
        r = row;

        if (rotate == 1) {
                c = row;
                r = MATRIX_COLUMNS - column - 1;
        }

        if (rotate == 2) {
                c = MATRIX_COLUMNS - column - 1;
                r = MATRIX_ROWS - row - 1;
        }

        if (rotate == 3) {
                c = MATRIX_ROWS - row - 1;
                r = column;
        }

        return matrix->pixels[c][r].status;
}

bool matrix_apply_intensity(matrix_t *matrix, uint16_t column, uint16_t row)
{
        pixel_t *pixel;
        double on_per_off;

        if (matrix == NULL)
                return false;

        if (column >= MATRIX_COLUMNS)
                return false;

        if (row >= MATRIX_ROWS)
                return false;

        pixel = &matrix->pixels[column][row];

        if (pixel->intensity == 0)
                return false;

        if (pixel->intensity >= 100)
                return true;

        on_per_off = (double) pixel->intensity / (100.0 - (double) pixel->intensity);
        if (pixel->intensity_counter < 1) {
                pixel->intensity_counter += on_per_off;
        }

        if (pixel->intensity_counter >= 1) {
                pixel->intensity_counter--;
                return true;
        }

        return false;
}

void matrix_refresh(matrix_t *matrix)
{
        if (matrix == NULL)
                return;

        bool needs_refresh;

        needs_refresh = ticker_check(&matrix->refresh_ticker);
        if (! needs_refresh)
                return;

        for (int i = 0; i < MATRIX_COLUMNS; i++) {
                uint32_t bits;

                bits = matrix_columns[i];
                for (int j = 0; j < MATRIX_ROWS; j++) {
                        bool status_on, intensity_on;

                        status_on = matrix_apply_rotation(matrix, i, j);

                        intensity_on = matrix_apply_intensity(matrix, i, j);

                        if (status_on && intensity_on) {
                                bits = bits & ~matrix_rows[j];
                        } else {
                                bits = bits | matrix_rows[j];
                        }
                }

                for (int k = 4; k > 0; k--) {
                        ic74hc595_send8bits(&matrix->ic74hc595, ((uint8_t *) &bits)[k - 1]);
                }
                ic74hc595_latch(&matrix->ic74hc595);
        }
}

