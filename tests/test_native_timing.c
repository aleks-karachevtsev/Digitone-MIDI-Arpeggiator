#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "firmware/live.c"
volatile int fw_tempo=14400,fw_active_track=4;
volatile uint32_t fw_timeline,fw_transport;
static uint32_t ons_at[128],offs_at[128];
static int ons,offs;
void *stock_sound(void *p,int t){(void)t;return p;}
int stock_set_mode(void *p,int v){(void)p;return v;}
int midi_channel(int t){return t;}
void fw_note_on(int c,int n,int v){assert(c==0&&n>=60&&n<=127&&v==100);ons_at[ons++]=ticks;}
void fw_note_off(int c,int n){assert(c==0&&n>=60&&n<=127);offs_at[offs++]=ticks;}
int main(void){
    static const int denominator[18]={1,2,3,4,5,6,8,10,12,16,20,24,32,40,48,64,80,96};
    arp_restore(0);live_poll();panel_set_mode(arp_sound(0,4),2);live_keys=1;
    for(int i=0;i<18;i++){
        proxies[0].bytes[0x110]=(uint8_t)i;
        live_poll();assert(engine.track[0].division_ticks==1920/denominator[i]);
    }
    proxies[0].bytes[0x110]=9; /* 1/16 at 120 BPM: 120 ticks = 125ms */
    proxies[0].bytes[0x111]=7; /* 8 octaves */
    proxies[0].bytes[0x112]=64;
    live_poll();assert(engine.track[0].octaves==8);
    assert(engine.track[0].gate_percent==(64*98/127)+1);
    proxies[0].bytes[0x111]=0;live_poll();
    assert(live_input(0,4,60,100));live_poll();
    for(int i=0;i<3000;i++){live_block(0);live_poll();}
    assert(ons>=8 && offs>=7);
    for(int i=1;i<8;i++)assert(ons_at[i]-ons_at[i-1]==120);
    for(int i=0;i<7;i++)assert(offs_at[i]-ons_at[i]==
        (uint32_t)(120*engine.track[0].gate_percent/100));
    live_input(0,4,60,0);live_poll();assert(ons==offs);
    puts("PASS 18 stock speed divisions, 480 PPQN, 120 BPM spacing, LEN gate, RANGE and balanced Note Off");
}
