#ifndef BLINKER_H
#define BLINKER_H

#include <stdbool.h>

#include "ticker.h"

typedef struct {
        uint16_t on_len;
        uint16_t off_len;
        bool status;

        ticker_t ticker;
} blinker_t;

extern void blinker_init(blinker_t *blinker);
extern bool blinker_check(blinker_t *blinker);

#endif /* BLINKER_H */

