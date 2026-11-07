.section .run,"ax"
.globl fw_task_create,fw_task_start,fw_sem_init,fw_sem_pend,fw_sem_post
fw_task_create: jmp 0x4000170c
fw_task_start: jmp 0x40001758
fw_sem_init: jmp 0x400017dc
fw_sem_pend: jmp 0x400017ea
fw_sem_post: jmp 0x400018d0
