#include <assert.h>
#include "trig_window.h"
int main(void)
{
    uint8_t payload[40] = { [31] = 12, [36] = 255, [37] = 255, [38] = 255, [39] = 240 };
    dn144_window window = dn144_trig_window(payload);
    assert(window.deadline == 8);
    assert(!dn144_window_done(window, UINT32_MAX));
    assert(!dn144_window_done(window, 7));
    assert(dn144_window_done(window, 8));
    payload[31] = 0;
    assert(!dn144_window_done(dn144_trig_window(payload), 8));
}
