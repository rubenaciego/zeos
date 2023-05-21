#include <libc.h>

#define MALLOC_ADDRESS_MULTIPLE 8
#define MALLOC_SIZE_MASK (~(MALLOC_ADDRESS_MULTIPLE-1))
#define MALLOC_STATUS_MASK (MALLOC_ADDRESS_MULTIPLE-1)

typedef enum
{
  FREE, USED
} malloc_status;

typedef union
{
  unsigned long size;
  malloc_status status;
} malloc_header;

void* malloc_initial_address = 0;
void* malloc_last_address;

void set_status(malloc_header* h, malloc_status s)
{
  h->status = (h->status & MALLOC_SIZE_MASK) | s;
}

void set_size(malloc_header* h, unsigned long s)
{
  h->size = (h->size & MALLOC_STATUS_MASK) | (s & MALLOC_SIZE_MASK);
}

unsigned long get_size(malloc_header* h)
{
  return h->size & MALLOC_SIZE_MASK;
}

malloc_status get_status(malloc_header* h)
{
  return h->status & MALLOC_STATUS_MASK;
}

void new_chunk(void* address, int size)
{
  malloc_header* h = (malloc_header*)address;
  set_size(h, size);
  set_status(h, FREE);
  malloc_header* h_end = address + size + sizeof(malloc_header);
  set_size(h_end, size);
}

void malloc_init(int bytes)
{
  malloc_initial_address = dyn_mem(bytes + 2*sizeof(malloc_header));
  malloc_last_address = malloc_initial_address + bytes + 2*sizeof(malloc_header);
  new_chunk(malloc_initial_address, bytes);
}

void* malloc(int bytes)
{
  if (bytes < 0) return 0;
  bytes = MALLOC_ADDRESS_MULTIPLE * ((bytes + MALLOC_ADDRESS_MULTIPLE - 1) / MALLOC_ADDRESS_MULTIPLE);

  if (malloc_initial_address == 0) malloc_init(bytes);
  malloc_header* chunk = (malloc_header*)malloc_initial_address;

  while (chunk != malloc_last_address)
  {
    int csize = get_size(chunk);
    if (get_status(chunk) == FREE && csize >= bytes)
    {
      int rem_size = csize - bytes - 2*sizeof(malloc_header);
      if (rem_size > 0)
      {
        // split
        set_size((malloc_header*)((void*)chunk + bytes + 2*sizeof(malloc_header)), rem_size);
        set_status((malloc_header*)((void*)chunk + bytes + 2*sizeof(malloc_header)), FREE);
        set_size((malloc_header*)((void*)chunk + csize + sizeof(malloc_header)), rem_size);

        set_size(chunk, bytes);
        set_size((malloc_header*)((void*)chunk + bytes + sizeof(malloc_header)), bytes);
      }

      set_status(chunk, USED);
      return (void*)chunk + sizeof(malloc_header);
    }

    chunk = (malloc_header*) ((void*)chunk + csize + 2*sizeof(malloc_header));
  }

  dyn_mem(bytes + 2*sizeof(malloc_header));
  malloc_last_address += bytes + 2*sizeof(malloc_header);
  new_chunk(chunk, bytes);
  set_status(chunk, USED);
  return (void*)chunk + sizeof(malloc_header);
}

void free(void* address)
{
  malloc_header* h = (malloc_header*)(address - sizeof(malloc_header));
  set_status(h, FREE);
  int hsize = get_size(h);
  
  if (h != malloc_initial_address)
  {
    int prev_size = get_size(h-1);
    malloc_header* prev = (malloc_header*)((void*)h - prev_size - 2*sizeof(malloc_header));
    
    if (get_status(prev) == FREE)
    {
      // merge chunks
      set_size(prev, prev_size + hsize + 2*sizeof(malloc_header));
      set_size((malloc_header*)(address + hsize), get_size(prev));
      h = prev;
      hsize = get_size(h);
    }
  }

  if ((void*)h + hsize + 2*sizeof(malloc_header) == malloc_last_address)
  {
    dyn_mem(-(hsize + 2*sizeof(malloc_header)));
    malloc_last_address -= hsize + 2*sizeof(malloc_header);
  }
}
