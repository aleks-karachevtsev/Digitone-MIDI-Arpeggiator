#include "src/digiarp.h"
#include <assert.h>
#include <stdio.h>
static int pitches[64],ons,offs;
static void out(void *ctx,int tr,int ch,int n,int v,int on){(void)ctx;(void)tr;(void)ch;(void)v;if(on)pitches[ons++]=n;else {assert(offs<ons);assert(n==pitches[offs]);offs++;}}
int main(void){
 da_engine e;uint8_t ch[4]={0};int8_t offsets[16]={0};
 da_init(&e,out,0);da_config(&e,0,DA_UP,2,10,50,0);
 offsets[0]=5;offsets[2]=-7;
 da_steps(&e,0,0xfffd,offsets);
 da_note_on(&e,0,0,60,100);
 for(int tick=0;tick<40;tick++)da_tick(&e,tick,ch);
 assert(ons==3&&offs==3);
 fprintf(stderr,"pitches %d %d %d ons %d offs %d\n",pitches[0],pitches[1],pitches[2],ons,offs);
 assert(pitches[0]==65&&pitches[1]==53&&pitches[2]==72);
 da_steps(&e,0,0,offsets);
 for(int tick=40;tick<200;tick++)da_tick(&e,tick,ch);
 assert(ons==3&&offs==3);
 da_steps(&e,0,0xffff,offsets);
 for(int tick=200;tick<240;tick++)da_tick(&e,tick,ch);
 assert(ons==7&&offs==7);
 da_note_off(&e,0,0,60);da_panic(&e);assert(ons==offs);
 puts("PASS step mask, signed MIDI pitches, silent slots preserve timing, balanced off");
}
