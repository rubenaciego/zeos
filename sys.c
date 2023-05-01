/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <keyboard.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <p_stats.h>

#include <errno.h>

#include <dyn_mem.h>

#define LECTURA 0
#define ESCRIPTURA 1

void * get_ebp();

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; 
  if (permissions!=ESCRIPTURA) return -EACCES; 
  return 0;
}

void user_to_system(void)
{
  update_stats(&(current()->p_stats.user_ticks), &(current()->p_stats.elapsed_total_ticks));
}

void system_to_user(void)
{
  update_stats(&(current()->p_stats.system_ticks), &(current()->p_stats.elapsed_total_ticks));
}

int sys_ni_syscall()
{
	return -ENOSYS; 
}

int sys_read(char* buffer, int size)
{
  if (!access_ok(ESCRIPTURA, buffer, size)) return -EACCES;
  int l = roundbuf_copy_to(&keyboard_rbuf, buffer, size);
  return l;
}

int sys_getpid()
{
	return current()->PID;
}

int global_TID=1000;

int ret_from_fork()
{
  return 0;
}

int sys_fork(void)
{
  struct list_head *lhcurrent = NULL;
  union task_union *uchild;
  
  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;

  lhcurrent=list_first(&freequeue);
  
  list_del(lhcurrent);
  
  uchild=(union task_union*)list_head_to_task_struct(lhcurrent);
  
  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));
  
  /* new pages dir */
  allocate_DIR((struct task_struct*)uchild);
  
  /* Allocate pages for DATA+STACK */
  int new_ph_pag, pag, i;
  page_table_entry *process_PT = get_PT(&uchild->task);
  for (pag=0; pag<NUM_PAG_DATA; pag++)
  {
    new_ph_pag=alloc_frame();
    if (new_ph_pag!=-1) /* One page allocated */
    {
      set_ss_pag(process_PT, PAG_LOG_INIT_DATA+pag, new_ph_pag);
    }
    else /* No more free pages left. Deallocate everything */
    {
      /* Deallocate allocated pages. Up to pag. */
      for (i=0; i<pag; i++)
      {
        free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
        del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
      }
      /* Deallocate task_struct */
      list_add_tail(lhcurrent, &freequeue);
      
      /* Return error */
      return -EAGAIN; 
    }
  }

  /* Copy parent's SYSTEM and CODE to child. */
  page_table_entry *parent_PT = get_PT(current());
  for (pag=0; pag<NUM_PAG_KERNEL; pag++)
  {
    set_ss_pag(process_PT, pag, get_frame(parent_PT, pag));
  }
  for (pag=0; pag<NUM_PAG_CODE; pag++)
  {
    set_ss_pag(process_PT, PAG_LOG_INIT_CODE+pag, get_frame(parent_PT, PAG_LOG_INIT_CODE+pag));
  }
  /* Copy parent's DATA to child. We will use TOTAL_PAGES-1 as a temp logical page to map to */
  for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE; pag<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pag++)
  {
    /* Map one child page to parent's address space. */
    set_ss_pag(parent_PT, pag+NUM_PAG_DATA, get_frame(process_PT, pag));
    copy_data((void*)(pag<<12), (void*)((pag+NUM_PAG_DATA)<<12), PAGE_SIZE);
    del_ss_pag(parent_PT, pag+NUM_PAG_DATA);
  }
  
  //TODO: check working
  /* If fork is invoked from a thread, overwrite main() stack with calling thread stack */
  if (current()->PID != current()->TID) {
    int last_data_pag = NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA-1;
    set_ss_pag(parent_PT, last_data_pag+NUM_PAG_DATA, get_frame(process_PT, last_data_pag));
    set_ss_pag(parent_PT, last_data_pag-1+NUM_PAG_DATA, get_frame(process_PT, last_data_pag-1));
    copy_data((void*)(current()->th_stack_page<<12), (void*)((last_data_pag+NUM_PAG_DATA)<<12)-16, PAGE_SIZE);
    del_ss_pag(parent_PT, last_data_pag+NUM_PAG_DATA);
    del_ss_pag(parent_PT, last_data_pag-1+NUM_PAG_DATA);
    uchild->stack[1022] = USER_ESP - ((0x1000 - (uchild->stack[1022]&0xfff))&0xfff); 
  }
  
  /* Deny access to the child's memory space */
  set_cr3(get_DIR(current()));

  uchild->task.TID=++global_TID;
  uchild->task.PID=uchild->task.TID;
  uchild->task.state=ST_READY;

  int register_ebp;		/* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int) get_ebp();
  register_ebp=(register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp=register_ebp + sizeof(DWord);

  DWord temp_ebp=*(DWord*)register_ebp;
  /* Prepare child stack for context switch */
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=(DWord)&ret_from_fork;
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=temp_ebp;

  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  
  /* Set the thread list to be empty */
  INIT_LIST_HEAD(&(uchild->task.th_list));
  
  
  
  return uchild->task.PID;
}

#define TAM_BUFFER 512

int sys_write(int fd, char *buffer, int nbytes) {
char localbuffer [TAM_BUFFER];
int bytes_left;
int ret;

	if ((ret = check_fd(fd, ESCRIPTURA)))
		return ret;
	if (nbytes < 0)
		return -EINVAL;
	if (!access_ok(VERIFY_READ, buffer, nbytes))
		return -EFAULT;
	
	bytes_left = nbytes;
	while (bytes_left > TAM_BUFFER) {
		copy_from_user(buffer, localbuffer, TAM_BUFFER);
		ret = sys_write_console(localbuffer, TAM_BUFFER);
		bytes_left-=ret;
		buffer+=ret;
	}
	if (bytes_left > 0) {
		copy_from_user(buffer, localbuffer,bytes_left);
		ret = sys_write_console(localbuffer, bytes_left);
		bytes_left-=ret;
	}
	return (nbytes-bytes_left);
}


extern int zeos_ticks;

int sys_gettime()
{
  return zeos_ticks;
}


void thread_wrapper(void (*function)(void* arg), void* parameter );

int sys_create_thread( void (*function)(void* arg), void* parameter ) {
  struct list_head *lhcurrent = NULL;
  union task_union *uchild;
  int stack_pag;
  
  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;
  stack_pag = alloc_new_th_stack(current());
  if (stack_pag == -1) return -ENOMEM;
  
  
  lhcurrent=list_first(&freequeue);
  
  list_del(lhcurrent);
  
  uchild=(union task_union*)list_head_to_task_struct(lhcurrent);
  
  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));
  
  uchild->task.TID=++global_TID;
  
  
  /* Prepare child stack for context switch */
  DWord * user_stack = (DWord *) ((stack_pag+1)<<12);
  --user_stack;
  *user_stack = (DWord) parameter;
  --user_stack;
  *user_stack = (DWord) function;
  --user_stack;
  *user_stack = 0; //false return address, could be ommited
  
  
  uchild->stack[1022] = (DWord) user_stack;
  uchild->stack[1019] = (DWord)&thread_wrapper;

  int register_ebp;		/* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int) get_ebp();
  register_ebp=(register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp=register_ebp + sizeof(DWord);

  DWord temp_ebp=*(DWord*)register_ebp;
  /* Prepare child stack for context switch */
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=(DWord)&ret_from_fork;
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=temp_ebp;
  
  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  
  /* Set the thread list to be empty */
  list_add_tail(&(uchild->task.th_list), &(current()->th_list));
  
  return uchild->task.TID;
}


void sys_exit();

void sys_exit_thread() {
  if(current()->PID == current()->TID) sys_exit();

  page_table_entry *process_PT = get_PT(current());
  
  // Deallocate all the propietary physical page (stack)
  free_frame(get_frame(process_PT, current()->th_stack_page));
  del_ss_pag(process_PT, current()->th_stack_page);
  
  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);
  
  current()->TID=-1;
  current()->PID=-1;
  
  /* Restarts execution of the next process */
  sched_next_rr();  
}

void sys_exit()
{ 
  if(current()->PID != current()->TID) sys_exit_thread();
  
  int i;

  page_table_entry *process_PT = get_PT(current());

  struct list_head* pos, *n;
  
  /* Kill all child threads */
  list_for_each_safe(pos, n, &(current()->th_list)) {
    kill_thread(list_head_to_task_struct(pos));
  }
  
  // Deallocate all the propietary physical pages
  for (i=0; i<NUM_PAG_DATA; i++)
  {
    free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
    del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
  }
  
  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);
  
  current()->TID=-1;
  current()->PID=-1;
  
  /* Restarts execution of the next process */
  sched_next_rr();
}

/* System call to force a task switch */
int sys_yield()
{
  force_task_switch();
  return 0;
}

extern int remaining_quantum;

int sys_get_stats(int pid, struct stats *st)
{
  int i;
  
  if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT; 
  
  if (pid<0) return -EINVAL;
  for (i=0; i<NR_TASKS; i++)
  {
    if (task[i].task.PID==pid)
    {
      task[i].task.p_stats.remaining_ticks=remaining_quantum;
      copy_to_user(&(task[i].task.p_stats), st, sizeof(struct stats));
      return 0;
    }
  }
  return -ESRCH; /*ESRCH */
}
