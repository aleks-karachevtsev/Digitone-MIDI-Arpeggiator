#include "src/digiarp.h"
#include <assert.h>
#include <stdio.h>
static int note[32],ons,offs;
static void output(void *p,int track,int channel,int pitch,int vel,int on){
 (void)p;(void)vel;assert(track==0&&channel==0);
 if(on){assert(ons<32);note[ons++]=pitch;}else{assert(offs<ons);offs++;}
}
int main(void){
 da_engine e;const uint8_t ch[4]={0};da_init(&e,output,0);
 da_config(&e,0,DA_UP_TWICE,1,10,50,0);
 da_note_on(&e,0,0,60,100);da_note_on(&e,0,0,64,100);
 da_tick(&e,0,ch);assert(ons==1&&note[0]==60&&e.track[0].step==1);
 da_update_timing(&e,0,2,10,90);
 assert(e.track[0].step==1&&e.track[0].count==2&&e.track[0].playing);
 da_tick(&e,9,ch);assert(offs==1);
 da_tick(&e,10,ch);assert(ons==2&&note[1]==60);
 da_update_timing(&e,0,2,5,50);
 da_tick(&e,12,ch);assert(offs==2);
 da_tick(&e,15,ch);assert(ons==3&&note[2]==64);
 for(int tick=16;tick<=30;tick++)da_tick(&e,tick,ch);
 assert(ons>=6&&note[3]==64&&note[4]==72&&note[5]==72);
 da_panic(&e);assert(ons==offs);
 puts("PASS dynamic SPEED/RANGE/LEN preserve phase, exact MIDI pitches and balanced releases");
}
