#ifndef DIGIARP_H
#define DIGIARP_H
#include <stdint.h>
#define DA_TRACKS 4
#define DA_KEYS 16
#define DA_MAX_SEQUENCE 128

typedef enum { DA_UP, DA_DOWN, DA_UPDOWN, DA_UPDOWN_REPEAT, DA_RANDOM, DA_AS_PLAYED, DA_UP_TWICE, DA_DOWN_TWICE } da_mode;
typedef void (*da_emit)(void *user, int track, int channel, int note, int velocity, int on);
typedef struct {
  uint8_t notes[DA_KEYS], velocity[DA_KEYS], held[DA_KEYS];
  uint8_t count, step, octaves, gate_percent, enabled, latch, playing, active_note, active_channel, active_velocity, start_armed;
  uint8_t pattern_step, pattern_length;
  int8_t offsets[16];
  uint16_t step_mask;
  uint16_t division_ticks;
  uint32_t next_start, next_off, current_start, rng;
  da_mode mode;
} da_track;
typedef struct { da_track track[DA_TRACKS]; da_emit emit; void *user; } da_engine;
void da_init(da_engine *e, da_emit emit, void *user);
void da_config(da_engine *e,int track,da_mode mode,int octaves,int division_ticks,int gate_percent,int latch);
void da_update_timing(da_engine *e,int track,int octaves,int division_ticks,int gate_percent);
void da_steps(da_engine *e,int track,uint16_t mask,const int8_t offsets[16]);
void da_pattern_length(da_engine *e,int track,int length);
void da_note_on(da_engine *e,int track,int channel,int note,int velocity);
void da_note_off(da_engine *e,int track,int channel,int note);
void da_tick(da_engine *e,uint32_t tick, const uint8_t channels[DA_TRACKS]); /* 480 PPQN */
void da_panic(da_engine *e);
void da_clear_track(da_engine *e,int track);
#endif
