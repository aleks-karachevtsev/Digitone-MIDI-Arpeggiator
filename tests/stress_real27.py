exec(open('tests/base27.py').read().split("tap('MIDI');tap('T1');tap('SYN1');combo('A')")[0])
s.ev["unblock_skip"].add(int(syms["semaphore"],16))
for k in ['GLOBAL']+['DOWN']*4+['YES','DOWN','YES']+['DOWN']*3+['RIGHT','DOWN','RIGHT','NO','NO','NO']:
    tap(k)
assert word(0x41368d68)&1
# Arm native panel mode on MIDI A, then close the ARP view.
tap('MIDI');tap('T1');tap('SYN1');combo('A');combo('SYN1');s.turn(1,-32,after_ms=180);s.turn(1,32,after_ms=200);tap('NO')
proxy=int(syms['proxies'],16)+4
print('MODE',bytes(u.mem_read(proxy+0x10f,1)).hex(),flush=True)
u.mem_write(proxy+0x110,b'\x11') # stock SPD 1/96
u.mem_write(int(syms['fw_tempo'],16),(36000).to_bytes(4,'big')) # 300 BPM
u.mem_write(proxy+0x112,b'\x40') # native LEN raw 64
pat=word(0x4138f214)+4*976
print('PATTERN',hex(pat),flush=True)
u.mem_write(pat,bytes(128));u.mem_write(pat+2,b'\x00\x01')
u.mem_write(pat+0x1c8,bytes((60,4,7,0,0,0,0,0)))
u.mem_write(pat+0xc1,bytes([127]))
hits=[]
def hit(m,a,*args):
    hits.append((round(s.now_ms(),2),hex(a)))
for name in ('dn144_worker_accept','live_trig'):
    a=int(syms[name],16);u.hook_add(UC_HOOK_CODE,hit,begin=a,end=a)
tap('PLAY');s.run_ms(5000);print('PLAY',word(0x4138f208),'HITS',len(hits),'DIN packets',len(data),flush=True)
tap('STOP');s.run_ms(300);print('STOP',word(0x4138f208),'DIN packets',len(data),'HALT',s.halted,flush=True)

events=[]
status=None; payload=[]; sysex=False; other={}
for ms,h in data:
    for byte in bytes.fromhex(h):
        if byte>=0xf8:
            other[byte]=other.get(byte,0)+1
            continue
        if sysex:
            if byte==0xf7:sysex=False
            continue
        if byte==0xf0:
            sysex=True;status=None;payload=[];continue
        if byte&0x80:
            if byte>=0xf0:
                other[byte]=other.get(byte,0)+1;status=None;payload=[]
            else:status=byte;payload=[]
            continue
        if status is None:continue
        payload.append(byte)
        needed=1 if status&0xf0 in (0xc0,0xd0) else 2
        if len(payload)==needed:
            if needed==2:events.append((ms,status,*payload))
            payload=[]
print('OTHER',other,flush=True)
ons=[ms for ms,status,note,vel in events if status&0xf0==0x90 and vel]
offs=[ms for ms,status,note,vel in events if status&0xf0==0x80 or (status&0xf0==0x90 and not vel)]
intervals=[ons[i+1]-ons[i] for i in range(len(ons)-1)]
print('EVENTS',len(events),'ON',len(ons),'OFF',len(offs),'HALT',s.halted,'transport',word(0x4138f208),flush=True)
print('INTERVALS',intervals[:30],flush=True)
print('SUMMARY',len(ons),len(offs),'mean',sum(intervals)/len(intervals),'max',max(intervals),flush=True)
assert not s.halted and len(ons)>500 and len(ons)==len(offs)
assert abs(sum(intervals)/len(intervals)-60000/300/24)<1
assert max(intervals)<20
