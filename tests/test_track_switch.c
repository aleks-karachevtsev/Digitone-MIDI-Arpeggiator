#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "firmware/live.c"
volatile int fw_tempo=10320,fw_active_track=4;
volatile uint32_t fw_timeline,fw_transport;
void fast_wake(void){}
static int ons,offs;
void *stock_sound(void *p,int t){(void)t;return p;}
int stock_set_mode(void *p,int v){(void)p;return v;}
int midi_channel(int t){return t;}
void fw_note_on(int c,int n,int v){(void)c;(void)n;(void)v;ons++;}
void fw_note_off(int c,int n){(void)c;(void)n;offs++;}
int main(void){
    arp_restore(0);live_poll();panel_set_mode(arp_sound(0,4),2);
    assert(live_input(0,4,60,100));live_poll();assert(ons==1);
    fw_active_track=5;
    assert(live_input(0,5,60,0));live_poll();
    assert(!keys[0][60] && !engine.track[0].count && ons==offs);
    fw_active_track=0;
    assert(live_input(0,4,62,100));live_poll();assert(ons==offs+1);
    assert(live_input(0,0,62,0));live_poll();
    assert(!keys[0][62] && ons==offs);
    puts("PASS note release tracks physical key ownership across MIDI and synth track selection");
}
