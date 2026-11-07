#include <assert.h>
#include <stdint.h>
volatile uint32_t fw_transport=1,fw_timeline;
int dn144_control(int,int,int,int);
int dn144_worker_accept(const uint8_t *,int);
void dn144_worker_poll(void);
static int ons,offs;
void fw_note_on(int ch,int pitch,int velocity){
    assert(ch==2 && velocity==100 && pitch==60+12*(ons%2)); ++ons;
}
void fw_note_off(int ch,int pitch){
    assert(ch==2 && pitch==60+12*(offs%2)); ++offs;
}
int main(void){
    uint8_t event[40]={[19]=1,[20]=60,[29]=6,[35]=100};
    assert(!dn144_control(4,118,2,0));
    assert(dn144_control(4,119,1,0));
    assert(dn144_control(4,118,2,0));
    assert(dn144_control(4,117,50,0));
    assert(dn144_control(4,116,2,0));
    assert(dn144_control(4,115,127,0));
    assert(dn144_worker_accept(event,2));
    for(int i=1;i<=4;i++){
        fw_timeline+=900000; dn144_worker_poll();
        assert(ons==1+i/2 && offs==(i+1)/2);
    }
    assert(dn144_control(4,115,0,0));
    dn144_worker_poll(); assert(ons==3 && offs==3);
}
