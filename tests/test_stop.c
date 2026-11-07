#include <assert.h>
#include "playback.h"
static int ons, offs;
static void capture(void *ctx,int t,int ch,int note,int vel,int on)
{
    (void)ctx; (void)vel;
    assert(t==0 && ch==3 && note==60);
    if(on) ++ons; else ++offs;
}
int main(void)
{
    for(unsigned stop=1; stop<=6; ++stop) {
        dn144_playback p={0};
        const uint8_t channels[]={3,0,0,0};
        uint8_t event[40]={[19]=1,[20]=60,[35]=100};
        ons=offs=0;
        da_init(&p.engine,capture,0);
        assert(dn144_accept(&p,event,0,0));
        for(unsigned i=0;i<32;++i)
            dn144_advance(&p,i,i,channels,i<stop || i>12);
        assert(ons==1 && offs==1);
    }
}
