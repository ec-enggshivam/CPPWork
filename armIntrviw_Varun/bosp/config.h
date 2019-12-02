/* config.h - ARM Integrator configuration header */

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
02h,11mar06,jb   Fix for SPRs 118173 and 118369 - T2_BOOTROM_COMPATIBILITY
02g,07mar06,jmt  Define SYS_MODEL to define sysModel string
02f,25jan06,jb   Adding VFP Support for 1136
02e,13dec05,jb   920t is re-enabled. Change entry point to allow for kernel
                 hardening
02d,24aug05,h_k  cleaned up VXLAYER.
02c,15aug05,h_k  undef INCLUDE_MMU_BASIC and define INCLUDE_MMU_GLOBAL_MAP for
                 lower layers.
02b,08aug05,h_k  fixed buffer overflow at sioPollInput in USER_APPL_INIT.
                 added or excluded some components in various layers.
02a,14jul05,h_k  changed MK_ALLOC to MEM_ALLOT. enabled MMU and cache in MKL by
                 default.
01z,08jul05,h_k  removed INCLUDE_SERIAL. added VXLAYER.
01y,16may05,scm  spr 108763 -Base 6 ARM offsets moved out of page 0
01x,13apr05,jb   Bumping BSP Rev
01w,11apr05,jb   Remove Test configuration
01v,29mar05,jb   Adding 1136JF support
01u,09mar05,rec  SPR-106845 add T2 bootrom backward compatibility
01u,14jun05,pcm  removed INCLUDE_DOSFS
01t,07mar05,rec  fix LOCAL_MEM_SIZE typo for 926
01s,28feb05,rec  change RAM_LOW_ADRS
01r,07dec04,rec  update for base6
01q,12aug04,scm  add 926E support...
01p,29may03,rec  Power Management, increase ISR_STACK_SIZE
01q,07feb03,jb   Removing unwanted define
01p,04feb03,jb   Adding ARM10 support
01o,15jul02,m_h  WindML support, C++ protection
01n,22may02,m_h  Reduce ROM_SIZE for boards with 32 meg RAM (77901)
01m,15may02,m_h  INCLUDE_SHELL, etc are for BSP validation (75760, 75904)
01l,09oct01,jpd  corrected RAM_HIGH_ADRS and LOCAL_MEM_SIZE for integrator946.
                 bump revision number to /5
01k,03oct01,jpd  added support for Integrator 946es/946es_t.
01j,02may01,rec  bump revision number, fix 559 initialization problem
01m,01nov01,t_m  merge in 946 updates
01l,22oct01,jb  Setting MMU_BASIC as default for builds of cpus with MMU
01k,15oct01,jb  New assembly macros are in h/arch/arm/arm.h
01l,09oct01,jpd  corrected RAM_HIGH_ADRS and LOCAL_MEM_SIZE for integrator946.
01k,03oct01,jpd  added support for Integrator 946es/946es_t.
01j,02may01,rec  bump revision number, fix 559 initialization problem
01i,27apr01,rec  add support for 966
01h,25jan01,jmb  remove INCLUDE_MIILIB
01g,15dec00,rec  change RAM_HIGH_ADRS
01f,21nov00,jpd  added support for Intel Ethernet driver.
01e,17feb00,jpd  added define of INCLUDE_FLASH_SIB_FOOTER; raised RAM_HIGH_ADRS.
01d,07feb00,jpd  added support for ARM720T and ARM920T.
01c,13jan00,pr	 add support for Integrator 740T/740T_T.
01b,07dec99,pr	 add DEC and PCI support.
01a,05nov99,ajb  copied from PID BSP version 01p.
*/

/*
This module contains the configuration parameters for the ARM Integrator BSP.
*/

#ifndef	INCconfigh
#define	INCconfigh

#ifdef __cplusplus
extern "C" {
#endif

/* BSP version/revision identification, before configAll.h */

#define BSP_VER_1_1     1       /* 1.2 is backwards compatible with 1.1 */
#define BSP_VER_1_2     1
#define BSP_VERSION	"2.0"
#define BSP_REV		"/11"	/* 0 for first revision */

#include <configAll.h>

/*
 * STANDALONE_NET must be defined for network debug with
 * standalone vxWorks
 */

#define STANDALONE_NET

/* SYS_MODEL define 
*
* NOTE
* This define does not include all of the possible variants, and the
* inclusion of a variant in here does not mean that it is supported.
*
*/
#if	defined(CPU_7TDMI)
    #define SYS_MODEL "ARM Integrator - ARM7TDMI (ARM)"
#elif	defined(CPU_7TDMI_T)
    #define SYS_MODEL "ARM Integrator - ARM7TDMI (Thumb)"
#elif	defined(CPU_720T)
    #define SYS_MODEL "ARM Integrator - ARM720T (ARM)"
#elif	defined(CPU_720T_T)
    #define SYS_MODEL "ARM Integrator - ARM720T (Thumb)"
#elif	defined(CPU_740T)
    #define SYS_MODEL "ARM Integrator - ARM740T (ARM)"
#elif	defined(CPU_740T_T)
    #define SYS_MODEL "ARM Integrator - ARM740T (Thumb)"
#elif	defined(CPU_920T)
    #define SYS_MODEL "ARM Integrator - ARM920T (ARM)"
#elif	defined(CPU_926E)
    #define SYS_MODEL "ARM Integrator - ARM926E (ARM)"
#elif   defined(CPU_926E_T)
    #define SYS_MODEL "ARM Integrator - ARM926E (Thumb)"
#elif	defined(CPU_920T_T)
    #define SYS_MODEL "ARM Integrator - ARM920T (Thumb)"
#elif	defined(CPU_940T)
    #define SYS_MODEL "ARM Integrator - ARM940T (ARM)"
#elif	defined(CPU_940T_T)
    #define SYS_MODEL "ARM Integrator - ARM940T (Thumb)"
#elif   defined(CPU_946ES)
    #define SYS_MODEL "ARM Integrator - ARM946ES (ARM)"
#elif   defined(CPU_946ES_T)
    #define SYS_MODEL "ARM Integrator - ARM946ES (Thumb)"
#elif   defined(CPU_966ES)
    #define SYS_MODEL "ARM Integrator - ARM966ES (ARM)"
#elif   defined(CPU_966ES_T)
    #define SYS_MODEL "ARM Integrator - ARM966ES (Thumb)"
#elif   defined(CPU_1020E)
    #define SYS_MODEL "ARM Integrator - ARM1020E (ARM)"
#elif   defined(CPU_1022E)
    #define SYS_MODEL "ARM Integrator - ARM1022E (ARM)"
#elif   defined(CPU_1136JF)
    #define SYS_MODEL "ARM Integrator - ARM1136JF (ARM)"
#else
#error CPU not supported
#endif	/* defined(CPU_7TDMI) */
 
/*
 * Support network devices.
 */

#define  INCLUDE_NETWORK

#if	defined(CPU_7TDMI)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator7t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_7TDMI_T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator7t_t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_720T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator720t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_720T_T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator720t_t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_740T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator740t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_740T_T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator740t_t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_920T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator920t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_926E)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator926ejs/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_926E_T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator926ejs_t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_920T_T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator920t_t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_940T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator940t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_940T_T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator940t_t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_966ES)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator966es/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_966ES_T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator966es_t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_946ES)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator946es/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_946ES_T)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator946es_t/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_1020E)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator10200/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_1022E)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator10220/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#elif defined(CPU_1136JF)
#  define DEFAULT_BOOT_LINE \
    "fei(0,0) host:/tor2/target/config/integrator1136jfs/vxWorks " \
    "h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=targetname"
#else
#  error CPU type not supported
#endif  /* defined(CPU_7TDMI) */

/* Memory configuration */

#undef	LOCAL_MEM_AUTOSIZE			/* run-time memory sizing */
#define USER_RESERVED_MEM	0		/* see sysMemTop() */

/*
 * Local-to-Bus memory address constants:
 * the local memory address always appears at 0 locally;
 * it is not dual ported.
 */

#if defined(CPU_946ES) || defined(CPU_946ES_T)
#define LOCAL_MEM_LOCAL_ADRS	0x00100000	/* fixed at 1Mbyte after TCM */
#define LOCAL_MEM_BUS_ADRS	0x00100000	/* fixed at 1Mbyte after TCM */
#define LOCAL_MEM_SIZE		0x00700000	/* 7 Mbytes after TCM */
#elif defined(CPU_966ES) || defined(CPU_966ES_T)
#  define LOCAL_MEM_LOCAL_ADRS	0x08000000	/* fixed at 0x08000000 */
#  define LOCAL_MEM_BUS_ADRS	0x08000000	/* fixed at 0x08000000 */
#define LOCAL_MEM_SIZE		0x00800000	/* 8 Mbytes */
#elif defined(CPU_1020E) || defined(CPU_1022E)
#define LOCAL_MEM_LOCAL_ADRS	0x00000000	/* fixed at 2Mbyte after TCM */
#define LOCAL_MEM_BUS_ADRS	0x00000000	/* fixed at 2Mbyte after TCM */
#define LOCAL_MEM_SIZE		0x01000000 + INTEGRATOR_HDR_SSRAM_SIZE	/* 16 Mbytes after TCM */
#define INTEGRATOR_EARLY_I_CACHE_ENABLE		/* Enable Early I-Cache */
#define INTEGRATOR_CONSERVE_VIRTUAL_SPACE	/* The speeds up boot significantly */
#define LOCAL_MEM_AUTOSIZE			/* Enable AUTOSIZE */
#elif defined(CPU_926E) || defined(CPU_926E_T) || defined(CPU_1136JF)
#define INTEGRATOR_EARLY_I_CACHE_ENABLE         /* Enable Early I-Cache */
#define INTEGRATOR_CONSERVE_VIRTUAL_SPACE       /* The speeds up boot significantly */
#define LOCAL_MEM_LOCAL_ADRS    0x00000000
#define LOCAL_MEM_BUS_ADRS      0x00000000
#define LOCAL_MEM_SIZE          0x08000000
#define LOCAL_MEM_AUTOSIZE                      /* Enable AUTOSIZE */
#elif defined(CPU_920T)
#  define LOCAL_MEM_LOCAL_ADRS	0x00000000	/* fixed at zero */
#  define LOCAL_MEM_BUS_ADRS	0x00000000	/* fixed at zero */
#define LOCAL_MEM_SIZE		0x02000000	/* 32 Mbytes */
#else
#  define LOCAL_MEM_LOCAL_ADRS	0x00000000	/* fixed at zero */
#  define LOCAL_MEM_BUS_ADRS	0x00000000	/* fixed at zero */
#define LOCAL_MEM_SIZE		0x00800000	/* 8 Mbytes */
#endif
#define LOCAL_MEM_END_ADRS	(LOCAL_MEM_LOCAL_ADRS + LOCAL_MEM_SIZE)

/*
 * Boot ROM is an image written into Flash. Part of the Flash can be
 * reserved for boot parameters etc. (see the Flash section below).
 *
 * The following parameters are defined here and in the Makefile.
 * They must be kept synchronized; effectively config.h depends on Makefile.
 * Any changes made here must be made in the Makefile and vice versa.
 *
 * ROM_BASE_ADRS is the base of the Flash ROM/EPROM.
 * ROM_TEXT_ADRS is the entry point of the VxWorks image
 * ROM_SIZE is the size of the part of the Flash ROM/EPROM allocated to
 *		the VxWorks image (block size - size of headers)
 *
 * Two other constants are used:
 * ROM_COPY_SIZE is the size of the part of the ROM to be copied into RAM
 * 		 (e.g. in uncompressed boot ROM)
 * ROM_SIZE_TOTAL is the size of the entire Flash ROM (used in sysPhysMemDesc)
 *
 * The values are given as literals here to make it easier to ensure
 * that they are the same as those in the Makefile.
 */

#define ROM_BASE_ADRS       0x24000000     /* base of Flash/EPROM */
#define ROM_TEXT_ADRS       ROM_BASE_ADRS  /* code start addr in ROM */
#define ROM_SIZE            0x00100000     /* size of ROM holding VxWorks*/

#define ROM_COPY_SIZE       ROM_SIZE
#define ROM_SIZE_TOTAL      0x02000000	/* total size of ROM */

#if defined(CPU_946ES) || defined(CPU_946ES_T)
#  define RAM_LOW_ADRS		0x00101000	/* VxWorks image entry point */
#  define RAM_HIGH_ADRS		0x00600000	/* RAM address for ROM boot */
#elif defined(CPU_966ES) || defined(CPU_966ES_T)
#  define RAM_LOW_ADRS		0x08001000	/* VxWorks image entry point */
#  define RAM_HIGH_ADRS		0x08600000	/* RAM address for ROM boot */
#elif defined(CPU_1020E) || defined(CPU_1022E)
#  define RAM_LOW_ADRS		0x00201000	/* VxWorks image entry point */
#  define RAM_HIGH_ADRS		0x00600000	/* RAM address for ROM boot */
#elif defined(CPU_1136JF) || defined(CPU_920T) || defined(CPU_926E)
#  define RAM_LOW_ADRS		0x00004000	/* VxWorks image entry point */
#  define RAM_HIGH_ADRS		0x00600000	/* RAM address for ROM boot */
#else
#  define RAM_LOW_ADRS		0x00002000	/* VxWorks image entry point */
#  define RAM_HIGH_ADRS		0x00600000	/* RAM address for ROM boot */
#endif

/*
 * Count for a CPU delay loop at the beginning of romInit. There have been
 * reports of problems with certain boards and certain power supplies, and
 * adding a delay at the start of romInit appears to help with this. This
 * value may need tuning for different board/PSU combinations.
 */

#define INTEGRATOR_DELAY_VALUE	0x1000

/*
 * Flash/NVRAM memory configuration
 *
 * A block of the Flash memory (FLASH_SIZE bytes at FLASH_ADRS) is
 * reserved for non-volatile storage of data.
 *
 * See also integrator.h
 */

#define INCLUDE_FLASH

#ifdef INCLUDE_FLASH
#define FLASH_SIZE		0x00020000	/* one 128kbyte block of Flash*/
#define NV_RAM_SIZE		0x100		/* how much we use as NVRAM */
#undef	NV_BOOT_OFFSET
#define NV_BOOT_OFFSET		0		/* bootline at start of NVRAM */
#define FLASH_NO_OVERLAY			/* do not read-modify-write all
						 * of Flash */
#define INCLUDE_FLASH_SIB_FOOTER		/* add a SIB footer to block */
#else	/* INCLUDE_FLASH */
#define NV_RAM_SIZE		NONE

#endif	/* INCLUDE_FLASH */

/* Serial port configuration */

#undef	NUM_TTY
#define NUM_TTY			N_SIO_CHANNELS

#define DEFAULT_BAUD      9600

/*
 * Define SERIAL_DEBUG to enable debugging
 * via the serial ports
 */
#undef SERIAL_DEBUG

#define INCLUDE_WDB

#ifdef INCLUDE_WDB
      #undef WDB_COMM_TYPE
      #undef WDB_TTY_BAUD
      #undef WDB_TTY_CHANNEL
      #undef WDB_TTY_DEV_NAME

     #ifdef SERIAL_DEBUG
      #define WDB_NO_BAUD_AUTO_CONFIG
      #define WDB_COMM_TYPE       WDB_COMM_SERIAL /* WDB in Serial mode */
      #define WDB_TTY_BAUD        115200        /* Baud rate for WDB Connection */
      #define WDB_TTY_CHANNEL     1           /* COM PORT #2 */
      #define WDB_TTY_DEV_NAME    "/tyCo/1"   /* default TYCODRV_5_2 device name */
     #else /* SERIAL_DEBUG */
      #define WDB_COMM_TYPE       WDB_COMM_END
      #define WDB_TTY_BAUD        DEFAULT_BAUD    /* Baud rate for WDB Connection */
      #define WDB_TTY_CHANNEL     0               /* COM PORT #1 */
      #define WDB_TTY_DEV_NAME    "/tyCo/0"       /* default TYCODRV_5_2 device name */
     #endif /* SERIAL_DEBUG */
#endif /* INCLUDE_WDB */

/*
 * Cache/MMU configuration
 *
 * Note that when MMU is enabled, cache modes are controlled by
 * the MMU table entries in sysPhysMemDesc[], not the cache mode
 * macros defined here.
 */

#if defined(CPU_720T)  || defined(CPU_720T_T) || \
    defined(CPU_740T)  || defined(CPU_740T_T) || \
    defined(CPU_920T)  || defined(CPU_920T_T) || \
    defined(CPU_926E)  || defined(CPU_926E_T) || \
    defined(CPU_940T)  || defined(CPU_940T_T) || \
    defined(CPU_946ES) || defined(CPU_946ES_T) || \
    defined(CPU_1020E) || defined(CPU_1022E)  || \
    defined(CPU_1136JF)

/*
 * We use the generic architecture libraries, with caches/MMUs present. A
 * call to sysHwInit0() is needed from within usrInit before
 * cacheLibInit() is called.
 */

#ifndef _ASMLANGUAGE
IMPORT void sysHwInit0 (void);
#endif
#define INCLUDE_SYS_HW_INIT_0
#define SYS_HW_INIT_0()         sysHwInit0 ()

#endif /* defined(720T/740T/920T/926E/940T/946ES/1020E/1022E/1136JF) */

#if defined(CPU_720T) || defined(CPU_720T_T) || \
    defined(CPU_740T) || defined(CPU_740T_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)
/*
 * These processors can be either write-through or copyback (defines whether
 * write-buffer is enabled); cache itself is write-through.
 */

#undef  USER_I_CACHE_MODE
#define USER_I_CACHE_MODE	CACHE_COPYBACK

/*
 * 720T and 740T have a combined Instruction/Data cache, so the modes must
 * be identical.
 */

#undef  USER_D_CACHE_MODE
#define USER_D_CACHE_MODE	(USER_I_CACHE_MODE)

#endif /* defined(720T/720T_T/740T/740T_T/CPU_1020E/CPU_1022E/CPU_1136JF) */

#if defined(CPU_920T)  || defined(CPU_920T_T) || \
    defined(CPU_926E)  || defined(CPU_926E_T) || \
    defined(CPU_940T)  || defined(CPU_940T_T) || \
    defined(CPU_946ES) || defined(CPU_946ES_T)
/*
 * 920T/926E/940T/946ES I-cache mode is a bit of an inappropriate concept,
 * but use this.
 * */

#undef  USER_I_CACHE_MODE
#define USER_I_CACHE_MODE       CACHE_WRITETHROUGH

/* 920T/926E/940T/946ES has to be this. */

#undef  USER_D_CACHE_MODE
#define USER_D_CACHE_MODE       CACHE_COPYBACK

#endif /* defined(CPU_920T/926E/940T/946ES) */

#if defined(CPU_940T) || defined(CPU_940T_T) || \
    defined(CPU_926E) || defined(CPU_926E_T)

/*
 * All ARM 940T and 926ES BSPs must define a variable sysCacheUncachedAdrs: a
 * pointer to a word that is uncached and is safe to read (i.e. has no
 * side effects).  This is used by the cacheLib code to perform a read
 * (only) to drain the write-buffer. Clearly this address must be present
 * within one of the regions created within sysPhysMemDesc, where it must
 * be marked as non-cacheable. There are many such addresses we could use
 * on the board, but we choose to use an address here that will be
 * mapped in on just about all configurations: a safe address within the
 * interrupt controller: the IRQ Enabled status register. This saves us
 * from having to define a region just for this pointer. This constant
 * defined here is used to initialise sysCacheUncachedAdrs in sysLib.c
 * and is also used by the startup code in sysALib.s and romInit.s in
 * draining the write-buffer.
 */

#define SYS_CACHE_UNCACHED_ADRS               AMBA_INT_CSR_ENB
#endif /* defined(CPU_940T/940T_T) */

#if defined(CPU_740T)  || defined(CPU_740T_T) || \
    defined(CPU_940T)  || defined(CPU_940T_T) || \
    defined(CPU_946ES) || defined(CPU_946ES_T)

/*
 * 740T/940T/946E have an MPU and not a full MMU, so only INCLUDE_MMU_MPU can be
 * supported, and not full page-table-style MMU. Include support for
 * the MPU by default (this can be overridden, if desired).
 */

#undef	INCLUDE_MMU_BASIC
#define INCLUDE_MMU_MPU

#endif /* defined(740T/940T/946ES) */


#if defined(CPU_720T) || defined(CPU_720T_T) || \
    defined(CPU_920T) || defined(CPU_920T_T) || \
    defined(CPU_926E) || defined(CPU_926E_T) || \
    defined(CPU_1020E) || defined(CPU_1022E) || \
    defined(CPU_1136JF)

/*
 * Include MMU BASIC and CACHE support for command line and project builds
 */
#define	INCLUDE_MMU_BASIC
#define INCLUDE_CACHE_SUPPORT

#endif /* defined(720T/720T_T/920T/920T_T/926E/CPU_1020E/CPU_1022E/CPU_1136JF) */

#if defined(CPU_1136JF)

/*
 * Vector Floating Point Support
 */

#define INCLUDE_VFP

#endif /* defined(CPU_1136JF) */

/*
 * Network driver configuration.
 *
 * De-select unused (default) network drivers selected in configAll.h.
 */

#undef	INCLUDE_ENP		/* include CMC Ethernet interface*/
#undef	INCLUDE_EX		/* include Excelan Ethernet interface */
#undef	INCLUDE_SM_NET		/* include backplane net interface */
#undef	INCLUDE_SM_SEQ_ADDR	/* shared memory network auto address setup */

/* Enhanced Network Driver (END) Support */

#define INCLUDE_END

#ifdef	INCLUDE_END
#define INCLUDE_DEC21X40END	/* include PCI-based DEC 21X4X END Ethernet */
#define INCLUDE_FEI82557END	/* include PCI-based Intel END Ethernet */

#ifndef SERIAL_DEBUG
#define WBD_AGENT_END
#else
#undef WBD_AGENT_END
#endif /* SERIAL_DEBUG */

#ifdef INCLUDE_DEC21X40END
#define INCLUDE_MIILIB
#endif

#endif  /* INCLUDE_END */


/* PCI configuration */

#define INCLUDE_PCI


/*
 * Interrupt mode - interrupts can be in either preemptive or non-preemptive
 * mode.  For non-preemptive mode, change INT_MODE to INT_NON_PREEMPT_MODEL
 */

#define INT_MODE	INT_PREEMPT_MODEL

/*
 * Enable BSP-configurable interrupt priorities: order of servicing and
 * masking of interrupts will be determined by ambaIntLvlPriMap[] in
 * sysLib.c.  If AMBA_INT_PRIORITY_MAP is not defined, priority of
 * interrupts will be least-significant bit first.
 */

#define	AMBA_INT_PRIORITY_MAP	/* BSP-configurable interrupt priorities */


/*
 * miscellaneous definitions
 * Note: ISR_STACK_SIZE is defined here rather than in ../all/configAll.h
 * (as is more usual) because the stack size depends on the interrupt
 * structure of the BSP.
 */

#define ISR_STACK_SIZE	0x2000	/* size of ISR stack, in bytes */


/* Optional timestamp support */

#undef	INCLUDE_TIMESTAMP	/* define to include timestamp driver */
#define INCLUDE_TIMESTAMP


/* Optional TrueFFS support */

#undef	INCLUDE_TFFS		/* define to include TrueFFS driver */

#ifdef INCLUDE_TFFS
#define INCLUDE_SHOW_ROUTINES
#endif /* INCLUDE_TFFS */

/* ------- new features independent of layer ------- */

#define INCLUDE_SIO_POLL
#define SIO_POLL_CONSOLE	0	/* 0 = PrimeCell UART1
					 * 1 = PrimeCell UART2
					 */

#include "integrator.h"

#undef INCLUDE_WINDML          /* define to include windML support */

/*
 * For vxWorks 6.xx, ARM boot offsets moved out of page 0 in order
 * to support kernel hardening (vector page protection)...
 *
 * This BSP by default enables vxWorks 5.xx bootrom compatibility which
 * does not support kernel hardening.
 *
 * If INCLUDE_KERNEL_HARDENING is defined, 6.xx bootroms will need to
 * be flashed.
 */

/* undefine the following symbol if using 6.xx bootroms */
#define T2_BOOTROM_COMPATIBILITY

#if (!defined (INCLUDE_KERNEL_HARDENING) && defined (T2_BOOTROM_COMPATIBILITY))
   #undef SM_ANCHOR_OFFSET
   #define SM_ANCHOR_OFFSET        0x600
   #undef BOOT_LINE_OFFSET
   #define BOOT_LINE_OFFSET        0x700
   #undef EXC_MSG_OFFSET
   #define EXC_MSG_OFFSET          0x800
#endif /* !defined (INCLUDE_KERNEL_HARDENING) && defined (T2_BOOTROM_COMPATIBILITY) */

#if (defined (INCLUDE_KERNEL_HARDENING) && defined (T2_BOOTROM_COMPATIBILITY))
   #warning "T2_BOOTROM_COMPATIBILITY enabled with INCLUDE_KERNEL_HARDENING check config.h!"
   #warning "Current configuration is not compatible with vxWorks 5.xx bootroms!"
#endif

#ifdef __cplusplus
}
#endif
#endif  /* INCconfigh */

#if defined(PRJ_BUILD)
#include "prjParams.h"
#endif
