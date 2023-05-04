#include<sched.h>
#include<mm.h>
#include<mm_address.h>

/**
 * Returns the first available thread-stack page, -1 if none available
 */
int find_free_th_stack_page(page_table_entry* PT) {
  for(int i = PAG_LOG_TH_START; i < PAG_LOG_TH_START + NUM_PAG_TH_STACKS; ++i) {
    if(!PT[i].bits.present) return i;
  }
  return -1;
    
}

/**
 * Tries to allocate a new stack for a thread, returns the page number of the stack
 * if it succeeds, -1 otherwise
 */
int alloc_new_th_stack(struct task_struct* ts) {
  page_table_entry* PT = get_PT(ts);
  int th_stack_page = find_free_th_stack_page(PT);
  if (th_stack_page == -1) return -1;
  int th_stack_frame = alloc_frame();
  if (th_stack_frame == -1) return -1;
  set_ss_pag(PT, th_stack_page, th_stack_frame);
  return th_stack_page;
  
}
