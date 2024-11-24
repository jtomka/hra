#ifndef TICKER_H
#define TICKER_H

#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        unsigned int span;

        clock_t clock;
} ticker_t ;

extern void ticker_init(ticker_t *ticker);
extern bool ticker_check(ticker_t *ticker);

#ifdef __cplusplus
}
#endif

#endif /* TICKER_H */

