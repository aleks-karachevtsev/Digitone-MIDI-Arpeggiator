#ifndef DN144_TRIG_H
#define DN144_TRIG_H
#include "src/digiarp.h"

/* Called when the sequencer consumes a prepared event. Addresses are offsets
 * in the MIDI payload. Copy the pitches before firmware recycles the event. */
static inline uint32_t trig_word(const uint8_t *p)
{
    return ((uint32_t)p[0]<<24) | ((uint32_t)p[1]<<16) | ((uint32_t)p[2]<<8) | p[3];
}

static inline void trig_end(da_engine *e, unsigned track)
{
    da_track *s = &e->track[track];
    s->latch = 0;
    while (s->count)
        da_note_off(e, track, 0, s->notes[s->count-1]);
    s->step = 0;
}

static inline int trig_begin(da_engine *e, const uint8_t *payload, uint32_t now)
{
    unsigned track = trig_word(payload+4), count = trig_word(payload+16);
    unsigned velocity = trig_word(payload+32);
    if (track >= DA_TRACKS || !count || count > 8 || !velocity || velocity > 127)
        return 0;
    for (unsigned i=0; i<count; ++i)
        if (payload[20+i] > 127)
            return 0;
    trig_end(e, track);
    for (unsigned i=0; i<count; ++i)
        da_note_on(e, track, 0, payload[20+i], velocity);
    e->track[track].next_start = now;
    e->track[track].start_armed = 1;
    return 1;
}
#endif
