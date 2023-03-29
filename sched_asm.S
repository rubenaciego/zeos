#include <asm.h>

ENTRY(task_switch)
    push %esi
    push %edi
    push %ebx
    push 12(%esp)
    call inner_task_switch
    pop %ebx
    pop %ebx
    pop %edi
    pop %esi
    ret
