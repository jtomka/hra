#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        clock_t clock;
        clock_t prev_clock;
        int tick_len;
} metronome_t ;

extern void metronome_init(metronome_t *metronome);
extern bool metronome_status_update(metronome_t *metronome);

#ifdef __cplusplus
}
#endif

