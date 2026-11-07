#include <assert.h>
#include <stdio.h>
#include "firmware/live.c"
volatile int fw_tempo=14400,fw_active_track=4; volatile uint32_t fw_timeline=0,fw_transport=0;
static unsigned offs;
void *stock_sound(void *p,int t){(void)t;return p;}
int stock_set_mode(void *p,int v){(void)p;return v;}
void fast_wake(void){}
void fast_wake_at(uint32_t t){(void)t;}
void fast_schedule(uint32_t t){(void)t;}
static int led_index,led_priority,led_palette;
void fw_arp_led_set(void *mgr,int led,int priority,int palette){(void)mgr;led_index=led;led_priority=priority;led_palette=palette;}
int midi_channel(int t){return t;}
void fw_note_on(int c,int n,int v){(void)c;(void)n;(void)v;}
void fw_note_off(int c,int n){(void)c;(void)n;++offs;}
int main(void){
    arp_restore(0);
    assert(arp_settings.version==3);
    for(int t=0;t<4;t++){
        arp_proxy *p=arp_sound(0,t+4);
        panel_set_mode(p,t+2);
        p->bytes[0x110]=t+4;
        p->bytes[0x111]=t+1;
        p->bytes[0x112]=20+t*15;
    }
    arp_capture();
    fw_active_track=4;
    panel_arp_leds(0);
    assert(led_index==19 && led_priority==2 && led_palette==31);
    uint32_t shortcut[5]={0,0,0,3,3};
    assert(panel_arp_shortcut(0,shortcut)==1);
    assert(!arp_enabled[0] && proxies[0].bytes[0x10f]==2);
    panel_arp_leds(0);
    assert(led_priority==1 && led_palette==2);
    arp_capture();
    arp_saved disk=arp_settings;
    for(int t=0;t<4;t++)panel_set_mode(&proxies[t],0);
    arp_settings=disk;
    arp_restore(1);
    for(int t=0;t<4;t++){
        assert(proxies[t].bytes[0x10f]==t+2);
        assert(proxies[t].bytes[0x110]==t+4);
        assert(proxies[t].bytes[0x111]==t+1);
        assert(proxies[t].bytes[0x112]==20+t*15);
    }
    assert(!arp_enabled[0] && arp_enabled[1] && arp_enabled[2] && arp_enabled[3]);
    assert(panel_arp_shortcut(0,shortcut)==1);
    assert(arp_enabled[0] && proxies[0].bytes[0x10f]==2);
    live_poll();
    assert(!project_changed);
    live_input(0,4,60,100);
    live_poll();
    for(int k=0;k<1200;k++){live_block(0);live_poll();}
    assert(engine.track[0].count==1);
    unsigned before=offs;
    arp_restore(0);
    assert(offs==before);
    live_poll();
    assert(!engine.track[0].count && !engine.track[0].playing);
    assert(offs>=before);
    arp_settings.version=999;
    arp_settings.track[0][15]=7;
    arp_restore(1);
    assert(!proxies[0].bytes[0x10f]);
    puts("PASS independent parameter restoration, new/unknown project defaults, deferred runtime reset");
}
