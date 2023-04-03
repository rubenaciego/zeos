/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <interrupt.h>

#include <libc.h>
#include <stat_funcs.h>

#define DEFAULT_QUANTUM 1000

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));
  
struct task_struct* task_pids[NR_PIDS] = { 0 };


struct list_head freequeue;
struct list_head readyqueue;
extern struct list_head blocked;

struct task_struct* idle_task;

int sched_ticks;

void inner_task_switch_asm(DWord** stack_save, DWord* new_esp);
void zeos_print_pid(int pid);

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
	task_pids[0] = idle_task_st;
	//Idle should try to give up the processor at every tick:
	idle_task_st->quantum = 1;
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
	task_pids[1] = init_task_st;
	init_task_st->quantum = DEFAULT_QUANTUM;
	allocate_DIR(init_task_st);
	set_user_pages(init_task_st);

	unsigned long* stack = ((union task_union *) init_task_st)->stack;

	tss.esp0 = (DWord) &stack[KERNEL_STACK_SIZE];
	writeMSR(SYSENTER_ESP_MSR, (DWord) &stack[KERNEL_STACK_SIZE]);

	set_cr3(init_task_st->dir_pages_baseAddr);
	sched_ticks = get_quantum(init_task_st);
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
	tss.esp0 = (DWord)&(new->stack[KERNEL_STACK_SIZE]);
	writeMSR(SYSENTER_ESP_MSR, (DWord) &(new->stack[KERNEL_STACK_SIZE]));
	set_cr3(get_DIR(&(new->task)));
	stats_system_to_x();
	stats_ready_to_system((struct task_struct *) new);
	inner_task_switch_asm(&(current()->sys_stack), new->task.sys_stack);
}

int get_quantum(struct task_struct* t)
{
	return t->quantum;
}

void set_quantum(struct task_struct* t, int new_quantum)
{
	t->quantum = new_quantum;
}

void schedule()
{
	// PROBLEMA: si es crida schedule abans de saltar a mode
	// usuari per primer cop peta (esp no és el d'una task)
	update_sched_data_rr();

	if (needs_sched_rr())
	{
		if (current() != idle_task)
			update_process_state_rr(current(), &readyqueue);
		
		sched_next_rr();
	}
}

void sched_next_rr()
{
	struct task_struct* t;
	if (list_empty(&readyqueue)) t = idle_task;
	else
	{
		t = list_head_to_task_struct(list_first(&readyqueue));
		update_process_state_rr(t, NULL);
	}

	zeos_print_pid(t->PID);
	sched_ticks = get_quantum(t);

	if (t != current())	task_switch((union task_union*)t);
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest)
{
	if (t != current()) list_del(&t->list);
	if (dest != NULL) list_add_tail(&t->list, dest);
}

int needs_sched_rr()
{
	return sched_ticks == 0;
}

void update_sched_data_rr()
{
	--sched_ticks;
}

void zeos_print_pid(int pid)
{
	char buff[32];
	itoa(pid, buff);
	printk_xy(50, 0, "PID = ");
	printk_xy(56, 0, buff);
}
