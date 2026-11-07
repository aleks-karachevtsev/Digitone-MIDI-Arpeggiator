.section .run,"ax"
.globl stock_sound,midi_channel,live_down,live_up
stock_sound: jmp 0x4001dcf2
midi_channel: jmp 0x400f3814
live_down:
    move.l 20(%sp),-(%sp)
    move.l 20(%sp),-(%sp)
    move.l 20(%sp),-(%sp)
    move.l 20(%sp),-(%sp)
    jsr live_input
    lea 16(%sp),%sp
    tst.l %d0
    bne.s handled
    lea -24(%sp),%sp
    moveq #7,%d0
    jmp 0x4003b19e
live_up:
    clr.l -(%sp)
    move.l 20(%sp),-(%sp)
    move.l 20(%sp),-(%sp)
    move.l 20(%sp),-(%sp)
    jsr live_input
    lea 16(%sp),%sp
    tst.l %d0
    bne.s handled
    subq.l #4,%sp
    move.l %d3,-(%sp)
    moveq #7,%d1
    jmp 0x4003b274
handled: rts
.globl stock_set_mode, live_labels
stock_set_mode: jmp 0x400319a2
.globl fw_arp_led_set
fw_arp_led_set: jmp 0x40016e62
live_labels:
    move.l %a2,-(%sp)
    jsr panel_labels
    addq.l #4,%sp
    clr.l 0x2bc(%a2)
    clr.l 0x2c4(%a2)
    jmp 0x40051e06
