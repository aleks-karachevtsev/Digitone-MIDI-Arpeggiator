#ifndef DIGIARP_DN144_CLOCK_H
#define DIGIARP_DN144_CLOCK_H
#include <stdint.h>

/* Called once per main-CPU render block. Preserve phase across tempo edits.
 * 480 PPQN keeps all 18 stock SPEED fractions exact (whole note = 1920
 * ticks). Return every elapsed tick, including multiple ticks per block at
 * fast tempos. This routine performs no MIDI I/O. */
typedef struct { uint32_t remainder; } dn144_clock;
static inline int dn144_clock_block(dn144_clock *clock, int32_t tempo_x120)
{
    if (tempo_x120 <= 0 || tempo_x120 >= 450000)
        return 0;
    clock->remainder += (uint32_t)tempo_x120 * 40;
    uint32_t elapsed=clock->remainder / 900000u;
    clock->remainder %= 900000u;
    return (int)elapsed;
}
#endif
