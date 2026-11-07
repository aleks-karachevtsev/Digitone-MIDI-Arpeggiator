#include <assert.h>
#include <stdint.h>
volatile uint32_t fw_transport,fw_timeline;
static int pitches[6],ons,offs;
void fw_note_on(int ch,int p,int vel){assert(ch==9 && vel==100 && ons<6); pitches[ons++]=p;}
void fw_note_off(int ch,int p){assert(ch==9 && offs<ons && p==pitches[offs]); offs++;}
int dn144_control(int,int,int,int);
int dn144_worker_accept(const uint8_t *,int);
void dn144_worker_poll(void);
int main(void)
{
    uint8_t event[40]={[7]=2,[19]=3,[20]=67,[21]=60,[22]=64,[35]=100};
    fw_transport=1;
    assert(!dn144_worker_accept(event,9));
    assert(dn144_control(6,119,7,0));
    assert(dn144_worker_accept(event,9));
    for(int i=1;i<=30;i++){fw_timeline+=900000; dn144_worker_poll();}
    fw_transport=0;
    dn144_worker_poll();
    assert(ons==6 && offs==6);
    for(int i=0;i<6;i++)assert((pitches[i]==(int[]){60,64,67}[i/2]));
}
