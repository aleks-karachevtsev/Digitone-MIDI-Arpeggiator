exec(open('tests/base27.py').read().split("tap('MIDI');tap('T1');tap('SYN1');combo('A')")[0])
s.ev['unblock_skip'].add(int(syms['semaphore'],16))
for k in ['GLOBAL']+['DOWN']*4+['YES','DOWN','YES']+['DOWN']*3+['RIGHT','DOWN','RIGHT','NO','NO','NO']:tap(k)
assert word(0x41368d68)&1
base=int(syms['proxies'],16)+4
tap('MIDI')
for track in range(4):
 tap('T'+str(track+1));tap('SYN1');combo('A');combo('SYN1')
 s.turn(1,-32,after_ms=200);s.turn(1,32,after_ms=220)
 assert u.mem_read(base+812*track+0x10f,1)[0]!=0
 before=len(data);s.press(s.code('9'));s.run_ms(900);s.release(s.code('9'));s.run_ms(200)
 ev=data[before:];on=[h for _,h in ev if h.startswith('90')];off=[h for _,h in ev if h.startswith('80')]
 print('TRACK',track,'MODE',u.mem_read(base+812*track+0x10f,1).hex(),'ON',on,'OFF',off,'HALT',s.halted,flush=True)
 assert on and len(on)==len(off) and not s.halted
 tap('NO')
