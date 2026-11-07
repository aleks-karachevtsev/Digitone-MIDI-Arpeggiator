#include "src/digiarp.h"
#include <assert.h>
#include <stdio.h>
static int start=-1, end=-1, now;
static void emit(void*p,int t,int c,int n,int v,int on){(void)p;(void)t;(void)c;(void)n;(void)v;if(on){assert(start<0);start=now;}else{assert(end<0);end=now;}}
int main(void){da_engine a;uint8_t ch[4]={0};int8_t z[16]={0};int divs[]={20,80,320},gates[]={10,50,90};for(int i=0;i<3;i++)for(int j=0;j<3;j++){da_init(&a,emit,0);da_config(&a,0,DA_UP,1,divs[i],gates[j],0);da_steps(&a,0,0xffff,z);da_note_on(&a,0,0,60,100);start=end=-1;for(now=0;now<divs[i];now++)da_tick(&a,now,ch);assert(start==0&&end==(divs[i]*gates[j])/100);da_panic(&a);}puts("PASS gate 10/50/90 percent at 3 divisions: exact scheduled lengths");}
