/* sysALib.s - ARM Integrator system-dependent routines */

/* Copyright 1999-2003 ARM Limited */

/*
 * Copyright (c) 1999-2006 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
01r,04jan06,jb  Fix incomplete comment bug
01q,22sep05,h_k  fixed undef symbol sysIntStackSplit. (SPR #112718)
01p,20jul05,h_k  corrected _ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK conditions.
01o,11jul05,h_k  cleaned up interrupt stack protection decoupling.
01n,01apr05,rec  fix typo in CPU_926E preprocessor statements
01m,29mar05,jb   Adding 1136jf support
01l,09feb05,rec  fix boot type
01k,04nov04,rec  add power management support for 926 and 946
01j,12aug04,scm  add 926E support...
01i,29may03,rec  Power Management
01i,04feb03,jb   Adding ARM10 Support
01h,09oct01,jpd  added clock speed setting for 946ES.
01g,03oct01,jpd  tidied slightly.
01f,28sep01,pr   added support for ARM946E.
01g,04jun01,rec  memory clock rate changes for 740t
01f,21feb01,h_k  added support for ARM966ES and ARM966ES_T.
01e,23oct00,jpd  changed speeds on 920T; added conditional early
		 enabling of instruction cache on 920T.
01d,21feb00,jpd  added further initialisation code.
01c,07feb00,jpd  added support for ARM720T and ARM920T.
01b,13jan00,pr	 added support for ARM740T.
01a,15nov99,ajb  copied from pid940t version 01h.
*/

/*
DESCRIPTION
This module contains system-dependent routines written in assembly
language.  It contains the entry code, sysInit(), for VxWorks images
that start running from RAM, such as 'vxWorks'.  These images are
loaded into memory by some external program (e.g., a boot ROM) and then
started.  The routine sysInit() must come first in the text segment.
Its job is to perform the minimal setup needed to call the generic C
routine usrInit().

sysInit() masks interrupts in the processor and the interrupt
controller and sets the initial stack pointer.  Other hardware and
device initialisation is performed later in the sysHwInit routine in
sysLib.c.

NOTE
The routines in this module don't use the "C" frame pointer %r11@ ! or
establish a stack frame.

SEE ALSO:
.I "ARM Architecture Reference Manual,"
.I "ARM 7TDMI Data Sheet,"
.I "ARM 720T Data Sheet,"
.I "ARM 740T Data Sheet,"
.I "ARM 920T Technical Reference Manual",
.I "ARM 940T Technical Reference Manual",
.I "ARM 946E-S Technical Reference Manual",
.I "ARM 966E-S Technical Reference Manual",
.I "ARM 1020E Technical Reference Manual",
.I "ARM 1022E Technical Reference Manual",
.I "ARM 1136JF-S Technical Reference Manual",
.I "ARM Reference Peripherals Specification,"
.I "ARM Integrator/AP User Guide",
.I "ARM Integrator/CM7TDMI User Guide",
.I "ARM Integrator/CM720T User Guide",
.I "ARM Integrator/CM740T User Guide",
.I "ARM Integrator/CM920T User Guide",
.I "ARM Integrator/CM926EJ-S User Guide",
.I "ARM Integrator/CM940T User Guide",
.I "ARM Integrator/CM946E User Guide",
.I "ARM Integrator/CM9x6ES Datasheet".
.I "ARM Integrator/CM10200 User Guide",
.I "ARM Integrator/CM1136JF-S User Guide",
*/


#define _ASMLANGUAGE
#include <vxWorks.h>
#include <asm.h>
#include <regs.h>
#include <sysLib.h>
#include "config.h"
#include <mmuArmLib.h>

/* internals */
	.globl  FUNC(sysInit)        /* start of system code */
#ifndef	_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK
	.globl  FUNC(sysIntStackSplit)  /* routine to split interrupt stack */
#endif	/* !_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK */

#if defined(CPU_966ES) || defined(CPU_966ES_T) || \
    defined(CPU_926E)  || defined(CPU_926E_T)  || \
    defined(CPU_946ES) || defined(CPU_946ES_T) || \
    defined(CPU_1136JF)
	.globl  FUNC(archPwrDown)       /* power down callback */
#endif

/* externals */
	.extern	FUNC(usrInit)		/* system initialization routine */

#ifndef	_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK
	.extern	FUNC(vxSvcIntStackBase) /* base of SVC-mode interrupt stack */
	.extern	FUNC(vxSvcIntStackEnd)	/* end of SVC-mode interrupt stack */
	.extern	FUNC(vxIrqIntStackBase) /* base of IRQ-mode interrupt stack */
	.extern	FUNC(vxIrqIntStackEnd)	/* end of IRQ-mode interrupt stack */
#endif	/* !_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK */

	.text
	.balign 4

/*******************************************************************************
*
* sysInit - start after boot
*
* This routine is the system start-up entry point for VxWorks in RAM, the
* first code executed after booting.  It disables interrupts, sets up
* the stack, and jumps to the C routine usrInit() in usrConfig.c.
*
* The initial stack is set to grow down from the address of sysInit().  This
* stack is used only by usrInit() and is never used again.  Memory for the
* stack must be accounted for when determining the system load address.
*
* NOTE: This routine should not be called by the user.
*
* RETURNS: N/A

* sysInit ()              /@ THIS IS NOT A CALLABLE ROUTINE @/

*/

_ARM_FUNCTION(sysInit)

#if defined(CPU_720T)  || defined(CPU_720T_T) || \
    defined(CPU_740T)  || defined(CPU_740T_T) || \
    defined(CPU_920T)  || defined(CPU_920T_T) || \
    defined(CPU_926E)  || defined(CPU_926E_T) || \
    defined(CPU_940T)  || defined(CPU_940T_T) || \
    defined(CPU_946ES) || defined(CPU_946ES_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)

	/*
	 * Set processor and MMU to known state as follows (we may have not
	 * been entered from a reset). We must do this before setting the CPU
	 * mode as we must set PROG32/DATA32.
	 *
	 * MMU Control Register layout.
	 *
	 * bit
	 *  0 M 0 MMU disabled
	 *  1 A 0 Address alignment fault disabled, initially
	 *  2 C 0 Data cache disabled
	 *  3 W 0 Write Buffer disabled
	 *  4 P 1 PROG32
	 *  5 D 1 DATA32
	 *  6 L 1 Should Be One (Late abort on earlier CPUs)
	 *  7 B ? Endianness (1 => big)
	 *  8 S 0 System bit to zero } Modifies MMU protections, not really
	 *  9 R 1 ROM bit to one     } relevant until MMU switched on later.
	 * 10 F 0 Should Be Zero
	 * 11 Z 0 Should Be Zero (Branch prediction control on 810)
	 * 12 I 0 Instruction cache control
	 */

	/* Setup MMU Control Register */
	MOV	r1, #MMU_INIT_VALUE		/* Defined in mmuArmLib.h */

	MCR	CP_MMU, 0, r1, c1, c0, 0	/* Write to MMU CR */

	/*
	 * If MMU was on before this, then we'd better hope it was set
	 * up for flat translation or there will be problems. The next
	 * 2/3 instructions will be fetched "translated" (number depends
	 * on CPU).
	 *
	 * We would like to discard the contents of the Write-Buffer
	 * altogether, but there is no facility to do this. Failing that,
	 * we do not want any pending writes to happen at a later stage,
	 * so drain the Write-Buffer, i.e. force any pending writes to
	 * happen now.
	 */

#if defined(CPU_920T)  || defined(CPU_920T_T)  || \
    defined(CPU_926E) || defined(CPU_926E_T) || \
    defined(CPU_946ES) || defined(CPU_946ES_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)
	MOV	r1, #0				/* data SBZ */
	MCR	CP_MMU, 0, r1, c7, c10, 4	/* drain write-buffer */

	/* Flush (invalidate) both I and D caches */
	MCR	CP_MMU, 0, r1, c7, c7, 0	/* R1 = 0 from above, data SBZ*/

#if defined (CPU_926E) || defined (CPU_926E_T)
        LDR     r1, L$_sysCacheUncachedAdrs     /* R2 -> uncached area */
        LDR     r1, [r1]                        /* synchronise L2 AHB */
#endif /* defined (CPU_926E,926E_T) */

#if defined(CPU_920T) || defined(CPU_920T_T) || \
    defined(CPU_926E) || defined(CPU_926E_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)
#if defined(INTEGRATOR_EARLY_I_CACHE_ENABLE)
	MRC	CP_MMU, 0, r1, c1, c0, 0
  ORR     r1, r1, #MMUCR_I_ENABLE   /* conditionally enable Icache*/
	MCR	CP_MMU, 0, r1, c1, c0, 0	/* Write to MMU CR */
#endif /* defined(INTEGRATOR_EARLY_I_CACHE_ENABLE) */
#endif /* defined(CPU_920T/920T_T/926E/1020E/1022E/1136) */

#endif /* defined(CPU_920T,926E, 946ES,1020E,1022E, 1136JF) */

#if defined(CPU_720T)  || defined(CPU_720T_T) || \
    defined(CPU_920T)  || defined(CPU_920T_T) || \
    defined(CPU_926E)  || defined(CPU_926E_T) || \
    defined(CPU_1020E) || defined(CPU_1022E)  || \
    defined(CPU_1136JF)
      /*
       * Set Process ID Register to zero, this effectively disables
       * the process ID remapping feature.
       */
      MOV     r1, #0
      MCR     CP_MMU, 0, r1, c13, c0, 0
#endif /* defined(CPU_720T,920T,926E,1020E, 1022E) */

#if defined (CPU_926E) || defined (CPU_926E_T) || \
    defined (CPU_946ES) || defined (CPU_946ES_T) || \
    defined (CPU_1020E) || defined (CPU_1020E_T) || \
    defined (CPU_1136JF)
                                                                                                    
    /* Set Context ID Register to zero, including Address Space ID */
    MCR     CP_MMU, 0, r1, c13, c0, 1
#endif /* defined (CPU_926E, 946ES) */

#endif /* defined(CPU_720T,740T,920T,926E,940T,946ES,1020E,1022E) */

	/* disable interrupts in CPU and switch to SVC32 mode */
	MRS	r1, cpsr
	BIC	r1, r1, #MASK_MODE
	ORR	r1, r1, #MODE_SVC32 | I_BIT | F_BIT
	MSR	cpsr, r1

	/*
	 * CPU INTERRUPTS DISABLED
	 *
	 * disable individual interrupts in the interrupt controller
	 */
	MOV	r2, #IC_BASE	/* R2->interrupt controller */
	MVN	r1, #0				/* &FFFFFFFF */
	STR	r1, [r2, #FIQ_DISABLE-IC_BASE]	/* disable all FIQ sources */
	STR	r1, [r2, #IRQ_DISABLE-IC_BASE]	/* disable all IRQ sources */


#if !defined (CPU_1136JF)

	/* Setup asynchronous clocking (eg. core and memory clocks different) */
	LDR	r1, =INTEGRATOR_HDR_BASE
	LDR	r2, [r1, #INTEGRATOR_HDR_OSC_OFFSET]
	AND	r2, r2, #(3 << 23)
	CMP	r2, #(1 << 23)
	BNE	clock1

#if defined(CPU_920T) || defined(CPU_920T_T) || \
    defined(CPU_940T) || defined(CPU_940T_T)
	/*
	 * if HDR_OSC indicates (bits[23:24] = 01), set
	 * implementation-specific bits in the MMU control register - set
	 * asynchronous mode.
	 */
	MRC	CP_MMU, 0, r2, c1, c0, 0
	ORR	r2, r2, #MMUCR_ASYNC
	MCR	CP_MMU, 0, r2, c1, c0, 0
#endif

clock1:
	/* If bits[23:24] were 0, set asynchronous mode in HDR_CTRL */
	LDRLT	r2, [r1, #INTEGRATOR_HDR_CTRL_OFFSET]
	BICLT	r2, r2, #INTEGRATOR_HDR_CTRL_FASTBUS
	STRLT	r2, [r1, #INTEGRATOR_HDR_CTRL_OFFSET]

	/*
	 * setup default clocks
	 *
	 * Core clock must always be greater than memory clock.
	 *
	 * Read HDR_PROC register, if this is non zero then there is no
	 * coprocessor, in this case use the default settings. First,
	 * load the default settings.
	 */
	LDR	r2, =INTEGRATOR_HDR_OSC_DFLT_VAL
	LDR	r1, =INTEGRATOR_HDR_BASE
	LDR	r3, [r1, #INTEGRATOR_HDR_PROC_OFFSET]
	CMP	r3, #0
	BNE	write_clock
	
	/*
	 * HDR_PROC was zero => there is a coprocessor. Get the processor ID.
	 *
	 * If the processor type is not recognised then the default settings
	 * will be used.
	 *
	 * For safety's sake, make the following conditional upon there being a
	 * coprocessor in the CPU.
	 */

#if defined(CPU_720T)  || defined(CPU_720T_T)  || \
    defined(CPU_740T)  || defined(CPU_740T_T)  || \
    defined(CPU_920T)  || defined(CPU_920T_T)  || \
    defined(CPU_926E)  || defined(CPU_926E_T) ||  \
    defined(CPU_940T)  || defined(CPU_940T_T)  || \
    defined(CPU_946ES) || defined(CPU_946ES_T) || \
    defined(CPU_966ES) || defined(CPU_966ES_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)
	MRC	CP_MMU, 0, r1, c0, c0, 0
	MOV	r3, r1, LSL #16
	MOV	r3, r3, LSR #20		/* move bits 15-3 to 12-0 */

	CMP	r3, #0x720		/* is this a 720 */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_720T_VAL
	BEQ	write_clock

	CMP	r3, #0x740		/* is this a 740 */
        LDREQ   r2, =INTEGRATOR_HDR_OSC_740T_VAL
	BEQ     write_clock

	CMP	r3, #0x940		/* is this a 940 */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_940T_VAL
	BEQ	write_clock

	LDR	r12, =0x946
	CMP	r3, r12			/* is this a 946ES */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_946ES_VAL
	BEQ	write_clock

	CMP	r3, #0x920		/* is this a 920 */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_920T_VAL
	BEQ	write_clock

	LDR	r12, =0x926
	CMP     r3, r12                 /* is this a 926 */
	LDREQ   r2, =INTEGRATOR_HDR_OSC_926ES_VAL
	BEQ     write_clock

	CMP	r3, #0xA20		/* is this a 1020E */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_1020E_VAL
	BEQ	write_clock

	LDR	r12, =0xA22
	CMP	r3, r12			/* is this a 1022E */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_1022E_VAL
	BEQ	write_clock

	LDR	r12, =0xB36
	CMP     r3, r12                 /* is this a 1136 */
	LDREQ   r2, =INTEGRATOR_HDR_OSC_1136JF_VAL
	BEQ     write_clock

	MOV	r1, r3, LSR #4		/* is this a 966 */
	CMP	r1, #0x96
	BNE	write_clock
	AND	r1, r3, #0xF
	CMP	r1, #6
	LDREQ	r2, =INTEGRATOR_HDR_OSC_966ES_VAL
#endif /* defined(CPU_720T/740T/920T/926E/940T/966ES/1020E/1022E/1136JF) */

write_clock:
	/* Write clock settings */
	LDR	r1, =INTEGRATOR_HDR_BASE
	LDR	r3, =0xA05F
	STR	r3, [r1, #INTEGRATOR_HDR_LOCK_OFFSET]
	STR	r2, [r1, #INTEGRATOR_HDR_OSC_OFFSET]
	MOV	r2, #0
	STR	r2, [r1, #INTEGRATOR_HDR_LOCK_OFFSET]

#else
        LDR     r3, =0xA05F
#endif /* !defined (CPU_1136JF) */

	/* Set up System BUS and PCI clocks */
	LDR	r1, =INTEGRATOR_SC_BASE
	STR	r3, [r1, #INTEGRATOR_SC_LOCK_OFFSET]
	LDR	r2, =(INTEGRATOR_SC_OSC_SYS_20MHz | INTEGRATOR_SC_OSC_PCI_33MHz)
	STR	r2, [r1, #INTEGRATOR_SC_OSC_OFFSET]
	MOV	r2, #0
	STR	r2, [r1, #INTEGRATOR_SC_LOCK_OFFSET]

	/* Initialize static memory. */
	MOV	r1, #INTEGRATOR_EBI_BASE
	
	/* CS0 - ROM (Boot Flash) */
	MOV	r2, #INTEGRATOR_EBI_8_BIT | INTEGRATOR_EBI_WS_3
	STR	r2, [r1, #INTEGRATOR_EBI_CSR0_OFFSET]

	/* CS1 - Flash (Application Flash) */
	MOV	r2, #INTEGRATOR_EBI_32_BIT | INTEGRATOR_EBI_WS_3
	STR	r2, [r1, #INTEGRATOR_EBI_CSR1_OFFSET]

	/* CS2 - SSRAM (Not on Rev A Boards) */
	MOV	r2, #INTEGRATOR_EBI_32_BIT | INTEGRATOR_EBI_WRITE_ENABLE | \
		     INTEGRATOR_EBI_SYNC | INTEGRATOR_EBI_WS_2
	STR	r2, [r1, #INTEGRATOR_EBI_CSR2_OFFSET]

	/* CS3 - Unused (Set up for debug) */
	MOV	r2, #INTEGRATOR_EBI_8_BIT | INTEGRATOR_EBI_WRITE_ENABLE
	STR	r2, [r1, #INTEGRATOR_EBI_CSR3_OFFSET]


	/* set initial stack pointer so stack grows down from start of code */
	ADR	sp, FUNC(sysInit)		/* initialize stack pointer */
	mov	fp, #0		         /* initialize frame pointer */
	

	/* Make sure Boot type is set correctly. visionClick doesn't */
	mov	r1,#BOOT_NORMAL
  cmp	r1,r0
	beq	L$_Good_Boot

	mov	r1,#BOOT_NO_AUTOBOOT
  cmp	r1,r0
	beq	L$_Good_Boot

	mov	r1,#BOOT_CLEAR
  cmp	r1,r0
	beq	L$_Good_Boot

	mov	r1,#BOOT_QUICK_AUTOBOOT
        cmp	r1,r0
	beq	L$_Good_Boot

  mov r0, #BOOT_NORMAL /* default startType */

L$_Good_Boot:
	/* now call usrInit (startType) */
#if	(ARM_THUMB)
	LDR	r12, L$_usrInit
	BX	r12
#else
	B	FUNC(usrInit)
#endif	/* (ARM_THUMB) */

#ifndef	_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK
/*******************************************************************************
*
* sysIntStackSplit - split interrupt stack and set interrupt stack pointers
*
* This routine is called, via a function pointer, during kernel
* initialisation.  It splits the allocated interrupt stack into IRQ and
* SVC-mode stacks and sets the processor's IRQ stack pointer. Note that
* the pointer passed points to the bottom of the stack allocated i.e.
* highest address+1.
*
* IRQ stack needs 6 words per nested interrupt;
* SVC-mode will need a good deal more for the C interrupt handlers.
* For now, use ratio 1:7 with any excess allocated to the SVC-mode stack
* at the lowest address.
*
* Note that FIQ is not handled by VxWorks so no stack is allocated for it.
*
* The stacks and the variables that describe them look like this.
* .CS
*
*         - HIGH MEMORY -
*     ------------------------ <--- vxIrqIntStackBase (r0 on entry)
*     |                      |
*     |       IRQ-mode       |
*     |    interrupt stack   |
*     |                      |
*     ------------------------ <--{ vxIrqIntStackEnd
*     |                      |    { vxSvcIntStackBase
*     |       SVC-mode       |
*     |    interrupt stack   |
*     |                      |
*     ------------------------ <--- vxSvcIntStackEnd
*         - LOW  MEMORY -
* .CE
*
* NOTE: This routine should not be called by the user.
*
* void sysIntStackSplit
*     (
*     char *pBotStack   /@ pointer to bottom of interrupt stack @/
*     long size		/@ size of stack @/
*     )

*/

	.balign	4

_ARM_FUNCTION_CALLED_FROM_C(sysIntStackSplit)

	/*
	 * r0 = base of space allocated for stacks (i.e. highest address)
	 * r1 = size of space
	 */
	SUB	r2, r0, r1			/* r2->lowest usable address */
	LDR	r3, L$_vxSvcIntStackEnd
	STR	r2, [r3]			/*  == end of SVC-mode stack */
	SUB	r2, r0, r1, ASR #3		/* leave 1/8 for IRQ */
	LDR	r3, L$_vxSvcIntStackBase
	STR	r2, [r3]

	/* now allocate IRQ stack, setting irq_sp */
	LDR	r3, L$_vxIrqIntStackEnd
	STR	r2, [r3]
	LDR	r3, L$_vxIrqIntStackBase
	STR	r0, [r3]

	MRS	r2, cpsr
	BIC	r3, r2, #MASK_MODE
	ORR	r3, r3, #MODE_IRQ32 | I_BIT	/* set irq_sp */
	MSR	cpsr, r3
	MOV	sp, r0

	/* switch back to original mode and return */
	MSR	cpsr, r2

#if	(ARM_THUMB)
	BX	lr
#else
	MOV	pc, lr
#endif	/* (ARM_THUMB) */
#endif	/* !_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK */
		
#if defined(CPU_926E)  || defined(CPU_926E_T) || \
    defined(CPU_966ES) || defined(CPU_966ES_T) || \
    defined(CPU_1136JF)
/*******************************************************************************
*
* archPwrDown - turn the processor into reduced power mode
*
* This routine activates the reduced power mode.
* It is called by the scheduler when the kernel enters the idle loop.
* This function is called by default. Overload it by using routine
* vxArchPowerDownRtnSet().
*
* RETURNS: void.
*
* SEE ALSO: vxArchPowerDownRtnSet().
*
* void archPwrDown (void)
*
*/
_ARM_FUNCTION_CALLED_FROM_C(archPwrDown)


/*
 * NB debugger doesn't like powering down.  Use foreverloop for debug.
 *foreverLoop:
 *       B     foreverLoop
 */

        /*
        * Write to coprocessor 15 register 7 (the core control )
        * register to set idle
        */
        MOV     r0, #PWRMODE_IDLE
        MCR     CP_CORECTL, 0, r0, c7, c0, 4  /* idle processor */

/* Return after waking up */
#if (ARM_THUMB)
	BX	lr
#else
	MOV	pc, lr
#endif

/* (REC) test code */
#else
  #ifdef POWER_MGT_INSTRUMENT
    _ARM_FUNCTION_CALLED_FROM_C(archPwrDown)
  foreverLoop:    /* test code */
        B     foreverLoop
#endif /* POWER_MGT_INSTRUMENT */

#endif /* CPU_926E || CPU_926E_T || CPU_966ES || CPU_966ES_T || CPU_1136JF */

/******************************************************************************/

/*
 * PC-relative-addressable pointers - LDR Rn,=sym is broken
 * note "_" after "$" to stop preprocessor preforming substitution
 */

	.balign	4

#ifndef	_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK
L$_vxSvcIntStackBase:
	.long	VAR(vxSvcIntStackBase)

L$_vxSvcIntStackEnd:
	.long	VAR(vxSvcIntStackEnd)

L$_vxIrqIntStackBase:
	.long	VAR(vxIrqIntStackBase)

L$_vxIrqIntStackEnd:
	.long	VAR(vxIrqIntStackEnd)
#endif	/* !_ARCH_SUPPORTS_PROTECT_INTERRUPT_STACK */

#if	(ARM_THUMB)
L$_usrInit:
	.long	FUNC(usrInit)
#endif	/* (ARM_THUMB) */

#if defined(CPU_720T) || defined(CPU_720T_T) || \
    defined(CPU_740T) || defined(CPU_740T_T)
L$_sysCacheSwapVar:
	.long   sysCacheSwapVar
#endif

#if defined(CPU_940T) || defined(CPU_940T_T) || \
    defined(CPU_926E) || defined(CPU_926E_T)
L$_sysCacheUncachedAdrs:
	.long   SYS_CACHE_UNCACHED_ADRS
#endif
