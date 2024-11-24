#include <stdlib.h>
#include <unistd.h>

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

        matrix->refresh_clock.tick_len = 1000 / MATRIX_REFRESH_RATE;
        metronome_init(&matrix->refresh_clock);

        matrix->ic74hc595.signal_pin = matrix->signal_pin;
        matrix->ic74hc595.clock_pin = matrix->clock_pin;
        matrix->ic74hc595.latch_pin = matrix->latch_pin;
        matrix->ic74hc595.send_mode = IC74HC595_SEND_MODE_MSB_FIRST;

        ic74hc595_init(&matrix->ic74hc595);

        matrix_empty(matrix);
}

void matrix_dot_set(matrix_t *matrix, int column, int row, int value)
{
        if (matrix == NULL)
                return;

        if (column < 0 || column >= MATRIX_COLUMNS)
                return;

        if (row < 0 || row >= MATRIX_ROWS)
                return;

        matrix->content[column][row] = value ? 1 : 0;
}

void matrix_dot_add(matrix_t *matrix, int column, int row)
{
        if (matrix == NULL)
                return;

        if (column < 0 || column >= MATRIX_COLUMNS)
                return;

        if (row < 0 || row >= MATRIX_ROWS)
                return;

        matrix_dot_set(matrix, column, row, 1);
}

void matrix_dot_remove(matrix_t *matrix, int column, int row)
{
        if (matrix == NULL)
                return;

        if (column < 0 || column >= MATRIX_COLUMNS)
                return;

        if (row < 0 || row >= MATRIX_ROWS)
                return;

        matrix_dot_set(matrix, column, row, 0);
}

void matrix_empty(matrix_t *matrix)
{
        if (matrix == NULL)
                return;

        for (int i = 0; i < MATRIX_COLUMNS; i++) {
                for (int j = 0; j < MATRIX_ROWS; j++) {
                        matrix->content[i][j] = 0;
                }
        }
}

int matrix_apply_rotation(matrix_t *matrix, int column, int row)
{
        int c, r;
        int rotate;

        if (matrix == NULL)
                return -1;

        if (column < 0 || column >= MATRIX_COLUMNS)
                return -1;

        if (row < 0 || row >= MATRIX_ROWS)
                return -1;

        rotate = matrix->rotate % 4;

        // Portrait-landscape conversion not supported
        if (MATRIX_COLUMNS != MATRIX_ROWS) {
                if (rotate % 2 != 0) {
                        return -1;
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

        return matrix->content[c][r];
}

void matrix_refresh(matrix_t *matrix)
{
        if (matrix == NULL)
                return;

        int needs_refresh;

        needs_refresh = metronome_check(&matrix->refresh_clock);
        if (! needs_refresh)
                return;

        for (int i = 0; i < MATRIX_COLUMNS; i++) {
                uint32_t bits;

                bits = matrix_columns[i];
                for (int j = 0; j < MATRIX_ROWS; j++) {
                        int is_on;

                        is_on = matrix_apply_rotation(matrix, i, j);
                        if (is_on == -1)
                                continue;

                        if (is_on) {
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

