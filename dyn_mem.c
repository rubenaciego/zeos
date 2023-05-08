#include <sched.h>
#include <mm.h>
#include <mm_address.h>

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

int update_heap(struct task_struct* ts, void* next_brk)
{
  int curr_pages = ((int)ts->brk_ptr + PAGE_SIZE - 1) / PAGE_SIZE - PAG_LOG_INIT_HEAP;
  int next_pages = ((int)next_brk + PAGE_SIZE - 1) / PAGE_SIZE - PAG_LOG_INIT_HEAP;
  page_table_entry* PT = get_PT(ts);

  if (next_pages > curr_pages)
  {
    for (int p = curr_pages; p < next_pages; ++p)
    {
      int frame = alloc_frame();
      if (frame == -1)
      {
        for (int pp = curr_pages; pp < p; ++pp)
        {
          frame = get_frame(PT, pp + PAG_LOG_INIT_HEAP);
          free_frame(frame);
          del_ss_pag(PT, pp + PAG_LOG_INIT_HEAP);
        }

        return -1;
      }

      set_ss_pag(PT, p + PAG_LOG_INIT_HEAP, frame);
    }
  }
  else if (next_pages < curr_pages)
  {
    for (int p = next_pages; p < curr_pages; ++p)
    {
      int frame = get_frame(PT, p + PAG_LOG_INIT_HEAP);
      free_frame(frame);
      del_ss_pag(PT, p + PAG_LOG_INIT_HEAP);
    }

    if (ts->PID == current()->PID)
    {
      page_table_entry* dir = get_DIR(ts);
      set_cr3(dir);
    }
  }

  return 0;
}
