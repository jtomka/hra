
#include "blinker.h"

static uint16_t blinker_get_off_len(blinker_t *blinker)
{
        uint16_t off_len;

        if (blinker == NULL)
                return 0;

        off_len = blinker->off_len;
        if (off_len == 0) {
                off_len = blinker->on_len;
        }

        return off_len;
}

void blinker_init(blinker_t *blinker)
{
        uint16_t off_len;

        if (blinker == NULL)
                return;

        off_len = blinker_get_off_len(blinker);
        blinker->ticker.span = blinker->status ? blinker->on_len : off_len;
        ticker_init(&blinker->ticker);
}

bool blinker_check(blinker_t *blinker)
{
        uint16_t off_len;
        bool time_for_change;

        if (blinker == NULL)
                return false;

        // No blinking
        if (blinker->on_len == 0)
                return true;

        off_len = blinker_get_off_len(blinker);
        time_for_change = ticker_check(&blinker->ticker);
        if (time_for_change) {
                blinker->status = ! blinker->status;
                blinker->ticker.span = blinker->status ? blinker->on_len : off_len;
        }

        return blinker->status;
}

