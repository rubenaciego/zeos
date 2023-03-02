/*
 * interrupt.h - Definició de les diferents rutines de tractament d'exepcions
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>

#define IDT_ENTRIES 256

#define SYSENTER_CS_MSR  0x174
#define SYSENTER_ESP_MSR 0x175
#define SYSENTER_EIP_MSR 0x176

extern Gate idt[IDT_ENTRIES];
extern Register idtR;

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL);
void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL);

void setIdt();
void writeMSR(DWord msrReg, QWord val);

#endif  /* __INTERRUPT_H__ */
