#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "firmware/live.c"
volatile int fw_tempo=14400,fw_active_track=4;
volatile uint32_t fw_timeline,fw_transport;
static int ons,offs;
void *stock_sound(void *p,int t){(void)t;return p;}
int stock_set_mode(void *p,int v){(void)p;return v;}
void fast_wake(void){}
void fast_wake_at(uint32_t t){(void)t;}
void fast_schedule(uint32_t t){(void)t;}
void fw_arp_led_set(void *p,int l,int priority,int palette){(void)p;(void)l;(void)priority;(void)palette;}
int midi_channel(int t){return t;}
void fw_note_on(int c,int n,int v){assert(c==0&&n==60&&v==100);ons++;}
void fw_note_off(int c,int n){assert(c==0&&n==60);offs++;}
int main(void){
    uint8_t payload[40]={0};
    payload[19]=1;payload[20]=60;payload[35]=100;
    payload[31]=80;
    arp_restore(0);
    live_poll();
    panel_set_mode(arp_sound(0,4),2);
    proxies[0].bytes[0x110]=9; /* stock SPD 1/16 */
    assert(live_trig(payload,0)==1);
    live_poll();
    for(int n=0;n<1800;n++){live_block(0);live_poll();}
    assert(ons>3 && offs>2);
    fw_timeline=200;live_poll();
    assert(!engine.track[0].count && ons==offs);
    puts("PASS sequencer chord through panel-configured engine, clocked output, note window expiry");
}
