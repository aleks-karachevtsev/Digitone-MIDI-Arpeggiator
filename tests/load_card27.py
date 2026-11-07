exec(open('tests/base27.py').read().split("tap('MIDI');tap('T1');tap('SYN1');combo('A')")[0])
s.ev["unblock_skip"].add(int(syms["semaphore"],16))
from emu.panel import write_png
base=int(syms['proxies'],16)
def show(label):
    print(label,[(bytes(u.mem_read(base+812*t+4+0x10f,2)).hex()) for t in range(4)],flush=True)
    print('ENABLED',label,bytes(u.mem_read(int(syms['arp_enabled'],16),4)).hex(),flush=True)
    print('LENGTH',label,[u.mem_read(base+812*t+4+0x113,1).hex() for t in range(4)],flush=True)
    print("STEPS",[(bytes(u.mem_read(base+812*t+4+0x114,5)).hex()) for t in range(4)],flush=True)
    frame=s.screen_at(s.now_ms())
    if frame:write_png(frame,'test-artifacts/load27-'+label+'.png')
hits=[]
def trace(m,a,*args):hits.append((round(s.now_ms(),2),hex(a)))
for n in ('core_proj_load','core_proj_import','arp_restore'):
 a=int(syms[n],16);u.hook_add(UC_HOOK_CODE,trace,begin=a,end=a)
show('initial')
tap('GLOBAL');tap('YES');tap('YES');s.run_ms(250);show('slots')
tap('DOWN');tap('YES');s.run_ms(700);show('selected')
print('HITS',hits,'HALT',s.halted,flush=True)
tap('YES');s.run_ms(900);show('confirmed')
print('HITS',hits,'HALT',s.halted,flush=True)
devcard=s.ev.get('esdhc').card
print('CARD WRITES',len(devcard.overlay),flush=True)
devcard.flush()
print('FLUSH',__import__('hashlib').sha256(Path(paths.card).read_bytes()).hexdigest()[:16],flush=True)
