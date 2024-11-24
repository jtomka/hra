#include "event.h"

#include "metronome.h"

void metronome_init(metronome_t *metronome)
{
        if (metronome == NULL)
                return;

        metronome->clock = event_now();
}

bool metronome_status_update(metronome_t *metronome)
{
        clock_t now, diff, tick_len_passed;

        if (metronome == NULL)
                return false;

        now = event_now();
        diff = now - metronome->clock;
        tick_len_passed = diff > metronome->tick_len;

        if (tick_len_passed) {
                metronome->prev_clock = metronome->clock;
                metronome->clock = now;
        }

        return tick_len_passed;
}


