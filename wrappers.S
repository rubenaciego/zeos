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

ENTRY(read)
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx

    movl 8(%ebp), %ebx
    movl 12(%ebp), %ecx
    movl $3, %eax

    pushl $read_ret
    pushl %ebp
    movl %esp, %ebp
    sysenter
read_ret:
    popl %ebp
    addl $4, %esp

    cmpl $0, %eax
    jge read_no_err
    negl %eax
    movl %eax, errno
    movl $-1, %eax
read_no_err:
    popl %ebx
    popl %ebp
    ret

ENTRY(gettime)
    pushl %ebp
    movl %esp, %ebp
    
    movl $10, %eax
    pushl $gettime_ret
    pushl %ebp
    movl %esp, %ebp
    sysenter
gettime_ret:
    popl %ebp
    addl $4, %esp
    
    cmpl $0, %eax
    jge gettime_no_err
    negl %eax
    movl %eax, errno
    movl $-1, %eax
gettime_no_err:
    popl %ebp
    ret

ENTRY(getpid)
    pushl %ebp
    movl %esp, %ebp
    
    movl $20, %eax
    pushl $getpid_ret
    pushl %ebp
    movl %esp, %ebp
    sysenter
getpid_ret:
    popl %ebp
    addl $4, %esp
    
    cmpl $0, %eax
    jge getpid_no_err
    negl %eax
    movl %eax, errno
    movl $-1, %eax
getpid_no_err:
    popl %ebp
    ret

ENTRY(fork)
    pushl %ebp
    movl %esp, %ebp
    
    movl $2, %eax
    pushl $fork_ret
    pushl %ebp
    movl %esp, %ebp
    sysenter
fork_ret:
    popl %ebp
    addl $4, %esp
    
    cmpl $0, %eax
    jge fork_no_err
    negl %eax
    movl %eax, errno
    movl $-1, %eax
fork_no_err:
    popl %ebp
    ret

ENTRY(exit)
    pushl %ebp
    movl %esp, %ebp
    
    movl $1, %eax
    pushl $exit_ret
    pushl %ebp
    movl %esp, %ebp
    sysenter
exit_ret:
    popl %ebp
    addl $4, %esp
    
    cmpl $0, %eax
    jge exit_no_err
    negl %eax
    movl %eax, errno
    movl $-1, %eax
exit_no_err:
    popl %ebp
    ret
    
    
ENTRY(get_stats)
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    movl 8(%ebp), %ebx
    movl 12(%ebp), %ecx
    
    movl $35, %eax
    pushl $get_stats_ret
    pushl %ebp
    movl %esp, %ebp
    sysenter
get_stats_ret:
    popl %ebp
    addl $4, %esp
    
    cmpl $0, %eax
    jge get_stats_no_err
    negl %eax
    movl %eax, errno
    movl $-1, %eax
get_stats_no_err:
    popl %ebx
    popl %ebp
    ret
