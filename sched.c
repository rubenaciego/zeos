/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <interrupt.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));


struct list_head freequeue;
struct list_head readyqueue;
extern struct list_head blocked;

struct task_struct* idle_task;

void inner_task_switch_asm(DWord* stack_save, DWord new_esp);

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	printk("CPU IDLE\n");
	while(1)
	{
	;
	}
}

void init_idle (void)
{
	struct list_head* idle_task_head = list_first(&freequeue);
	struct task_struct* idle_task_st = list_head_to_task_struct(idle_task_head);
	list_del(idle_task_head);

	idle_task_st->PID = 0;
	//allocate_DIR(idle_task_st);

	unsigned long* stack = ((union task_union *) idle_task_st)->stack;
	stack[KERNEL_STACK_SIZE-1] = (unsigned long) cpu_idle;
	stack[KERNEL_STACK_SIZE-2] = 0;
	idle_task_st->sys_stack = &stack[KERNEL_STACK_SIZE-2];

	idle_task = idle_task_st;
}

void init_task1(void)
{
	struct list_head* init_task_head = list_first(&freequeue);
	struct task_struct* init_task_st = list_head_to_task_struct(init_task_head);
	list_del(init_task_head);

	init_task_st->PID = 1;
	allocate_DIR(init_task_st);
	set_user_pages(init_task_st);

	unsigned long* stack = ((union task_union *) init_task_st)->stack;

	tss.esp0 = (DWord) &stack[KERNEL_STACK_SIZE];
	writeMSR(SYSENTER_ESP_MSR, (DWord) &stack[KERNEL_STACK_SIZE]);

	set_cr3(init_task_st->dir_pages_baseAddr);
}


void init_sched(void)
{
	INIT_LIST_HEAD(&freequeue);
	struct list_head* prev = &freequeue;

	for (int i = 0; i < NR_TASKS; ++i)
	{
		struct list_head* next = &task[i].task.list;
		list_add(next, prev);
		prev = next;
	}

	INIT_LIST_HEAD(&readyqueue);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );

  return (struct task_struct*)(ret_value & 0xfffff000);
}

void inner_task_switch(union task_union* new)
{
	tss.esp0 = &(new->stack[KERNEL_STACK_SIZE]);
	writeMSR(SYSENTER_ESP_MSR, (DWord) &(new->stack[KERNEL_STACK_SIZE]));
	set_cr3(get_DIR(&(new->task)));
	inner_task_switch_asm(&(current()->sys_stack), new->task.sys_stack);
}
