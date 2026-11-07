#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "firmware/live.c"
volatile int fw_tempo=14400,fw_active_track=4;
volatile uint32_t fw_timeline,fw_transport;
static int ons,offs,channel,onch[16],offch[16];
void *stock_sound(void *p,int t){(void)t;return p;}
int stock_set_mode(void *p,int v){(void)p;return v;}
int midi_channel(int t){return t==0?channel:t;}
void fw_note_on(int c,int n,int v){(void)n;(void)v;ons++;onch[c]++;}
void fw_note_off(int c,int n){(void)n;offs++;offch[c]++;}
int main(void){
    uint8_t payload[40]={0};payload[19]=1;payload[20]=60;payload[35]=100;payload[31]=80;
    arp_restore(0);live_poll();
    panel_set_mode(arp_sound(0,4),2);
    proxies[0].bytes[0x110]=9; /* stock SPD 1/16 */
    assert(live_trig(payload,0));live_poll();
    for(int n=0;n<250;n++){live_block(0);live_poll();}
    assert(ons>0);
    panel_set_mode(&proxies[0],0);live_poll();assert(ons==offs && !engine.track[0].count);
    panel_set_mode(&proxies[0],2);live_poll();
    for(int n=0;n<250;n++){live_block(0);live_poll();}
    assert(ons>offs);
    channel=1;live_poll();assert(onch[0]==offch[0]);
    arp_restore(0);live_poll();assert(!seq_notes[0][60] && ons==offs);
    panel_set_mode(&proxies[0],2);live_poll();
    for(int n=0;n<250;n++){live_block(0);live_poll();}
    assert(ons==offs);
    assert(live_trig(payload,1));fw_transport=1;live_poll();
    fw_transport=0;live_poll();
    assert(!seq_notes[0][60] && !engine.track[0].count && ons==offs);
    puts("PASS mode off, channel change, project restore clear active notes and stale trigs");
}
