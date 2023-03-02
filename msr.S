#include <asm.h>

ENTRY(writeMSR)
    movl 4(%esp), %ecx
    movl 8(%esp), %eax
    movl 12(%esp), %edx
    wrmsr
    ret
