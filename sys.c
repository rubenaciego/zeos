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

int min_pid = 2;

int ret_from_fork();

int sys_fork()
{  
  if (list_empty(&freequeue)) {
    return -EAGAIN;
  }

  struct list_head* new_task_head = list_first(&freequeue);
  struct task_struct* new_task_st = list_head_to_task_struct(new_task_head);
  list_del(new_task_head);
  
  copy_data((union task_union *) current(), (union task_union *) new_task_st, sizeof(union task_union));
  
  allocate_DIR(new_task_st);
  
  int data_frames[NUM_PAG_DATA];
  
  for(int i = 0; i < NUM_PAG_DATA; ++i) {
    data_frames[i] = alloc_frame();
    if (data_frames[i] == -1) {
      for (int j = 0; j < i; ++j) free_frame(j);
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
  
  new_task_st->PID = min_pid++; //TODO patch
  
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
