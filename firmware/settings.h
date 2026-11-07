#ifndef DIGIARP_SETTINGS_H
#define DIGIARP_SETTINGS_H
#include <stdint.h>
#define ARP_RECORD_BYTES 64
typedef struct {
    uint32_t version;
    uint8_t track[4][ARP_RECORD_BYTES];
    uint8_t enabled[4];
} arp_saved;
extern arp_saved arp_settings;
void arp_restore(int found);
void arp_capture(void);
#endif
