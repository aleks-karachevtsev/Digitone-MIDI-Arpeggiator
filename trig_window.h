#ifndef DN144_TRIG_WINDOW_H
#define DN144_TRIG_WINDOW_H
#include "trig.h"
typedef struct {
    uint32_t deadline;
    uint8_t finite;
} dn144_window;

static inline dn144_window dn144_trig_window(const uint8_t *payload)
{
    uint32_t length = trig_word(payload + 0x1c);
    dn144_window window = {trig_word(payload + 0x24) + 2u * length, length != 0};
    return window;
}

static inline int dn144_window_done(dn144_window window, uint32_t timeline)
{
    return window.finite && (int32_t)(timeline - window.deadline) >= 0;
}
#endif
