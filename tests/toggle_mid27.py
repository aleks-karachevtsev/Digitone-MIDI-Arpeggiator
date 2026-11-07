exec(open('tests/base27.py').read().split("tap('MIDI');tap('T1');tap('SYN1');combo('A')")[0])
s.ev['unblock_skip'].add(int(syms['semaphore'],16))
for k in ['GLOBAL']+['DOWN']*4+['YES','DOWN','YES']+['DOWN']*3+['RIGHT','DOWN','RIGHT','NO','NO','NO']:tap(k)
assert word(0x41368d68)&1
base=int(syms['proxies'],16)+4;en=int(syms['arp_enabled'],16)
tap('MIDI');tap('T1');tap('SYN1');combo('A');combo('SYN1');s.turn(1,-32,after_ms=180);s.turn(1,32,after_ms=180)
u.mem_write(base+0x110,b'\x09');u.mem_write(base+0x112,b'\x70')
s.press(s.code('9'));s.run_ms(200);before=len(data)
combo('KEYBOARD');cut=data[before:];quiet_at=len(data);s.run_ms(400)
ons=[(t,h) for t,h in data[quiet_at:] if h.startswith('90') and not h.endswith('00')];offs=[(t,h) for t,h in cut if h.startswith('80')]
print('OFF',bytes(u.mem_read(en,4)).hex(),'BEFORE',data[:before],'DURING',cut,'QUIET',data[quiet_at:],'HALT',s.halted,flush=True)
assert not ons and offs and u.mem_read(en,1)==b'\x00'
combo('KEYBOARD');s.run_ms(420);s.release(s.code('9'));s.run_ms(160)
print('REON',data[before:], 'MODE',u.mem_read(base+0x10f,1).hex(),'HALT',s.halted,flush=True)
assert u.mem_read(en,1)==b'\x01' and u.mem_read(base+0x10f,1)[0]==5
assert sum(h.startswith('90') and not h.endswith('00') for _,h in data)==sum(h.startswith('80') for _,h in data)
