#ifndef BLINKER_H
#define BLINKER_H

#include <stdbool.h>

#include "ticker.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        uint16_t on_len;
        uint16_t off_len;
        bool status;

        ticker_t ticker;
} blinker_t;

extern bool blinker_check(blinker_t *blinker);

#ifdef __cplusplus
}
#endif

#endif /* BLINKER_H */

