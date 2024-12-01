#include <errno.h>

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

int8_t matrix_clear(matrix_t *matrix)
{
        if (matrix == NULL) {
                errno = EFAULT;
                return -1;
        }

        for (int i = 0; i < MATRIX_COLUMNS; i++) {
                for (int j = 0; j < MATRIX_ROWS; j++) {
                        matrix_pixel_t *pixel;

                        pixel = &matrix->pixels[i][j];
                        pixel->status = false;
                        pixel->brightness = 100;
                }
        }

        return 0;
}

int8_t matrix_init(matrix_t *matrix)
{
        int8_t retval;

        if (matrix == NULL) {
                errno = EFAULT;
                return -1;
        }

        matrix->brightness = 100;
        matrix->prev_brightness = 100 - matrix->brightness;

        matrix->ic74hc595.signal_pin = matrix->signal_pin;
        matrix->ic74hc595.clock_pin = matrix->clock_pin;
        matrix->ic74hc595.latch_pin = matrix->latch_pin;
        matrix->ic74hc595.send_mode = IC74HC595_SEND_MODE_MSB_FIRST;

        retval = ic74hc595_init(&matrix->ic74hc595);
        if (retval == -1) {
                return -1;
        }

        retval = matrix_clear(matrix);
        if (retval == -1) {
                return -1;
        }

        return 0;
}

static inline matrix_pixel_t *matrix_apply_rotation_get_pixel(
                        matrix_t *matrix, uint8_t column, uint8_t row)
{
        uint8_t rotation;
        uint8_t c, r;

        if (matrix == NULL) {
                errno = EFAULT;
                return NULL;
        }
        if (column >= MATRIX_COLUMNS) {
                errno = EINVAL;
                return NULL;
        }
        if (row >= MATRIX_ROWS) {
                errno = EINVAL;
                return NULL;
        }

        rotation = matrix->rotation;
        if (rotation > 3) {
                rotation = rotation % 4;
        }
        // Portrait-landscape conversion not supported
        if (MATRIX_COLUMNS != MATRIX_ROWS) {
                if (rotation == 1 || rotation == 3) {
                        errno = ENOTSUP;
                        return NULL;
                }
        }
                
        c = column;
        r = row;

        if (rotation == 1) {
                c = row;
                r = MATRIX_COLUMNS - column - 1;

        } else if (rotation == 2) {
                c = MATRIX_COLUMNS - column - 1;
                r = MATRIX_ROWS - row - 1;

        } else if (rotation == 3) {
                c = MATRIX_ROWS - row - 1;
                r = column;
        }

        return &matrix->pixels[c][r];
}

static inline int8_t matrix_get_pixel_brightness_duty_cycle(
                        matrix_t *matrix, matrix_pixel_t *pixel)
{
        double matrix_brightness;
        double combined_brightness;
        uint8_t adj_min, adj_max;

        if (matrix == NULL) {
                errno = EFAULT;
                return -1;
        }
        if (pixel == NULL) {
                errno = EFAULT;
                return -1;
        }

        if (matrix->brightness == 0) {
                return 0;
        }
        if (pixel->brightness == 0) {
                return 0;
        }

        adj_min = matrix->brightness_adj_min;
        adj_max = matrix->brightness_adj_max;

        if (adj_max == 0) {
                adj_max = 100;
        }
        if (adj_max > 100) {
                adj_max = 100;
        }
        if (adj_min >= adj_max) {
                errno = EINVAL;
                return -1;
        }

        matrix_brightness = adj_max - adj_min;
        matrix_brightness *= (double) matrix->brightness / 100;
        matrix_brightness += adj_min;

        combined_brightness = pixel->brightness * matrix_brightness / 100.0;

        return combined_brightness;
}

static inline bool matrix_apply_brightness(matrix_t *matrix, matrix_pixel_t *pixel)
{
        bool matrix_brightness_changed;
        bool pixel_brightness_changed;
        pwm_t *pixel_pwm;
        bool brightness_status;

        if (matrix == NULL) {
                errno = EFAULT;
                return -1;
        }
        if (pixel == NULL) {
                errno = EFAULT;
                return -1;
        }

        matrix_brightness_changed = false;
        if (matrix->brightness != matrix->prev_brightness) {
                matrix_brightness_changed = true;
        }

        pixel_brightness_changed = false;
        if (pixel->brightness != pixel->prev_brightness) {
                pixel_brightness_changed = true;
        }

        pixel_pwm = &pixel->brightness_pwm;
        if (matrix_brightness_changed || pixel_brightness_changed) {
                int8_t retval;

                retval = matrix_get_pixel_brightness_duty_cycle(matrix, pixel);
                if (retval == -1) {
                        return -1;
                }
                pixel_pwm->duty_cycle = retval;
        }

        brightness_status = pwm_check(pixel_pwm);
        return brightness_status;
}

int8_t matrix_refresh(matrix_t *matrix)
{
static clock_t now = 0;
static uint32_t cycles = 0;

if (ticker_now() - now > 10000) {
        printf("%f Hz\n", cycles / 10.0);
        now = ticker_now();
        cycles = 0;
}
cycles++;

        bool matrix_brightness_changed;

        if (matrix == NULL) {
                errno = EINVAL;
                return -1;
        }

        matrix_brightness_changed = false;
        if (matrix->prev_brightness != matrix->brightness) {
                matrix_brightness_changed = true;
        }

        for (int i = 0; i < MATRIX_COLUMNS; i++) {
                uint32_t bits;
                int retval;

                bits = matrix_columns[i];
                for (int j = 0; j < MATRIX_ROWS; j++) {
                        matrix_pixel_t *pixel;
                        bool pixel_status;
                        int8_t brightness_status;

                        pixel = matrix_apply_rotation_get_pixel(matrix, i, j);
                        if (pixel == NULL) {
                                return -1;
                        }
                        pixel_status = pixel->status;

                        brightness_status = matrix_apply_brightness(matrix, pixel);
                        if (brightness_status == -1) {
                                return -1;
                        }

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
                        retval = ic74hc595_send8bits(&matrix->ic74hc595,
                            ((uint8_t *) &bits)[k - 1]);
                        if (retval == -1) {
                                return -1;
                        }
                }
                retval = ic74hc595_latch(&matrix->ic74hc595);
                if (retval == -1) {
                        return -1;
                }
        }

        if (matrix_brightness_changed) {
                matrix->prev_brightness = matrix->brightness;
        }

        return 0;
}

int8_t matrix_block(matrix_t *matrix, uint16_t x1, uint16_t y1,
                  uint16_t x2, uint16_t y2, bool fill)
{
        if (matrix == NULL) {
                errno = EFAULT;
                return -1;
        }

        if (x1 >= MATRIX_COLUMNS) {
                errno = EINVAL;
                return -1;
        }

        if (y1 >= MATRIX_ROWS) {
                errno = EINVAL;
                return -1;
        }

        if (x2 >= MATRIX_COLUMNS) {
                errno = EINVAL;
                return -1;
        }

        if (y2 >= MATRIX_ROWS) {
                errno = EINVAL;
                return -1;
        }

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

        return 0;
}

