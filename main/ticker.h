#ifndef TICKER_H
#define TICKER_H

#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        uint16_t span;

        uint16_t prev_span;
        clock_t clock;
} ticker_t ;

extern clock_t ticker_now();

extern bool ticker_check(ticker_t *ticker);

#ifdef __cplusplus
}
#endif

#endif /* TICKER_H */

