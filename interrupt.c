/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>

#include <libc.h>

#include <zeos_interrupt.h>

#define PAGE_FAULT_IDT_ENTRY 14
#define CLOCK_IDT_ENTRY 32
#define KEYBOARD_IDT_ENTRY 33
#define KEYBOARD_PORT 0x60

#define SYSCALL_IDT_ENTRY 0x80

Gate idt[IDT_ENTRIES];
Register    idtR;
QWord zeos_ticks;

void clock_handler();
void keyboard_handler();
void syscall_handler();
void custom_page_fault_handler();



char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}


void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  setInterruptHandler(KEYBOARD_IDT_ENTRY, keyboard_handler, 0);
  setInterruptHandler(PAGE_FAULT_IDT_ENTRY, custom_page_fault_handler, 0);
  setInterruptHandler(CLOCK_IDT_ENTRY, clock_handler, 0);
  setTrapHandler(SYSCALL_IDT_ENTRY, syscall_handler, 3);

  set_idt_reg(&idtR);
}

/* INTERRUPT SERVICE ROUTINES */
extern struct task_struct* idle_task;
void keyboard_routine()
{
  unsigned char kbstate = inb(KEYBOARD_PORT);
  if (kbstate & 0x80) { //Break

  } else { //Make
    unsigned char scancode = kbstate & 0x7f;
    unsigned char ch = scancode >= 98 ? 'C' : char_map[scancode];
    if (ch == '\0') ch = 'C';
    printc_xy(0, 0, ch);
  }

  struct task_struct* c = current();
  static struct task_struct* other;

  char buff[128] = {'P', 'I', 'D', ' ', '=', ' ' };
  itoa(c->PID, buff + 6);
  printk(buff);

  if (c != idle_task)
  {
    //other = c;
    task_switch(idle_task);
  }
  else
    task_switch(other);
}

void clock_routine() {
  ++zeos_ticks;
  zeos_show_clock();
}

char num_to_hex(Byte num) {
  num &= 0xf;
  char ans[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  return ans[num];
}

void custom_page_fault_routine(int dir, int eip) {
  char msg[] = "\nProcess generates a PAGE FAULT exception at EIP: 0x";
  char snum[] = "GGGGGGGG ";
  for (int i = 7; i >= 0; --i){
    snum[i] = num_to_hex(eip);
    eip >>= 4; 
  }
  sys_write_console(msg, sizeof(msg)-1);
  sys_write_console(snum, sizeof(snum)-1);
  char msg2[] = "by trying to access 0x";
  for (int i = 7; i >= 0; --i){
    snum[i] = num_to_hex(dir);
    dir >>= 4; 
  }
  sys_write_console(msg2, sizeof(msg2)-1);
  snum[8] = '\n';
  sys_write_console(snum, sizeof(snum));
  
  
  while (1) {};
  
}
