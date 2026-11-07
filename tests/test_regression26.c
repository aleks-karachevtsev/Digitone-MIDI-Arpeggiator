#include "src/digiarp.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int notes[512], times[512], on_count, off_count, now;
static void emit(void *unused,int track,int ch,int n,int vel,int on){
 (void)unused;(void)track;(void)ch;(void)vel;
 if(on){assert(on_count<512);notes[on_count]=n;times[on_count++]=now;}
 else off_count++;
}
static void advance(da_engine *e,int first,int end){uint8_t channels[4]={0};for(now=first;now<end;now++)da_tick(e,now,channels);}
static void reset(da_engine *e){da_init(e,emit,0);on_count=off_count=0;}
int main(void){
 da_engine e;int8_t offsets[16]={0};
 for(int length=1;length<=16;length++){
  reset(&e);da_config(&e,0,DA_UP,1,10,50,0);
  for(int i=0;i<16;i++)offsets[i]=(int8_t)i;
  da_steps(&e,0,0xffff,offsets);da_pattern_length(&e,0,length);da_note_on(&e,0,0,60,100);
  advance(&e,0,20*10);assert(on_count==20);
  for(int i=0;i<on_count;i++){assert(notes[i]==60+i%length);assert(times[i]==10*i);}
  da_note_off(&e,0,0,60);assert(on_count==off_count);
 }
 reset(&e);memset(offsets,0,sizeof(offsets));offsets[1]=7;
 da_config(&e,0,DA_UP,1,10,50,0);da_steps(&e,0,0xfffd,offsets);da_pattern_length(&e,0,4);
 da_note_on(&e,0,0,60,100);advance(&e,0,45);
 assert(on_count==4 && notes[0]==60 && notes[1]==60 && notes[2]==60 && notes[3]==60);
 assert(times[0]==0 && times[1]==20 && times[2]==30 && times[3]==40);
 da_pattern_length(&e,0,2);advance(&e,45,80);assert(on_count==5 && times[4]==60);
 da_note_off(&e,0,0,60);assert(on_count==off_count);
 memset(offsets,0,sizeof(offsets));
 for(int mode=0;mode<8;mode++){
  reset(&e);da_config(&e,0,(da_mode)mode,1,10,50,0);da_steps(&e,0,0xffff,offsets);
  da_pattern_length(&e,0,16);da_note_on(&e,0,0,60,100);advance(&e,0,200);
  assert(on_count==20);for(int i=0;i<on_count;i++)assert(notes[i]==60);
  da_note_off(&e,0,0,60);assert(off_count==on_count);
 }
 for(int count=1;count<=8;count++)for(int release=0;release<count;release++){
  reset(&e);da_config(&e,0,DA_UP,1,10,50,0);
  for(int i=0;i<count;i++)da_note_on(&e,0,0,60+i,100);
  advance(&e,0,30);da_note_off(&e,0,0,60+release);
  assert(e.track[0].count==count-1);advance(&e,30,140);
  if(count>1){assert(on_count>=10);for(int i=3;i<on_count;i++)assert(notes[i]!=60+release);}
  for(int i=0;i<count;i++)if(i!=release)da_note_off(&e,0,0,60+i);
  assert(e.track[0].count==0 && on_count==off_count);
 }
 puts("PASS lengths 1..16; rests and offsets; live length change; one key eight modes; chord release 1..8");
}
