#include <stdint.h>

/* The worker runs on the firmware's MIDI task, not in the render interrupt.
   Live panel keys and recorded trigs share the same engine and settings. */
extern void live_poll(void);
extern int fast_active(void);
extern int live_trig(const uint8_t *payload,int channel);

void dn144_worker_poll(void)
{
    if(!fast_active())live_poll();
}

int dn144_worker_accept(const uint8_t *payload,int channel)
{
    return live_trig(payload,channel);
}
