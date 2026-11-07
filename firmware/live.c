#include "../clock.h"
#include "settings.h"
#include "../trig_window.h"
#include "../src/digiarp.h"
#include <stdint.h>

/* Only the ARP view's five sound selectors are redirected. All other views
   and all four internal sound objects keep the stock implementation. */
typedef struct { void **vtable; uint8_t bytes[0x328]; } arp_proxy;
static arp_proxy proxies[4];
static void *methods[32];
extern void fast_wake(void);
static void *sound(arp_proxy *p){return p->bytes;}
static volatile int settings_dirty;
static volatile int ui_needs_redraw;
/* The stock UI calls this from an encoder's synchronous setter. Keep that
   path constant time; the worker copies the saved record after the event. */
static int ignore(void){settings_dirty=1;ui_needs_redraw=1;fast_wake();return 0;}
void ui_arp_redraw(void *ctrl){
    if(ui_needs_redraw){
        ((volatile uint8_t *)ctrl)[0x20]=1; /* stock UI invalidation flag */
        ui_needs_redraw=0;
    }
}
extern void *stock_sound(void *,int);
extern int midi_channel(int);
extern volatile int fw_tempo;
extern volatile uint32_t fw_timeline;
extern volatile uint32_t fw_transport;
extern void fast_wake_at(uint32_t);
extern void fast_schedule(uint32_t);
extern void fw_note_on(int,int,int),fw_note_off(int,int);
void *arp_sound(void *owner,int track)
{
    if((unsigned)(track-4)>=4) return stock_sound(owner,track);
    arp_proxy *p=&proxies[track-4];
    if(!p->vtable){
        for(int i=0;i<32;i++) methods[i]=(void *)ignore;
        methods[10]=(void *)sound;
        p->vtable=methods;
    }
    return p;
}
static volatile uint8_t keys[4][128];
/* Keep the owner of a physical key when the selected track changes. */
static volatile uint8_t key_owner[128]; /* 0: none, 1..4: MIDI A..D */
static volatile uint8_t arp_enabled[4];
static uint8_t sounding_input[4][128],channels[4],last_mode[4];
static volatile unsigned live_keys;
static volatile uint8_t pending[4];
static uint8_t seq_notes[4][128], seq_velocity[4][128];
static dn144_window seq_window[4];
static uint8_t prior_config[4][5];
static volatile uint32_t ticks;
static dn144_clock clock_state;
static da_engine engine;
static int ready;
static volatile int project_changed;
static int transport_was_running;
static uint8_t captured[4][4];
static int captured_valid;
static void capture_changed(void){
    int changed=!captured_valid || settings_dirty;
    settings_dirty=0;
    for(unsigned t=0;t<4;t++)for(unsigned j=0;j<4;j++){
        uint8_t value=proxies[t].bytes[0x10f+j];
        if(captured[t][j]!=value){captured[t][j]=value;changed=1;}
    }
    if(changed){arp_capture();captured_valid=1;}
}
void live_block(void *words){
    (void)words;
    int elapsed=dn144_clock_block(&clock_state,fw_tempo);
    if(elapsed){ticks+=(uint32_t)elapsed;fast_wake_at(ticks);}
}
int live_input(int source,int track,int note,int velocity){
    if(source || (unsigned)note>=128) return 0;
    /* A physical key may be released after the panel changes tracks. The
       release belongs to the track that received its Note On. */
    if(!velocity){
        int handled=0;
        unsigned owner=key_owner[note];
        if(owner && owner<=4){
            int i=(int)owner-1;
            keys[i][note]=0;pending[i]=1;handled=1;key_owner[note]=0;
        }
        if(handled)fast_wake();
        return handled;
    }
    if((unsigned)(track-4)>=4)return 0;
    int t=track-4;
    if(!arp_enabled[t] && !keys[t][note]) return 0;
    unsigned owner=key_owner[note];
    if(owner && owner!=(unsigned)(t+1)){
        keys[owner-1][note]=0;pending[owner-1]=1;
    }
    key_owner[note]=(uint8_t)(t+1);
    keys[t][note]=(uint8_t)velocity;
    pending[t]=1;
    live_keys=1;
    fast_wake();
    return 1;
}
static void emit(void *p,int t,int ch,int n,int v,int on){
    (void)p;(void)t;
    if(on)fw_note_on(ch,n,v);else fw_note_off(ch,n);
}
void live_poll(void){
    int transport_running=(fw_transport==1);
    /* A queued sequencer trig may arrive after STOP, or during an already
       stopped boot. It must never join a live keyboard chord. */
    if(!transport_running){
        for(unsigned t=0;t<4;t++){
            if(seq_window[t].finite || transport_was_running){
                seq_window[t].finite=0;
                for(unsigned n=0;n<128;n++)seq_notes[t][n]=0;
                pending[t]=1;
            }
        }
    }
    transport_was_running=transport_running;
    if(project_changed){
        if(ready)da_panic(&engine);
        ready=0;live_keys=0;project_changed=0;
        for(unsigned t=0;t<4;t++){
            last_mode[t]=0;pending[t]=0;seq_window[t].finite=0;
            for(unsigned n=0;n<128;n++){keys[t][n]=0;sounding_input[t][n]=0;seq_notes[t][n]=0;seq_velocity[t][n]=0;}
        }
        for(unsigned n=0;n<128;n++)key_owner[n]=0;
    }
    capture_changed();
    for(unsigned t=0;t<4;t++){
        if(seq_window[t].finite && dn144_window_done(seq_window[t],fw_timeline)){
            for(unsigned n=0;n<128;n++)seq_notes[t][n]=0;
            seq_window[t].finite=0;pending[t]=1;
        }
    }
    if(!live_keys){fast_schedule(ticks+16);return;}
    if(!ready){da_init(&engine,emit,0);ready=1;}
    for(int t=0;t<4;t++){
        const uint8_t *steps=&proxies[t].bytes[0x114];
        uint16_t mask=((uint16_t)steps[0]<<8)|steps[1];
        da_steps(&engine,t,mask,(const int8_t *)(steps+2));
        da_pattern_length(&engine,t,(int)proxies[t].bytes[0x113]+1);
        unsigned mode=arp_enabled[t]?proxies[t].bytes[0x10f]:0;
        if(!mode && !last_mode[t])continue;
        int ch=midi_channel(t);
        if(ch<0 || ch>15)ch=255;
        uint8_t speed=proxies[t].bytes[0x110];
        uint8_t octave=proxies[t].bytes[0x111];
        uint8_t gate=proxies[t].bytes[0x112];
        if(mode!=last_mode[t] || ch!=channels[t] ||
            prior_config[t][0]!=speed || prior_config[t][1]!=octave ||
            prior_config[t][2]!=gate){
            pending[t]=1;
            if(!mode || ch!=channels[t]){
                da_clear_track(&engine,t);
                for(unsigned n=0;n<128;n++)sounding_input[t][n]=0;
            }
            static const da_mode modes[]={DA_UP,DA_AS_PLAYED,DA_UP,DA_DOWN,DA_UPDOWN,DA_UP_TWICE,DA_DOWN_TWICE,DA_UPDOWN_REPEAT};
            da_mode m=modes[mode & 7];
            static const uint8_t denominator[18]={1,2,3,4,5,6,8,10,12,16,20,24,32,40,48,64,80,96};
            int division=1920/denominator[speed<18?speed:17];
            int percent=gate ? (gate*98/127)+1 : 75;
            if(mode!=last_mode[t] || ch!=channels[t])
                da_config(&engine,t,m,octave<7 ? octave+1 : 8,division,percent,0);
            else
                da_update_timing(&engine,t,octave<7 ? octave+1 : 8,division,percent);
            prior_config[t][0]=speed;prior_config[t][1]=octave;prior_config[t][2]=gate;
            last_mode[t]=mode;channels[t]=ch;
        }
        if(!pending[t])continue;
        pending[t]=0;
        for(int n=0;n<128;n++){
            unsigned v=mode?(keys[t][n]?keys[t][n]:seq_velocity[t][n]*!!seq_notes[t][n]):0;
            if(v==sounding_input[t][n])continue;
            sounding_input[t][n]=v;
            if(v)da_note_on(&engine,t,ch,n,v);else da_note_off(&engine,t,ch,n);
        }
    }
    da_tick(&engine,ticks,channels);
    uint32_t deadline=ticks+16;
    for(int t=0;t<4;t++){
        da_track *state=&engine.track[t];
        if(state->playing && (int32_t)(state->next_off-deadline)<0)
            deadline=state->next_off;
        if(state->count && state->start_armed &&
           (int32_t)(state->next_start-deadline)<0)
            deadline=state->next_start;
    }
    fast_schedule(deadline);
}

extern int stock_set_mode(void *,int);
void panel_set_mode(void *obj,int value)
{
    for(int i=0;i<4;i++){
        if(obj!=&proxies[i])continue;
        unsigned old_mode=proxies[i].bytes[0x10f];
        proxies[i].bytes[0x10f]=value<0?0:value>7?7:value;
        /* Editing a non-OFF mode must not silently re-enable a track that
           the hardware shortcut left disabled. */
        if(!proxies[i].bytes[0x10f])arp_enabled[i]=0;
        else if(!old_mode)arp_enabled[i]=1;
        settings_dirty=1;
        ui_needs_redraw=1;
        fast_wake();
        return;
    }
    stock_set_mode(obj,value);
}
extern volatile int fw_active_track;
extern void fw_arp_led_set(void *manager,int led,int priority,int palette);
void panel_arp_leds(void *manager){
    unsigned track=(unsigned)(fw_active_track-4);
    if(track<4)
        fw_arp_led_set(manager,19,arp_enabled[track]?2:1,arp_enabled[track]?31:2);
}
/* FUNC+NOTE (the KEYBOARD key's secondary function) toggles MIDI ARP.
   Consume it on MIDI tracks so the stock key handler does not open a menu. */
int panel_arp_shortcut(void *brain,void *event){
    (void)brain;
    const uint32_t *words=(const uint32_t *)event;
    unsigned key=words[3],flags=words[4];
    if(key!=3 || !(flags&1) || !(flags&2) || (flags&8) ||
       (unsigned)(fw_active_track-4)>=4)return 0;
    unsigned t=(unsigned)(fw_active_track-4);
    if(!proxies[t].bytes[0x10f])proxies[t].bytes[0x10f]=1;
    arp_enabled[t]=!arp_enabled[t];
    settings_dirty=1;ui_needs_redraw=1;fast_wake();
    return 1;
}
void panel_labels(void *view)
{
    if((unsigned)(fw_active_track-4)>=4)return;
    const char **shorts=(const char **)((char *)view+0x234);
    const char **longs=(const char **)((char *)view+0x254);
    shorts[5]="UP2"; longs[5]="UP TWICE";
    shorts[6]="DN2"; longs[6]="DOWN TWICE";
    shorts[7]="U/D2"; longs[7]="UP/DOWN REPEAT";
}

/* Only parameter bytes are serialized. Runtime pointers and held notes never
   enter the project record. */
arp_saved arp_settings;
void arp_capture(void)
{
    for(unsigned t=0;t<4;t++)
        for(unsigned j=0;j<ARP_RECORD_BYTES;j++)
            arp_settings.track[t][j]=proxies[t].bytes[0x100+j];
    for(unsigned t=0;t<4;t++)arp_settings.enabled[t]=arp_enabled[t];
    arp_settings.version=3;
}
void arp_restore(int found)
{
    project_changed=1;
    captured_valid=0;
    settings_dirty=0;
    for(unsigned t=0;t<4;t++){
        for(unsigned j=0;j<ARP_RECORD_BYTES;j++)
            proxies[t].bytes[0x100+j]=(found && (arp_settings.version==1 || arp_settings.version==2 || arp_settings.version==3))?arp_settings.track[t][j]:0;
        /* v1 records had no step engine and new projects have a blank proxy.
           Begin with every graphical step active, as the old MIDI engine did. */
        if(!found || arp_settings.version<2 || arp_settings.version>3){
            proxies[t].bytes[0x114]=0xff;
            proxies[t].bytes[0x115]=0xff;
        }
        if(proxies[t].bytes[0x10f]>7)proxies[t].bytes[0x10f]=0;
        arp_enabled[t]=(found && arp_settings.version==3)?!!arp_settings.enabled[t]:(proxies[t].bytes[0x10f]!=0);
    }
    arp_capture();
    fast_wake();
}
struct project_record { uint32_t tag,size; void *data; void (*loaded)(int); };
const struct project_record digiarp_saved={0x44415250,sizeof(arp_settings),&arp_settings,arp_restore};

int live_trig(const uint8_t *payload,int channel)
{
    unsigned t=trig_word(payload+4),count=trig_word(payload+16);
    if(fw_transport!=1 || t>=4 || !arp_enabled[t] || channel<0 || channel>15 ||
       !count || count>8)return 0;
    for(unsigned n=0;n<128;n++)seq_notes[t][n]=0;
    for(unsigned i=0;i<count;i++){
        unsigned note=payload[20+i];
        if(note<128){seq_notes[t][note]=1;seq_velocity[t][note]=(uint8_t)trig_word(payload+32);}
    }
    seq_window[t]=dn144_trig_window(payload);
    channels[t]=(uint8_t)channel;
    pending[t]=1;live_keys=1;
    fast_wake();
    return 1;
}
