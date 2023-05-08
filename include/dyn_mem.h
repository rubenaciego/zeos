#ifndef DYN_MEM_H__
#define DYN_MEM_H__


int alloc_new_th_stack(struct task_struct* ts);

int update_heap(struct task_struct* ts, void* next_brk);

#endif
