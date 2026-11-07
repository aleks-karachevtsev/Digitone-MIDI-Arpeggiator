#include <assert.h>
#include <stdio.h>
#include "firmware/live.c"
volatile int fw_tempo=14400,fw_active_track=4;volatile uint32_t fw_timeline=0,fw_transport=0;
void *stock_sound(void *p,int t){(void)t;return p;}
int stock_set_mode(void *p,int v){(void)p;return v;}
void fast_wake(void){}void fast_wake_at(uint32_t t){(void)t;}void fast_schedule(uint32_t t){(void)t;}
void fw_arp_led_set(void *p,int a,int b,int c){(void)p;(void)a;(void)b;(void)c;}
int midi_channel(int t){return t;}void fw_note_on(int c,int n,int v){(void)c;(void)n;(void)v;}
void fw_note_off(int c,int n){(void)c;(void)n;}
int main(void){
 arp_restore(0);arp_proxy *p=arp_sound(0,4);uint32_t ev[5]={0,0,0,3,3};
 panel_set_mode(p,5);assert(arp_enabled[0]);
 assert(panel_arp_shortcut(0,ev)==1&&!arp_enabled[0]);
 panel_set_mode(p,6);assert(!arp_enabled[0]&&p->bytes[0x10f]==6);
 assert(panel_arp_shortcut(0,ev)==1&&arp_enabled[0]&&p->bytes[0x10f]==6);
 assert(live_input(0,4,60,100)==1&&key_owner[60]==1);
 assert(live_input(0,4,64,100)==1&&key_owner[64]==1);
 fw_active_track=5;assert(live_input(0,5,67,100)==0); /* B is disabled */
 assert(live_input(0,5,64,0)==1&&key_owner[64]==0&&!keys[0][64]&&keys[0][60]);
 assert(live_input(0,5,60,0)==1&&key_owner[60]==0&&!keys[0][60]);
 uint8_t trig[64]={0};assert(live_trig(trig,0)==0); /* transport STOP */
 fw_active_track=4;panel_set_mode(p,0);assert(!arp_enabled[0]);
 assert(panel_arp_shortcut(0,ev)==1&&arp_enabled[0]&&p->bytes[0x10f]==1);
 assert(panel_arp_shortcut(0,ev)==1&&!arp_enabled[0]&&p->bytes[0x10f]==1);
 puts("PASS mode persists across shortcut, note ownership survives track switch, STOP rejects trig, mode OFF toggles exactly once");
}
