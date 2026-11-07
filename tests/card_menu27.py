exec(open('tests/base27.py').read().split("tap('MIDI');tap('T1');tap('SYN1');combo('A')")[0])
s.ev["unblock_skip"].add(int(syms["semaphore"],16))
import os,hashlib
card=Path(paths.card)
def sha():return hashlib.sha256(card.read_bytes()).hexdigest()[:16]
print('CARD',card,'INITIAL',sha(),flush=True)
proxy=int(syms['proxies'],16)+4
hits=[]
def trace(m,a,*args):hits.append((round(s.now_ms(),2),hex(a)))
for n in ('core_proj_save','core_proj_load','core_proj_import'):
 a=int(syms[n],16);u.hook_add(UC_HOOK_CODE,trace,begin=a,end=a)
tap('MIDI');tap('T1');tap('SYN1');combo('A');combo('SYN1');s.turn(1,-32,after_ms=180);s.turn(1,32,after_ms=200);tap('NO')
u.mem_write(proxy+0x110,b'\x09');u.mem_write(proxy+0x113,b'\x06')
u.mem_write(proxy+0x114,b"\xff\xfd\x05\x00\xf9")
s.run_ms(500)
print('BEFORE SAVE',bytes(u.mem_read(proxy+0x10f,4)).hex(),sha(),'HITS',hits,flush=True)
tap('GLOBAL');s.run_ms(400)
from emu.panel import write_png
write_png(s.screen_at(s.now_ms()),'test-artifacts/card27-menu.png')
print('SAVE SHORTCUT',sha(),'HITS',hits,'HALT',s.halted,flush=True)
tap('YES');s.run_ms(1500)
write_png(s.screen_at(s.now_ms()),'test-artifacts/card27-yes.png')
print('YES',sha(),'HITS',hits,'HALT',s.halted,flush=True)
tap('DOWN');tap('YES');s.run_ms(500)
write_png(s.screen_at(s.now_ms()),'test-artifacts/card27-saveas.png')
print('SAVE AS',sha(),'HITS',hits,flush=True)
tap('YES');s.run_ms(700)
write_png(s.screen_at(s.now_ms()),'test-artifacts/card27-slot.png')
print('SLOT',sha(),'HITS',hits,flush=True)
tap('NO');tap('DOWN');tap('YES');s.run_ms(600)
write_png(s.screen_at(s.now_ms()),'test-artifacts/card27-empty.png')
print('EMPTY',sha(),'HITS',hits,flush=True)
tap('YES');s.run_ms(1300)
write_png(s.screen_at(s.now_ms()),'test-artifacts/card27-created.png')
print('CREATED',sha(),'HITS',hits,flush=True)
devcard=s.ev.get('esdhc').card
print('CARD WRITES',len(devcard.overlay),'CARD PATH',devcard.path,flush=True)
devcard.flush()
print('FLUSHED',sha(),flush=True)
