#include "clock.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    dn144_clock clock = {0};
    const int bpm[] = {30, 87, 120, 137, 300, 92};
    uint64_t elapsed = 0;
    uint32_t ticks = 0;
    for (unsigned tempo = 0; tempo < sizeof(bpm)/sizeof(bpm[0]); tempo++) {
        for (unsigned block = 0; block < 100003; block++) {
            elapsed += bpm[tempo] * 4800;
            ticks += dn144_clock_block(&clock, bpm[tempo] * 120);
            assert(ticks == elapsed / 900000);
            assert(clock.remainder == elapsed % 900000);
        }
    }
    puts("PASS: 600018 render-block inputs, six tempos, phase preserved");
}
