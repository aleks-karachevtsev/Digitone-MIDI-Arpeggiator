exec(open('tests/base27.py').read().split("tap('MIDI');tap('T1');tap('SYN1');combo('A')")[0])
s.ev['unblock_skip'].add(int(syms['semaphore'],16))
pat=word(0x4138f214)+3*976
before=bytes(u.mem_read(pat,976))
tap('MIDI');tap('T4');tap('SYN1');combo('A');combo('SYN1')
s.turn(1,-32,after_ms=200);s.turn(1,32,after_ms=180);tap('RIGHT');tap('DOWN');s.turn(5,-32,after_ms=180);s.turn(5,32,after_ms=180)
after=bytes(u.mem_read(pat,976))
print('SYNTH T4 PATTERN CHANGE',[(i,x,y)for i,(x,y)in enumerate(zip(before,after))if x!=y], 'HALT',s.halted,flush=True)
assert before==after and not s.halted
