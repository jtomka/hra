#ifndef __METRONOME_H__
#define __METRONOME_H__ 1

#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        int tick_len;
        clock_t clock;
        clock_t prev_clock;
} metronome_t ;

extern void metronome_init(metronome_t *metronome);
extern bool metronome_check(metronome_t *metronome);

#ifdef __cplusplus
}
#endif

#endif /* __METRONOME_H__ */

