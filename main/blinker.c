#include "blinker.h"

static inline uint16_t blinker_get_off_len(blinker_t *blinker)
{
        uint16_t off_len;

        if (blinker == NULL) {
                return 0;
        }

        off_len = blinker->off_len;
        if (off_len == 0) {
                off_len = blinker->on_len;
        }

        return off_len;
}

bool blinker_check(blinker_t *blinker)
{
        uint16_t off_len;
        bool needs_toggle;

        if (blinker == NULL) {
                return false;
        }

        // No blinking
        if (blinker->on_len == 0) {
                return true;
        }

        off_len = blinker_get_off_len(blinker);
        needs_toggle = ticker_check(&blinker->ticker);
        if (needs_toggle) {
                blinker->status = ! blinker->status;
                blinker->ticker.span = blinker->status ? blinker->on_len : off_len;
        }

        return blinker->status;
}

