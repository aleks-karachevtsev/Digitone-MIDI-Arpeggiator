import json,os
from pathlib import Path
from emu.fwcheck import prepare
from emu.bootstrap import first_run,FirmwarePaths
from emu.session import Session
from emu.midi import MidiOut
from unicorn import UC_HOOK_CODE
from unicorn.m68k_const import UC_M68K_REG_A7,UC_M68K_REG_D0
import hashlib
root=Path('emu-env/device').resolve()
folder=root/'build'/('dn1-'+hashlib.sha256(Path('test-artifacts/DA27.syx').read_bytes()).hexdigest()[:10])
if (folder/'firmware.json').exists():
    paths=FirmwarePaths(str(folder),'DA27.syx',str(folder/'devices'))
else:
    paths,dev=prepare('test-artifacts/DA27.syx',str(root))
os.environ.update(paths.env())
snap=first_run(paths,options={'ddr':128*1024*1024})
s=Session(snap,syx=paths.syx,audio=True,ddr=128*1024*1024);u=s.m.uc
def word(a):return int.from_bytes(u.mem_read(a,4),'big')
def tap(k):s.tap(s.code(k),80,160)
def combo(k):
    s.press(s.code('FUNC'));s.run_ms(60);tap(k);s.release(s.code('FUNC'));s.run_ms(140)
data=[]
port=MidiOut(lambda b:data.append((round(s.now_ms(),2),bytes(b).hex())))
port.install(s.m);port.attach(s.m,s.ips);s.sources+=(port,)
syms=json.load(open('test-artifacts/DA27.syx.map.json'))
def trace(m,a,*_):
    sp=m.reg_read(UC_M68K_REG_A7)
    print(hex(a),hex(word(sp+4)),hex(word(sp+8)),hex(m.reg_read(UC_M68K_REG_D0)),flush=True)
for a in ():u.hook_add(UC_HOOK_CODE,trace,begin=a,end=a)
print('BOOT',snap,flush=True)
tap('MIDI');tap('T1');tap('SYN1');combo('A')
for slot in range(4):
    tap('T'+str(slot+1));combo('SYN1');s.turn(1,-32,after_ms=240);s.turn(1,32,after_ms=240);tap('NO')
    for key in ('9','11','13'):s.press(s.code(key));s.run_ms(50)
    s.run_ms(800)
    for key in ('11','9','13'):s.release(s.code(key));s.run_ms(50)
    print('track',slot,'halt',s.halted,'transport',word(0x4138f208),'DIN',data[-50:],flush=True)
    if s.halted:break
