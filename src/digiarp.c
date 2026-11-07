#include "digiarp.h"
static int valid(int t){return t>=0&&t<DA_TRACKS;}
static void stop(da_engine*e,int t){da_track*s=&e->track[t];if(s->playing){e->emit(e->user,t,s->active_channel,s->active_note,0,0);s->playing=0;}}
void da_init(da_engine*e,da_emit emit,void*user){for(unsigned i=0;i<sizeof(*e);i++) ((uint8_t *)e)[i]=0;e->emit=emit;e->user=user;for(int t=0;t<DA_TRACKS;t++){e->track[t].enabled=1;e->track[t].octaves=1;e->track[t].gate_percent=75;e->track[t].division_ticks=6;e->track[t].step_mask=0xffff;e->track[t].pattern_length=16;e->track[t].rng=(uint32_t)(0x1234567u+t*7321u);}}
void da_panic(da_engine*e){for(int t=0;t<DA_TRACKS;t++){stop(e,t);e->track[t].step=0;e->track[t].pattern_step=0;e->track[t].start_armed=0;e->track[t].next_start=0;e->track[t].count=0;}}
void da_clear_track(da_engine*e,int t){if(!valid(t))return;stop(e,t);e->track[t].step=0;e->track[t].pattern_step=0;e->track[t].start_armed=0;e->track[t].next_start=0;e->track[t].count=0;}
void da_steps(da_engine*e,int t,uint16_t mask,const int8_t offsets[16]){if(!valid(t))return;da_track*s=&e->track[t];s->step_mask=mask;for(int i=0;i<16;i++)s->offsets[i]=offsets[i];}
void da_pattern_length(da_engine*e,int t,int length){if(!valid(t))return;da_track*s=&e->track[t];s->pattern_length=(uint8_t)(length<1?1:length>16?16:length);if(s->pattern_step>=s->pattern_length)s->pattern_step=0;}
void da_config(da_engine*e,int t,da_mode mode,int octaves,int division_ticks,int gate_percent,int latch){if(!valid(t))return;da_track*s=&e->track[t];stop(e,t);s->mode=mode;s->octaves=(uint8_t)(octaves<1?1:octaves>8?8:octaves);s->division_ticks=(uint16_t)(division_ticks<1?1:division_ticks>1920?1920:division_ticks);s->gate_percent=(uint8_t)(gate_percent<1?1:gate_percent>99?99:gate_percent);s->latch=!!latch;s->step=0;s->pattern_step=0;s->start_armed=0;s->next_start=0;}
/* Encoder edits to SPEED/RANGE/LEN must not repeatedly restart the chord.
   Keep the phase and held notes; apply timing to the next onset and the
   current gate immediately. MODE changes still use da_config above. */
void da_update_timing(da_engine*e,int t,int octaves,int division_ticks,int gate_percent){
    if(!valid(t))return;
    da_track*s=&e->track[t];
    uint16_t division=(uint16_t)(division_ticks<1?1:division_ticks>1920?1920:division_ticks);
    uint8_t gate=(uint8_t)(gate_percent<1?1:gate_percent>99?99:gate_percent);
    if(s->start_armed && division!=s->division_ticks)
        s->next_start=s->next_start-s->division_ticks+division;
    s->division_ticks=division;
    s->octaves=(uint8_t)(octaves<1?1:octaves>8?8:octaves);
    s->gate_percent=gate;
    if(s->playing){
        uint32_t duration=(division*gate)/100;
        s->next_off=s->current_start+(duration?duration:1);
    }
}
void da_note_on(da_engine*e,int t,int channel,int note,int vel){(void)channel;if(!valid(t)||note<0||note>127||vel<1||vel>127)return;da_track*s=&e->track[t];for(int i=0;i<s->count;i++)if(s->notes[i]==note){s->held[i]=1;s->velocity[i]=(uint8_t)vel;return;}if(s->count>=DA_KEYS)return;int i=s->count++;s->notes[i]=(uint8_t)note;s->velocity[i]=(uint8_t)vel;s->held[i]=1;}
void da_note_off(da_engine*e,int t,int channel,int note){(void)channel;if(!valid(t))return;da_track*s=&e->track[t];for(int i=0;i<s->count;i++)if(s->notes[i]==note){s->held[i]=0;if(s->latch)return;for(int k=i;k+1<s->count;k++){s->notes[k]=s->notes[k+1];s->velocity[k]=s->velocity[k+1];s->held[k]=s->held[k+1];}s->count--;if(!s->count){
    stop(e,t);
    s->step=0;s->pattern_step=0;
    s->start_armed=0;
}return;}}
static int sorted_index(const da_track*s,int pos){int indices[DA_KEYS];for(int i=0;i<s->count;i++)indices[i]=i;for(int i=1;i<s->count;i++){int k=indices[i],j=i;while(j>0&&s->notes[indices[j-1]]>s->notes[k]){indices[j]=indices[j-1];j--;}indices[j]=k;}return indices[pos];}
static int select_note(da_track*s,int*vel){int n=s->count,oct=s->octaves,full=n*oct;if(!full)return -1;unsigned step=s->step;int pos=0;switch(s->mode){case DA_DOWN:pos=full-1-(step%full);break;case DA_UPDOWN:{int period=full<=1?1:2*full-2;int k=step%period;pos=k<full?k:period-k;break;}case DA_UPDOWN_REPEAT:{int period=full<=1?1:2*full;int k=step%period;pos=k<full?k:2*full-1-k;break;}case DA_RANDOM:s->rng=1664525u*s->rng+1013904223u;pos=(s->rng>>16)%full;break;case DA_AS_PLAYED:pos=step%full;break;case DA_UP_TWICE:pos=(step/2)%full;break;case DA_DOWN_TWICE:pos=full-1-((step/2)%full);break;default:pos=step%full;break;}
/* Keep phase inside the actual period, including across long performances. */
unsigned cycle = full;
switch (s->mode) {
case DA_UPDOWN: cycle = full > 1 ? 2 * full - 2 : 1; break;
case DA_UPDOWN_REPEAT:
case DA_UP_TWICE:
case DA_DOWN_TWICE: cycle = 2 * full; break;
default: break;
}
s->step = (uint8_t)((step + 1) % cycle);
int idx=s->mode==DA_AS_PLAYED?pos%n:sorted_index(s,pos%n);int note=s->notes[idx]+12*(pos/n);*vel=s->velocity[idx];return note<=127?note:-1;}
void da_tick(da_engine*e,uint32_t tick,const uint8_t channels[DA_TRACKS]){for(int t=0;t<DA_TRACKS;t++){da_track*s=&e->track[t];if(!s->enabled){stop(e,t);continue;}if(s->playing && (int32_t)(tick-s->next_off)>=0)stop(e,t);if(!s->count)continue;if(!s->start_armed){s->next_start=tick;s->start_armed=1;} if((int32_t)(tick-s->next_start)<0)continue;stop(e,t);int vel=0,note=select_note(s,&vel);unsigned pattern=s->pattern_step;s->pattern_step=(uint8_t)((pattern+1)%s->pattern_length);if(!(s->step_mask&(1u<<pattern)))note=-1;else if(note>=0){note+=s->offsets[pattern];if(note<0||note>127)note=-1;}int chan=channels[t];if(note>=0&&chan>=0&&chan<=15){e->emit(e->user,t,chan,note,vel,1);s->playing=1;s->active_note=(uint8_t)note;s->active_channel=(uint8_t)chan;s->active_velocity=(uint8_t)vel;s->current_start=tick;uint32_t duration=(s->division_ticks*s->gate_percent)/100;if(!duration)duration=1;s->next_off=tick+duration;}
/* Advance from the ideal beat grid, never from a delayed worker callback. */
uint32_t next=s->next_start+s->division_ticks;
if((int32_t)(tick-next)>=0)next+=((tick-next)/s->division_ticks+1)*s->division_ticks;
s->next_start=next;}}
