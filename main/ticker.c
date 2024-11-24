#include "event.h"

#include "ticker.h"

void ticker_init(ticker_t *ticker)
{
        if (ticker == NULL)
                return;

        ticker->clock = event_now();
}

bool ticker_check(ticker_t *ticker)
{
        clock_t now, diff, span_passed;

        if (ticker == NULL)
                return false;

        now = event_now();
        diff = now - ticker->clock;
        span_passed = diff > ticker->span;

        if (span_passed) {
                ticker->clock = now;
        }

        return span_passed;
}


