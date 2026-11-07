#include "src/digiarp.h"
#include <assert.h>
#include <stdio.h>
static unsigned ons, offs;
static void capture(void *ctx,int track,int channel,int note,int velocity,int on){
 (void)ctx;(void)velocity;assert(track==0 && channel==2 && note==60);
 if(on)ons++;else offs++;
}
int main(void){
 const uint8_t channels[4]={2}; da_engine engine;
 da_init(&engine,capture,0);da_config(&engine,0,DA_UP,1,6,50,0);
 da_note_on(&engine,0,2,60,100);da_tick(&engine,0,channels);
 assert(ons==1 && offs==0);
 da_note_off(&engine,0,2,60);assert(ons==offs && engine.track[0].count==0);
 da_tick(&engine,6,channels);assert(ons==1 && offs==1);
 da_note_on(&engine,0,2,60,100);da_tick(&engine,100,channels);
 assert(ons==2 && offs==1);
 da_note_off(&engine,0,2,60);assert(ons==offs);
 puts("PASS live release, no retrigger after release, balanced Note Off");
}
