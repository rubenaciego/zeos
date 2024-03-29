#include <asm.h>
#include <segment.h>

ENTRY (sys_call_table)
	.long sys_ni_syscall	//0
	.long sys_exit		    //1
	.long sys_fork		    //2
	.long sys_read	        //3
	.long sys_write		    //4
	.long sys_mutex_init	//5
	.long sys_mutex_lock	//6
	.long sys_mutex_unlock	//7
	.long sys_ni_syscall	//8
	.long sys_ni_syscall	//9
	.long sys_gettime	    //10
	.long sys_ni_syscall	//11
	.long sys_ni_syscall	//12
	.long sys_yield		    //13
	.long sys_ni_syscall	//14
	.long sys_ni_syscall	//15
	.long sys_ni_syscall	//16
	.long sys_ni_syscall	//17
	.long sys_ni_syscall	//18
	.long sys_ni_syscall	//19
	.long sys_getpid	    //20	
	.long sys_ni_syscall	//21
	.long sys_ni_syscall	//22
	.long sys_ni_syscall	//23
	.long sys_ni_syscall	//24
	.long sys_ni_syscall	//25
	.long sys_ni_syscall	//26
	.long sys_ni_syscall	//27
	.long sys_ni_syscall	//28
	.long sys_ni_syscall	//29
	.long sys_ni_syscall	//30
	.long sys_ni_syscall	//31
	.long sys_ni_syscall	//32
	.long sys_ni_syscall	//33
	.long sys_ni_syscall	//34
	.long sys_get_stats	    //35
	.long sys_ni_syscall	//36
	.long sys_ni_syscall	//37
	.long sys_ni_syscall	//38
	.long sys_ni_syscall	//39
	.long sys_dyn_mem      	//40
	.long sys_ni_syscall	//41
	.long sys_ni_syscall	//42
	.long sys_ni_syscall	//43
	.long sys_ni_syscall	//44
	.long sys_ni_syscall	//45
	.long sys_ni_syscall	//46
	.long sys_ni_syscall	//47
	.long sys_ni_syscall	//48
	.long sys_ni_syscall	//49
	.long sys_ni_syscall    //50
	.long sys_ni_syscall	//51
	.long sys_ni_syscall	//52
	.long sys_ni_syscall	//53
	.long sys_ni_syscall	//54
	.long sys_exit_thread	//55
	.long sys_create_thread	//56
.globl MAX_SYSCALL
MAX_SYSCALL = (. - sys_call_table)/4
