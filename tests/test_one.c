#include "src/digiarp.h"
#include <stdio.h>
#include <assert.h>
static int on,off;
static void emit(void*p,int t,int c,int n,int v,int e){(void)p;(void)t;(void)c;(void)v;assert(n==60);if(e)on++;else off++;}
int main(void){da_engine a;uint8_t ch[4]={0};int8_t z[16]={0};for(int m=0;m<8;m++){da_init(&a,emit,0);da_config(&a,0,(da_mode)m,1,10,70,0);da_steps(&a,0,0xffff,z);da_note_on(&a,0,0,60,100);on=off=0;for(int i=0;i<500;i++)da_tick(&a,i,ch);da_note_off(&a,0,0,60);assert(on==off&&on>=40);}puts("PASS all 8 modes repeat exact MIDI 60 with RANGE=1, zero offsets, matched off");}
