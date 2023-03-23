#include <asm.h>

ENTRY(task_switch)
    push %esi
    push %edi
    push %ebx
    call inner_task_switch
    pop %ebx
    pop %edi
    pop %esi
    ret
