#include <asm.h>

ENTRY(task_switch)
    push %esi
    push %edi
    push %ebx
    push 16(%esp)
    call inner_task_switch
    pop %ebx
    pop %ebx
    pop %edi
    pop %esi
    ret


ENTRY(inner_task_switch_asm)
    push %ebp
    movl %esp, %ebp
    mov 8(%ebp), %eax
	movl %esp, (%eax)
    movl 12(%ebp), %esp
	popl %ebp
	ret
