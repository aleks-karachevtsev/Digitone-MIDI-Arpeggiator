exec(open('tests/base27.py').read().split("tap('MIDI');tap('T1');tap('SYN1');combo('A')")[0])
s.ev['unblock_skip'].add(int(syms['semaphore'],16))
for k in ['GLOBAL']+['DOWN']*4+['YES','DOWN','YES']+['DOWN']*3+['RIGHT','DOWN','RIGHT','NO','NO','NO']:tap(k)
assert word(0x41368d68)&1
base=int(syms['proxies'],16)+4; en=int(syms['arp_enabled'],16)
tap('MIDI');tap('T1');tap('SYN1');combo('A');combo('SYN1')
u.mem_write(base+0x110,b'\x11');u.mem_write(base+0x111,b'\x00')
u.mem_write(base+0x114,b'\xff\xff'+bytes(range(16)))
u.mem_write(base+0x10f,b'\x02');u.mem_write(en,b'\x01')
def pitches():return [int(h[2:4],16) for _,h in data if h.startswith('90') and not h.endswith('00')]
def offs():return [int(h[2:4],16) for _,h in data if h.startswith('80')]
for length in [1,2,3,4,7,8,15,16]:
 u.mem_write(base+0x113,bytes([length-1]));data.clear();s.press(s.code('9'));s.run_ms(1100);s.release(s.code('9'));s.run_ms(180)
 on=pitches();off=offs();print('LENGTH',length,'ON',on,'OFF',off,'HALT',s.halted,flush=True)
 assert len(on)>=length+1 and all(n==60+(i%length) for i,n in enumerate(on)) and on==off and not s.halted
u.mem_write(base+0x113,b'\x03');u.mem_write(base+0x114,b'\xff\xff'+bytes(16))
data.clear()
for k in ('9','11','13'):s.press(s.code(k));s.run_ms(140)
s.run_ms(550);cut=len(data);s.release(s.code('11'));s.run_ms(800)
remain=[int(h[2:4],16) for _,h in data[cut:] if h.startswith('90')]
print('RELEASE E remain',remain,'HALT',s.halted,flush=True)
assert len(remain)>3 and set(remain)<={60,67} and not s.halted
for k in ('9','13'):s.release(s.code(k));s.run_ms(100)
for mode in range(8):
 u.mem_write(base+0x10f,bytes([mode]));s.run_ms(80)
 for i in range(3):
  before=u.mem_read(en,1)[0];combo('KEYBOARD');after=u.mem_read(en,1)[0]
  print('TOGGLE',mode,i,before,after,'MODE',u.mem_read(base+0x10f,1)[0],flush=True)
  assert after==1-before and not s.halted
print('PASS emulator pattern lengths, chord release, shortcut all modes',flush=True)
