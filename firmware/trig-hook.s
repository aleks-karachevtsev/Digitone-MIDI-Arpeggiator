| Entry by JMP over the stock channel test at 0x400f47b8, OS 1.44.
| a2 = payload, a6 = selected channel, d2 = MIDI track.
        .section .run,"ax"
        .globl dn144_trig_hook
dn144_trig_hook:
        tst.l   %a6
        blt.s   .Ldone
        lea     -16(%sp),%sp
        movem.l %d0-%d1/%a0-%a1,(%sp)
        move.l  %a6,-(%sp)
        move.l  %a2,-(%sp)
        jsr     dn144_worker_accept
        addq.l  #8,%sp
        tst.l   %d0
        bne.s   .Laccepted
        movem.l (%sp),%d0-%d1/%a0-%a1
        lea     16(%sp),%sp
        jmp     0x400f47be
.Laccepted:
        movem.l (%sp),%d0-%d1/%a0-%a1
        lea     16(%sp),%sp
.Ldone:
        jmp     0x400f49c0

        .globl dn144_worker_tail
dn144_worker_tail:
        lea     -16(%sp),%sp
        movem.l %d0-%d1/%a0-%a1,(%sp)
        jsr     dn144_worker_poll
        movem.l (%sp),%d0-%d1/%a0-%a1
        lea     16(%sp),%sp
        move.b  0x419b14ec,%d0
        rts

        .globl fw_note_on,fw_note_off
fw_note_on:
        jmp     0x400ee232
fw_note_off:
        jmp     0x400ee29e
