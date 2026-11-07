exec(open('tests/base27.py').read().split("tap('MIDI');tap('T1');tap('SYN1');combo('A')")[0])
from emu.panelleds import seed
s.ev['unblock_skip'].add(int(syms['semaphore'],16))
for k in ['GLOBAL']+['DOWN']*4+['YES','DOWN','YES']+['DOWN']*3+['RIGHT','DOWN','RIGHT','NO','NO','NO']:tap(k)
assert word(0x41368d68)&1
base=int(syms['proxies'],16)+4;en=int(syms['arp_enabled'],16)
def led():
 st=seed(u.mem_read,'1.44','Digitone');return st.index(18),st.colours().get(18)
tap('MIDI');tap('T1');tap('SYN1');combo('A');combo('SYN1')
s.turn(1,-32,after_ms=240);s.turn(1,32,after_ms=220)
print('A_ON',u.mem_read(base+0x10f,1).hex(),u.mem_read(en,4).hex(),led(),flush=True)
assert u.mem_read(base+0x10f,1)[0]==5 and led()[0]==31
combo('KEYBOARD');print('A_OFF',u.mem_read(base+0x10f,1).hex(),u.mem_read(en,4).hex(),led(),flush=True)
assert u.mem_read(en,1)==b'\0' and u.mem_read(base+0x10f,1)[0]==5 and led()[0]==2
combo('KEYBOARD');print('A_REON',u.mem_read(base+0x10f,1).hex(),led(),flush=True)
assert u.mem_read(en,1)==b'\x01' and led()[0]==31
tap('T2');print('B',led(),u.mem_read(en,4).hex(),flush=True);assert led()[0]==2
tap('T1');print('A_BACK',led(),flush=True);assert led()[0]==31
tap('MIDI');tap('T4');print('SYNTH_T4',word(int(syms['fw_active_track'],16)),led(),flush=True)
tap('MIDI');tap('T1');assert led()[0]==31
u.mem_write(base+0x110,b'\x09')
for gate in (20,64,110):
 u.mem_write(base+0x112,bytes([gate]));data.clear()
 s.press(s.code('9'));s.run_ms(1050);s.release(s.code('9'));s.run_ms(180)
 ons=[(t,int(m[2:4],16)) for t,m in data if m.startswith('90') and not m.endswith('00')]
 offs=[(t,int(m[2:4],16)) for t,m in data if m.startswith('80') or (m.startswith('90') and m.endswith('00'))]
 duration=[round(b[0]-a[0],2) for a,b in zip(ons,offs)]
 print('GATE',gate,'ON',ons,'OFF',offs,'DURATION',duration,flush=True)
 assert len(ons)>=3 and len(ons)==len(offs) and all(n==60 for t,n in ons+offs)
 assert all(d>0 for d in duration)
 print('MEDIAN',gate,sorted(duration)[len(duration)//2],flush=True)
assert not s.halted
