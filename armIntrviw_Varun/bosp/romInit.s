/* romInit.s - ARM Integrator ROM initialization module */

/* Copyright 1999-2001 ARM Limited */

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
01t,04jan06,jb  Fix incomplete comment bug
01s,28sep05,h_k  added a missing .data. (SPR #112990)
01r,21jul05,h_k  removed copyright_wind_river.
01q,01apr05,rec  fix typo in CPU_926E preprocessor statements
01p,29mar05,jb   Adding 1136jfs support
01o,12aug04,scm  add 926E support...
01n,04feb03,jb   Adding ARM10 Support
01m,25jan02,m_h  sdata needs "_" for bootrom_res
01l,09oct01,jpd  added clock speed setting for 946ES.
01k,03oct01,jpd  tidied slightly.
01j,28sep01,pr   added support for ARM946ES.
01i,04jun01,rec  memory clock rate changes for 740t
01h,21feb01,h_k  added support for ARM966ES and ARM966ES_T.
01g,20nov00,jpd  change speeds on 920T and add conditional early
		 enabling of I-cache on 920T.
01f,18sep00,rec  Add delay during power up
01e,23feb00,jpd  comments changes.
01d,22feb00,jpd  changed copyright string.
01c,20jan00,jpd  added support for ARM720T/ARM920T.
01b,13jan00,pr	 added support for ARM740T.
01a,30nov99,ajb  created, based on PID version 01i.
*/

/*
DESCRIPTION
This module contains the entry code for VxWorks images that start
running from ROM, such as 'bootrom' and 'vxWorks_rom'.  The entry
point, romInit(), is the first code executed on power-up.  It performs
the minimal setup needed to call the generic C routine romStart() with
parameter BOOT_COLD.

romInit() masks interrupts in the processor and the interrupt
controller and sets the initial stack pointer (to STACK_ADRS which is
defined in configAll.h).  Other hardware and device initialisation is
performed later in the sysHwInit routine in sysLib.c.

The routine sysToMonitor() jumps to a location after the beginning of
romInit, (defined by ROM_WARM_ADRS) to perform a "warm boot".  This
entry point allows a parameter to be passed to romStart().

The routines in this module don't use the "C" frame pointer %r11@ ! or
establish a stack frame.

SEE ALSO:
.I "ARM Architecture Reference Manual,"
.I "ARM 7TDMI Data Sheet,"
.
 "ARM 720T Data Sheet,"
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
.I "ARM Integrator/CM940T User Guide",
.I "ARM Integrator/CM946E User Guide",
.I "ARM Integrator/CM9x6ES Datasheet".
.I "ARM Integrator/CM10200 User Guide",
.I "ARM Integrator/CM1136JF-S User Guide",
*/

#define	_ASMLANGUAGE
#include <vxWorks.h>
#include <sysLib.h>
#include <asm.h>
#include <regs.h>
#include <config.h>
#include <arch/arm/mmuArmLib.h>

/* internals */

	.globl	FUNC(romInit)		/* start of system code */
	.globl	VAR(sdata)		/* start of data */
        .globl  _sdata
	.globl	VAR(integratorMemSize)	/* actual memory size */


/* externals */

	.extern	FUNC(romStart)	/* system initialization routine */

	.data
_sdata:
VAR_LABEL(sdata)
	.asciz	"start of data"
	.balign	4

/* variables */

VAR_LABEL(integratorMemSize)
	.long	0

	.text
	.balign 4

/*******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*

* romInit
*     (
*     int startType	/@ only used by 2nd entry point @/
*     )

* INTERNAL
* sysToMonitor examines the ROM for the first instruction and the string
* "Copy" in the third word so if this changes, sysToMonitor must be updated.
*/

_ARM_FUNCTION(romInit)
_romInit:
cold:
	MOV	r0, #BOOT_COLD	/* fall through to warm boot entry */
warm:
	B	start

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 1999-2001 ARM Limited"
	.ascii   "\nCopyright 1999-2001 Wind River Systems, Inc."
	.balign 4

start:
	/*
	 * There have been reports of problems with certain boards and
	 * certain power supplies not coming up after a power-on reset,
	 * and adding a delay at the start of romInit appears to help
	 * with this.
	 */

	TEQS	r0, #BOOT_COLD
	MOVEQ	r1, #INTEGRATOR_DELAY_VALUE
	MOVNE	r1, #1

delay_loop:
	SUBS	r1, r1, #1
	BNE	delay_loop

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
#endif /* defined(CPU_920T/920T_T/1020E/1022E/1136JF) */

	MCR	CP_MMU, 0, r1, c1, c0, 0	/* Write to MMU CR */
#if defined(CPU_1020E)
	NOP
	NOP
	NOP
#endif /* defined(CPU_1020E) */

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

#if defined(CPU_720T) || defined(CPU_720T_T) || \
    defined(CPU_740T) || defined(CPU_740T_T)
	MOV	r2, #INTEGRATOR_RESET_RAM_BASE	/* RAM base at reset */
	SWPB	r1, r1, [r2]			/* Drain write-buffer */

	/* Flush, (i.e. invalidate) all entries in the ID-cache */

	MCR	CP_MMU, 0, r1, c7, c0, 0	/* Flush (inval) all ID-cache */
#endif /* defined(CPU_720T,740T) */

#if defined(CPU_920T)  || defined(CPU_920T_T)  || \
    defined(CPU_926E) || defined(CPU_926E_T) || \
    defined(CPU_946ES) || defined(CPU_946ES_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)
	MOV	r1, #0				/* data SBZ */
	MCR	CP_MMU, 0, r1, c7, c10, 4	/* drain write-buffer */
#if defined(CPU_1020E)
        NOP
        NOP
        NOP
#endif /* defined(CPU_1020E) */

	/* Flush (invalidate) both I and D caches */

	MCR	CP_MMU, 0, r1, c7, c7, 0	/* R1 = 0 from above, data SBZ*/
#if defined(CPU_1020E)
        NOP
        NOP
        NOP
#endif /* defined(CPU_1020E) */
#if defined(CPU_920T) || defined(CPU_920T_T) || \
    defined(CPU_926E) || defined(CPU_926E_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)
#if defined(INTEGRATOR_EARLY_I_CACHE_ENABLE)
        MRC     CP_MMU, 0, r1, c1, c0, 0
	ORR	r1, r1, #MMUCR_I_ENABLE		/* conditionally enable Icache*/
        MCR     CP_MMU, 0, r1, c1, c0, 0        /* Write to MMU CR */
#endif /* defined(INTEGRATOR_EARLY_I_CACHE_ENABLE) */
#endif /* defined(CPU_920T,926E,946ES,1020E,1022E,1136JF) */

#if defined(CPU_940T) || defined(CPU_940T_T)
	LDR	r1, L$_sysCacheUncachedAdrs	/* R1 -> uncached area */
	LDR	r1, [r1]			/* drain write-buffer */

	/* Flush (invalidate) both caches */

	MOV	r1, #0				/* data SBZ */
	MCR	CP_MMU, 0, r1, c7, c5, 0	/* Flush (inval) all I-cache */
	MCR	CP_MMU, 0, r1, c7, c6, 0	/* Flush (inval) all D-cache */
#endif /* defined(940T,940T_T) */

#if defined(CPU_720T)  || defined(CPU_720T_T) || \
    defined(CPU_920T)  || defined(CPU_920T_T) || \
    defined (CPU_926E) || defined (CPU_926E_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)
        /*
	 * Set Process ID Register to zero, this effectively disables
	 * the process ID remapping feature.
	 */

	MOV	r1, #0
	MCR	CP_MMU, 0, r1, c13, c0, 0
#if defined(CPU_1020E)
        NOP
        NOP
        NOP
#endif /* defined(CPU_1020E) */

#endif /* defined(CPU_720T,920T,1020E, 1022E) */

#if defined (CPU_926E) || defined (CPU_926E_T) || \
    defined (CPU_946ES) || defined (CPU_946ES_T) || \
    defined (CPU_1020E) || defined (CPU_1020E_T) || \
    defined (CPU_1022E) || defined (CPU_1022E_T) || \
    defined (CPU_1136JF)
                                                                                                        
            /* Set Context ID Register to zero, including Address Space ID */
                                                                                                        
            MCR     CP_MMU, 0, r1, c13, c0, 1
#endif /* defined (CPU_926E, 946ES,1020E, 1022E, 1136JF) */


#endif /* defined(CPU_720T,740T,920T,940T,946ES,1020E,1022E,1136JF) */

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

	MOV	r2, #IC_BASE			/* R2->interrupt controller */
	MVN	r1, #0				/* &FFFFFFFF */
	STR	r1, [r2, #FIQ_DISABLE-IC_BASE]	/* disable all FIQ sources */
	STR	r1, [r2, #IRQ_DISABLE-IC_BASE]	/* disable all IRQ sources */

	/*
	 * Jump to the normal (higher) ROM Position. After a reset, the
	 * ROM is mapped into memory from* location zero upwards as well
	 * as in its normal position at This code could be executing in
	 * the lower position. We wish to be executing the code, still
	 * in ROM, but in its normal (higher) position before we remap
	 * the machine so that the ROM is no longer dual-mapped from zero
	 * upwards, but so that RAM appears from 0 upwards.
	 */

	LDR	pc, L$_HiPosn
HiPosn:

#if defined(CPU_966ES) || defined(CPU_966ES_T)
	/*
	 * Set 966RAM emulation, makes external SSRAM look like
	 * internal RAM.
	 */

	MOV	r2, #INTEGRATOR_HDR_BASE
	LDR	r1, =0xA05F
	STR	r1, [r2, #INTEGRATOR_HDR_LOCK_OFFSET]
	LDR	r1, =INTEGRATOR_HDR_TCRAM_EMULATE | \
                     INTEGRATOR_HDR_CLKRATIO_2 | \
                     INTEGRATOR_HDR_HCLKDIV_3 | \
                     INTEGRATOR_HDR_PLLBYPASS_ON
	STR	r1, [r2, #INTEGRATOR_HDR_INIT_OFFSET]
	MOV	r1, #0
	STR	r1, [r2, #INTEGRATOR_HDR_LOCK_OFFSET]

	/*
	 * Enable Instruction SRAM, Data SRAM and Write buffer.
	 */

	LDR	r1, =ARM966_I_SRAM_ENABLE | ARM966_WBUFF_ENABLE | \
                     ARM966_D_SRAM_ENABLE
	MCR	CP_MMU, 0, r1, c1, c0, 0
#endif

	/*
	 * We are now executing in the normal (higher, still in ROM)
	 * position in the memory map.  Remap memory to post-reset state,
	 * so that the ROM is not now dual-mapped to zero upwards, but
	 * RAM is mapped from zero, instead.
	 */

	MOV	r1, #INTEGRATOR_HDR_REMAP
	MOV	r2, #INTEGRATOR_HDR_BASE
	STR	r1, [r2, #INTEGRATOR_HDR_CTRL_OFFSET]

	/*
	 * SSRAM is now mapped from 0 upwards.
	 *
	 * Setup asynchronous clocking (eg. core and memory clocks different)
	 */

#if !defined (CPU_1136JF)
	LDR	r1, =INTEGRATOR_HDR_BASE
	LDR	r2, [r1, #INTEGRATOR_HDR_OSC_OFFSET]
	AND	r2, r2, #(3 << 23)
	CMP	r2, #(1 << 23)
	BNE	clock1

#if defined(CPU_940T) || defined(CPU_940T_T)
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
    defined(CPU_926E)  || defined(CPU_926E_T)  || \
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
	CMP	r3, r12   		/* is this a 926E */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_926ES_VAL
	BEQ	write_clock

	CMP	r3, #0xA20		/* is this a 1020E */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_1020E_VAL
	BEQ	write_clock

	LDR	r12, =0xA22
	CMP	r3, r12			/* is this a 1022E */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_1022E_VAL
	BEQ	write_clock

	LDR	r12, =0xB36
	CMP	r3, r12   		/* is this a 1136JF */
	LDREQ	r2, =INTEGRATOR_HDR_OSC_1136JF_VAL
	BEQ	write_clock

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
	
	/*
	 * Initialize external target memory.
	 * Copied (with modifications for GNU) from uHAL.
	 *
	 * Size SDRAM (see CM940T User Guide, ARM DUI0125A - s.4.3.8, p.4-16)
	 *
	 * Check to see if the SPD data has been loaded.  If the load has
	 * not completed we will loop upto 64K times before giving up.
	 */

	LDR	r1, =INTEGRATOR_HDR_SDRAM	/* Load address of HDR_SDRAM */
	MOV	r2, #0x10000			/* Load count */

sdram1:
	LDR	r3, [r1]			/* Load contents of HDR_SDRAM */

	/* Check to see if SPD data is loaded */

	TST	r3, #INTEGRATOR_HDR_SDRAM_SPD_OK
	BNE	sdram2
	SUBS	r2, r2, #1			/* Decrement the count */
	B	sdram5

sdram2:
	/* Load address of the base of SPD data */

	LDR	r1, =INTEGRATOR_HDR_SPDBASE
	MOV	r3, #0

	/* Calculate the memory size from the SPD data. */

	LDRB	r2, [r1, #31]		/* Get Module Bank Density */
	MOV	r2, r2, LSL #2		/* Multiply by 4 */
	LDRB	r3, [r1, #5]		/* Get Number of Banks */
	MULS	r2, r3, r2		/* Multiply to to get size in MBytes */
	BEQ	sdram5			/* If zero then something's gone wrong*/

	/* The maximum SDRAM DIMM supported is 256 Mbytes */

	CMP	r2, #256
	BGT	sdram5

	/*
	 * We need to convert the size in MBytes to the value the value
	 * to write to the MEMSIZE field of HDR_SDRAM.  The formula to do
	 * this is as follows -
	 *
	 * 	MEMSIZE = LOG2(SizeInMB) - 4
	 *
	 * All the sizes that are supported are powers of 2 so a simple
	 * algorithm to find LOG2 of number is to count the number of trailing
	 * zeros.
	 */

	MOV	r1, #0		/* Initialise the counter */
sdram4:
	TST	r2, #1		/* Is the bottom bit set of the size varible */
	MOVEQ	r2, r2, LSR #1	/* If not set then divide by 2 */
	ADDEQ	r1, r1, #1	/* If not set then increment the counter */
	BEQ	sdram4		/* If not set then loop */

	CMP	r2, #1		/* $w2 should now contain 1 */
	BNE	sdram5		/* If it doesn't then something has gone wrong*/

	/* Load base address of header registers */

	LDR	r2, =INTEGRATOR_HDR_BASE

	/* Load contents of HDR_SDRAM */

	LDR	r3, [r2, #INTEGRATOR_HDR_SDRAM_OFFSET]
	AND	r3, r3, #3	/* Clear the everything except CASLAT */
	SUBS	r1, r1, #4	/* Subtract 4 from the number of trailing bits*/

	BMI	sdram5 		/* If negative then something has gone wrong */
	ORR	r3, r3, r1, LSL #2	/* Merge it into contents of HDR_SDRAM*/

	LDRB	r1, [r2, #(INTEGRATOR_HDR_SPDBASE_OFFSET + 3)] /* No. of Rows */
	AND	r1, r1, #0xF		/* Only want bottom 4 bits */
	ORR	r3, r3, r1, LSL #8	/* Merge into HDR_SDRAM */

	LDRB	r1, [r2, #(INTEGRATOR_HDR_SPDBASE_OFFSET + 4)] /* Num Columns */
	AND	r1, r1, #0xF		/* Only want bottom 4 bits */
	ORR	r3, r3, r1, LSL #12	/* Merge into HDR_SDRAM */

	LDRB	r1, [r2, #(INTEGRATOR_HDR_SPDBASE_OFFSET + 5)] /* No. of Banks*/
	AND	r1, r1, #0xF		/* Only want bottom 4 bits */
	ORR	r3, r3, r1, LSL #16	/* Merge into HDR_SDRAM */

	/* Write back to HDR_SDRAM */

	STR	r3, [r2, #INTEGRATOR_HDR_SDRAM_OFFSET]

	/*
	 * Now calculate the size of memory in bytes, this is done by
	 * shifting 1 by MEMSIZE + 24.  The magic number 24 is the 4 we
	 * subtracted earlier plus 20 to get the value is bytes (2^20
	 * being 1 Mbyte).
	 */
	MOV	r1, r3, LSR #2		/* Need to extract MEMSIZE from the */
	AND	r1, r1, #0x7		/* value we wrote to HDR_SDRAM */

	MOV	r2, #1			/* Load 1 */
	ADD	r1, r1, #24		/* Add 24 to the MEMSIZE value */
	MOV	r1, r2, LSL r1		/* Shift 1 by (24 + MEMSIZE) */
	B	sdram6

sdram5:
	MOV	r1, #0			/* Could not find any good DRAM */
	
sdram6:
	/* Load base address of header registers */

	LDR	r2, =INTEGRATOR_HDR_BASE

	/* Load contents of HDR_STAT */

	LDR	r2, [r2, #INTEGRATOR_HDR_STAT_OFFSET]

	/* Clear all but bits 23:16 to get SSRAM size */

	ANDS	r2, r2, #0xFF0000

	/* If zero then this is a old header with 256K */

	MOVEQ	r2, #0x00040000
	CMP	r1, r2		/* Is there less SDRAM than the SSRAM */
	MOVMI	r1, r2		/* If so then return the size of the SSRAM */

	/* r1 now contains memory size: store this in Memory_Size variable */

	LDR	r3, L$_memSize
	STR	r1, [r3]

	MOV	r3, r1		/* Need to return size in both these registers*/
	
	/*
	 * End of DRAM initialisation.
	 *
	 * Initialize the stack pointer to just before where the
	 * uncompress code, copied from ROM to RAM, will run.
	 */

	LDR	sp, L$_STACK_ADDR
	MOV	fp, #0			/* zero frame pointer */

	/* jump to C entry point in ROM: routine - entry point + ROM base */

#if	(ARM_THUMB)
	LDR	r12, L$_rStrtInRom
	ORR	r12, r12, #1		/* force Thumb state */
	BX	r12
#else
	LDR	pc, L$_rStrtInRom
#endif	/* (ARM_THUMB) */

/******************************************************************************/

/*
 * PC-relative-addressable pointers - LDR Rn,=sym is broken
 * note "_" after "$" to stop preprocessor performing substitution
 */

	.balign	4

L$_HiPosn:
	.long	ROM_TEXT_ADRS + HiPosn - FUNC(romInit)

L$_rStrtInRom:
	.long	ROM_TEXT_ADRS + FUNC(romStart) - FUNC(romInit)

L$_STACK_ADDR:
	.long	STACK_ADRS

L$_memSize:
	.long	VAR(integratorMemSize)

#if defined(CPU_940T) || defined (CPU_940T_T)
L$_sysCacheUncachedAdrs:
	.long	SYS_CACHE_UNCACHED_ADRS
#endif /* defined(940T,940T_T) */
