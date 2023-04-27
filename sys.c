/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#include <hardware.h>

#include <limits.h>

#include <sched.h>

#include <stat_funcs.h>


#define LECTURA 0
#define ESCRIPTURA 1

#define WRITE_BUFFER_SIZE 4096

char write_buff[WRITE_BUFFER_SIZE];

int check_fd(int fd, int permissions)
{
  if (fd != 1) return -EBADF;
  if (permissions != ESCRIPTURA) return -EACCES;
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int get_PID() {
  static int PID = 0;
  while (task_pids[PID] != 0) {
    ++PID;
    if (PID == NR_PIDS) PID = 0;
  }
  return PID;
}

int ret_from_fork();

int sys_fork()
{  
  if (list_empty(&freequeue)) {
    return -EAGAIN;
  }

  struct list_head* new_task_head = list_first(&freequeue);
  struct task_struct* new_task_st = list_head_to_task_struct(new_task_head);
  //we need to delete here and not after checking that we can indeed fork as
  //copying the pcb will overwrite the queue head. copying the PCB partially
  //seems like a worse option than re-adding to the freequeue in case of failure
  list_del(new_task_head);
  copy_data((union task_union *) current(), (union task_union *) new_task_st, sizeof(union task_union));
  
  allocate_DIR(new_task_st);
  
  int data_frames[NUM_PAG_DATA];
  
  for(int i = 0; i < NUM_PAG_DATA; ++i) {
    data_frames[i] = alloc_frame();
    if (data_frames[i] == -1) {
      for (int j = 0; j < i; ++j) free_frame(j);
      list_add(new_task_head, &freequeue);
      return -ENOMEM;
    }
  }
  
  int pag; 
  page_table_entry* new_PT =  get_PT(new_task_st);
  page_table_entry* old_PT =  get_PT(current());


  for (pag=0;pag<NUM_PAG_CODE;pag++){
    new_PT[PAG_LOG_INIT_CODE+pag].entry = 0;
    new_PT[PAG_LOG_INIT_CODE+pag].bits.pbase_addr = old_PT[PAG_LOG_INIT_CODE+pag].bits.pbase_addr;
    new_PT[PAG_LOG_INIT_CODE+pag].bits.user = 1;
    new_PT[PAG_LOG_INIT_CODE+pag].bits.present = 1;
  }
  
  
  for (pag=0;pag<NUM_PAG_DATA;pag++){
    set_ss_pag(new_PT, PAG_LOG_INIT_DATA+pag, data_frames[pag]);
  }
  
  
  for (pag=0;pag<NUM_PAG_DATA;pag++){
    //temporarily mapping the page
    set_ss_pag(old_PT, PAG_LOG_INIT_TEMP+pag, data_frames[pag]);
    //copying the data
    copy_data((void*) ((USER_FIRST_PAGE + pag)*PAGE_SIZE),(void*) ((PAG_LOG_INIT_TEMP + pag)*PAGE_SIZE), PAGE_SIZE);
    //deleting the temporal mapping from the page table but not clearing TLB yet
    del_ss_pag(old_PT, PAG_LOG_INIT_TEMP+pag);
  }
  //clear TLB
  set_cr3(current()->dir_pages_baseAddr);
  
  int PID = get_PID();
  new_task_st->PID = PID;
  task_pids[PID] = new_task_st;
  clean_stats(new_task_st);
  
  // task_switch will pop ebp and ret
  // syscall_handler + call -> push 17*4 bytes onto the stack
  new_task_st->sys_stack = (void *) new_task_st + (0x1000 - 18*4);
  *(new_task_st->sys_stack) = (unsigned long) ret_from_fork;
  --new_task_st->sys_stack;
  *(new_task_st->sys_stack) = 0;
  
  list_add(new_task_head, &readyqueue);
  
  return new_task_st->PID;
}

void sys_exit()
{
  struct task_struct* t = current();
  int PID = current()->PID;
  task_pids[PID] = 0;
  page_table_entry* pt = get_PT(t);
  
  for (int pag = 0; pag < NUM_PAG_DATA; ++pag) {
    int frame = get_frame(pt, PAG_LOG_INIT_DATA + pag);
    free_frame(frame);
    del_ss_pag(pt, PAG_LOG_INIT_DATA + pag);
  }
  
  update_process_state_rr(t, &freequeue);
  sched_next_rr(); 
}

int sys_read(char* buff, int len)
{
  return 0;
}

int sys_write(int fd, char* buff, int len)
{
  int res = check_fd(fd, ESCRIPTURA);
  if (res < 0) return res;
  if (!access_ok(VERIFY_WRITE, buff, len)) return -EFAULT;
  if (len < 0) return -EFBIG;

  int wb = 0;
  while (len > 0)
  {
    int cpy_len = min(len, WRITE_BUFFER_SIZE);
    len -= cpy_len;
    res = copy_from_user(buff, write_buff, cpy_len);
    if (res == -1) return -ENOMEM;
    wb += sys_write_console(write_buff, cpy_len);
  }

  return wb;
}

int sys_gettime()
{
  if (zeos_ticks > INT_MAX) return -EOVERFLOW;
  return (int)zeos_ticks;
}

int sys_get_stats(int pid, struct stats *st) {
  if(!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) {
    return -EFAULT;
  }
  
  if (pid >= NR_PIDS || task_pids[pid] == 0) return -ESRCH;
  
  struct stats* stat_data = &(task_pids[pid]->stats);
  
  if (pid == current()->PID)
    stat_data->remaining_ticks = sched_ticks;
  else 
    stat_data->remaining_ticks = 0;
  int res = copy_to_user(stat_data, st, sizeof(struct stats));
  if (res == -1) return -EACCES;
  return 0;
}

