#include <asm.h>

ENTRY(write)
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx

    movl 8(%ebp), %ebx
    movl 12(%ebp), %ecx
    movl 16(%ebp), %edx
    movl $4, %eax

    pushl $write_ret
    pushl %ebp
    movl %esp, %ebp
    sysenter
write_ret:
    popl %ebp
    addl $4, %esp

    cmpl $0, %eax
    jge write_no_err
    negl %eax
    movl %eax, errno
    movl $-1, %eax
write_no_err:
    popl %ebx
    popl %ebp
    ret
