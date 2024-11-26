
#include "ticker.h"

clock_t ticker_now() {
        return (clock_t) ((clock() * 1000) / CLOCKS_PER_SEC);
}

void ticker_init(ticker_t *ticker)
{
        if (ticker == NULL)
                return;

        ticker->clock = ticker_now();
}

bool ticker_check(ticker_t *ticker)
{
        clock_t now, diff;
        bool span_passed;

        if (ticker == NULL)
                return false;

        now = ticker_now();
        diff = now - ticker->clock;
        span_passed = diff > ticker->span;

        if (span_passed) {
                ticker->clock = now;
        }

        // We needed to update the clock even if the ticker is off
        if (ticker->span == 0) {
                return false;
        }

        return span_passed;
}

