/* mmuArmLib.h - ARM MMU library header file */

/* Copyright 1996-2001 Wind River Systems, Inc. */

/*
 modification history
 --------------------
 01p,15jan02,m_h  920,940 thumb incorrectly defined as CPU_940_T not CPU_940T_T
 01o,05oct01,rec  added I-SRAM and D-SRAM CP15, R1 bit defs.
 01n,03oct01,jpd  added WRITETHROUGH definitions for 946E.
 01m,26jul01,scm  add extended small page table support for XScale...
 01l,25jul01,scm  add support for extended page attributres on XScale...
 01k,23jul01,scm  change XScale name to conform to coding standards...
 01j,11dec00,scm  replace references to ARMSA2 with XScale
 01i,01sep00,scm  add sa2 support...
 01h,07sep99,jpd  added ARM740T, ARM920T support.
 01g,20jan99,cdp  removed support for old ARM libraries;
 add prefix to MMU_IDs.
 01f,24nov98,jpd  added ARM940T support, added other CPU ids; removed
 mmuIntLock/Unlock as intIFLock/Unlock to be used instead;
 cdp  restructured for multiple cache/MMU libraries; big-endian.
 01e,09mar98,jpd  added MMUCTR_ definitions for ARM810 and extra cache states.
 01d,20jan98,jpd  added _func_armPhysToVirt() etc. function pointers.
 01c,17sep97,jpd  added MMU_INIT_VALUE and mmuIntLock/Unlock definitions.
 01b,24jan97,jpd  modified mmuAEnable definition.
 01a,09may96,cdp  created.
 */

#ifndef	__INCmmuArmLibh
#define	__INCmmuArmLibh

/*
 * N.B. although this library contains code written for the ARM810 CPU,
 * at the time of writing, this code has not been fully tested on
 * that CPU. YOU HAVE BEEN WARNED.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	_ASMLANGUAGE

#include "memLib.h"

#endif /* _ASMLANGUAGE */

/*
 * MMUs are not fitted to all processors, and so the definitions below
 * should not need to be included for processors without MMUs.
 */

#if 1 //(ARMMMU != ARMMMU_NONE) - shivam

/*
 * MMU identifiers as read from ID register (not all tested, some from
 * datasheets).
 */

#if (CPU==XSCALE)
#define MMU_ID_CPU_MASK		0xFFFFF000	/* Mask for CPU types */

#define MMU_ID_XSCALE           0x69052000	/* XScale needs validation */
#else
#define MMU_ID_CPU_MASK		0xFFFFF0	/* Mask for CPU types */

#define MMU_ID_ARM710A		0x047100
#define MMU_ID_ARM710T		0x807100
#define MMU_ID_ARM720T		0x807200
#define MMU_ID_ARM810		0x018100
#define MMU_ID_SA110		0x01A100
#define MMU_ID_SA1100		0x01A110
#define MMU_ID_SA1500		0x01A150
#define MMU_ID_ARM920T		0x029200
#define MMU_ID_ARM940T		0x029400
#define MMU_ID_ARM740T_8	0x807400	/* 8 kB cache variant */
#define MMU_ID_ARM740T_4	0x817400	/* 4 kB cache variant */
#define MMU_ID_ARM946E		0x049460
#endif /* if CPU=XSCALE */


/*
 * The page size we will use. Ignore the sub-page, Large Page and Tiny
 * Page features. Page size is still defined for MPUs, (it is the
 * minimum size for a region).
 */

#define PAGE_SIZE 4096


#if (!ARM_HAS_MPU)	/* definitions for page-table type MMUs */
/*
 * The amount described by a Level 1 Descriptor, which equals the smallest
 * amount of VM allocatable in VxWorks.
 */

#define PAGE_BLOCK_SIZE 0x100000	/* 1 MB */


#define PTE_INDEX_MASK 0x000FF000	/* extract PTE index from Virt Addr */
#define PTE_INDEX_SHIFT 12		/* shift to make that into PTE index */

#define L1D_TO_BASE_SHIFT 10		/* to turn L1D to PT base addr */

#define ADDR_PI_MASK 0x00000FFF		/* extract page index from Virt Addr */

#define ADDR_TO_PAGE 12	/* shift phys address to PTE page base address */


/* Level 1 Descriptor types */

#define DESC_TYPE_PAGE 1

#define DEF_L1_PAGE 0x00000011		/* domain zero, Page descriptor */

/*
 * Section descriptors, such as might be used to set up an intermediate
 * set of page tables on processors such as SA-1100/SA-1500 where this
 * needs to be done from BSP initialisation, before vm(Base)Lib sets up
 a proper set of page tables.
 */

#define RAM_SECT_DESC   	0xC1E  	/* R/W cacheable bufferable domain 0 */
#define MINICACHE_SECT_DESC	0xC1A   /* R/W C+ B- domain 0 */
#define OTHER_SECT_DESC		0xC12   /* R/W C- B- domain 0 */


/* Level 2 Descriptor or Page Table Entry (PTE) types */

#define PTE_TYPE_FAULT	 0		/* any access will cause a fault */
#define PTE_TYPE_SM_PAGE 2		/* Small page descriptor */
#define PTE_TYPE_EX_PAGE 3		/* Extended page descriptor */


/*
 * The Translation Table Base register (TTBR) points to a table of Level 1
 * Descriptors. these are either Invalid Section descriptors, Section
 * Descriptors, or Page Table Descriptors. If Page Table Descriptors, they
 * each point to a table of Level 2 Page Descriptors, or Page Table Entries
 * (PTEs).
 * The 32-bit (virtual) address space allows for 4096 M. Each Level 1
 * Descriptor describes a 1 M area of memory. There are therefore 4096 Level
 * 1 Descriptors, and each table of 256 Level 2 Page Descriptors (PTEs)
 * describes 256 4 kbyte pages.
 */

#define NUM_L1_DESCS 4096
#define NUM_L2_DESCS 256
#define L1_TABLE_SIZE (NUM_L1_DESCS * sizeof(LEVEL_1_DESC))


/* No. of pages a Level 1 Descriptor table takes up */

#define L1_DESC_PAGES (L1_TABLE_SIZE / PAGE_SIZE)


/* Size of a Page Table */

#define PAGE_TABLE_SIZE (NUM_L2_DESCS * sizeof(PTE))


/*
 * Architecture-dependent MMU states. These are states settable for pages and
 * here they correspond to settings in the Page Table Entries (PTEs).
 *
 * We set Valid/Invalid by setting a Fault second-level descriptor rather
 * than by using the Access Permissions within a small page second-level
 * descriptor. This is because we will want to use the Access Permissions to
 * allow read/write from supervisor mode and we cannot then use the AP bits
 * to disallow access as the SR bits in the MMU Control register must be set
 * to 01 in order to control read/write access from Supervisor mode.
 */

#define MMU_STATE_MASK_VALID		0x03		/* 2nd level desc type*/

#if (CPU==XSCALE)
#define MMU_STATE_MASK_WRITABLE		0x00000030	/* AP bits */
#else
#define MMU_STATE_MASK_WRITABLE		0x00000FF0	/* All APn bits */
#endif /* if CPU=XSCALE */

#define MMU_STATE_MASK_CACHEABLE	0x0000000C	/* CB bits */
#if (CPU==XSCALE)
#define MMU_STATE_MASK_EX_CACHEABLE	0x0000004C	/* X bit, and CB bits */
#endif

#if (CPU==XSCALE)
#define MMU_STATE_VALID			PTE_TYPE_EX_PAGE /* set to page type */
#else
#define MMU_STATE_VALID			PTE_TYPE_SM_PAGE /* set to page type */
#endif /* if CPU=XSCALE */

#define MMU_STATE_VALID_NOT		PTE_TYPE_FAULT	/* set to type fault */

#define MMU_STATE_WRITABLE_NOT		0x0		/* APn bits to 00 */

#if ((ARMMMU == ARMMMU_710A) || (ARMMMU == ARMMMU_720T) || \
     (ARMMMU == ARMMMU_XSCALE))
/* Write-through mode is only available on some CPUs */
#define MMU_STATE_CACHEABLE_WRITETHROUGH	0x8
#endif /* (ARMMMU == ARMMMU_710A/720T/XSCALE) */

/*
 * Set the default state to be copyback. CACHEABLE_WRITETHROUGH can also be
 * selected on 710A.
 */

/* MMU Control Register bit allocations */

#define MMUCR_M_ENABLE	 (1<<0)  /* MMU enable */
#define MMUCR_A_ENABLE	 (1<<1)  /* Address alignment fault enable */
#define MMUCR_C_ENABLE	 (1<<2)  /* (data) cache enable */
#define MMUCR_W_ENABLE	 (1<<3)  /* write buffer enable */
#define MMUCR_PROG32	 (1<<4)  /* PROG32 */
#define MMUCR_DATA32	 (1<<5)  /* DATA32 */
#define MMUCR_L_ENABLE	 (1<<6)  /* Late abort on earlier CPUs */
#define MMUCR_BIGEND	 (1<<7)  /* Big-endian (=1), little-endian (=0) */
#define MMUCR_SYSTEM	 (1<<8)  /* System bit, modifies MMU protections */
#define MMUCR_ROM	 (1<<9)  /* ROM bit, modifies MMU protections */
#define MMUCR_F		 (1<<10) /* Should Be Zero */
#define MMUCR_Z_ENABLE	 (1<<11) /* Branch prediction enable on 810 */
#define MMUCR_I_ENABLE	 (1<<12) /* Instruction cache enable */

#if ((defined CPU_946ES) || (defined CPU_946ES_T))
#define MMUCR_ALTVECT	     (1<<13) /* alternate vector select */
#define MMUCR_ROUND_ROBIN    (1<<14) /* round-robin placement */
#define MMUCR_DISABLE_TBIT   (1<<15) /* disable TBIT */
#define MMUCR_DSRAM_EN       (1<<16) /* D-SRAM enable  */
#define MMUCR_DSRAM_LOADMODE (1<<17) /* D-SRAM load mode select  */
#define MMUCR_ISRAM_EN       (1<<18) /* I-SRAM enable  */
#define MMUCR_ISRAM_LOADMODE (1<<19) /* I-SRAM load mode select  */
#endif /* CPU_946ES */

#if ((defined CPU_966ES) || (defined CPU_966ES_T))
#define MMUCR_ALTVECT	     (1<<13) /* alternate vector select */
#define MMUCR_DISABLE_TBIT   (1<<15) /* disable TBIT */
#endif /* CPU_966ES */

#if ((defined CPU_920T) || (defined CPU_920T_T) || \
     (defined CPU_940T) || (defined CPU_940T_T))
#define MMUCR_FB_DISABLE (1<<30) /* nFastBus bit */
#define MMUCR_ASYNC_BIT	 (1<<31) /* Async bit*/
#define MMUCR_SYNC	 (1<<30) /* Synchronous mode */
#define	MMUCR_ASYNC	 (3<<30) /* Asynchronous mode */
#endif	/* CPU_920T or CPU_940T */


/*
 * Values to be used when mmuEnable() is called. This will be after the MMU has
 * been initialised by sysInit()/romInit() and after cacheLib has set whatever
 * cache enable settings have been chosen.
 *
 * M 1 Enable MMU
 * A 0 Enable address alignment fault
 * C X ((D-)Cache Enable) Controlled by cacheLib
 * W X (Write Buffer) Controlled by cacheLib
 * P X (PROG32) should be set before this
 * D X (DATA32) should be set before this
 * L X (Late abort on earlier CPUs) ignore
 * B X (Big/Little-endian) should be set before this
 * S 0 (System)
 * R 1 (ROM)
 * F 0 Should be Zero
 * Z X (Branch prediction enable on 810) Controlled by cacheLib
 * I X (I-cache enable) Controlled by cacheLib
 *
 * For time being, do not enable the address alignment fault, as GCC
 * currently generates unaligned accesses in its code, and switching this
 * on will cause immediate faults. So, do not put it into the enable
 * mask.
 *
 * We used to clear all the Reserved/Should Be Zero bits when the
 * MMU/MPU was enabled, by including one bits for them in the definition
 * of MMU_ENABLE_MASK. We no longer do this, as CPU designers may extend
 * the definitions of the bits within the MMU Control Register. The MMU
 * Control register is initialised within romInit()/sysInit(), as the BSP
 * is the only place where the particular details of the CPU/MMU are
 * actually known. In general, it will be appropriate for the BSP to
 * initialise the MMUCR Reserved/Should Be Zero bits to zero. When the
 * MMU is enabled, we will only change the bits we are particularly
 * concerned about, by using a Read-Modify-Write strategy.
 */

#define MMU_ENABLE_MASK  (MMUCR_M_ENABLE | MMUCR_SYSTEM | MMUCR_ROM)


//#if 0
#if ((ARMMMU == ARMMMU_940T) || (ARMMMU == ARMMMU_920T))
/*
 * When enabling the 940T/920T, we should not alter the clocking mode that
 * has been selected earlier (by the BSP).
 */

#define MMU_ENABLE_MASK  (0x3FFFE000 | \
			MMUCR_M_ENABLE | \
			MMUCR_SYSTEM | MMUCR_ROM | MMUCR_F | MMUCR_Z_ENABLE)
#else
#define MMU_ENABLE_MASK  (0xFFFFE000 | \
			MMUCR_M_ENABLE | \
			MMUCR_SYSTEM | MMUCR_ROM | MMUCR_F | MMUCR_Z_ENABLE)
#endif /* (ARMMMU == ARMMMU_940T/920T) */
#endif /* <<< 0 */

#if ((ARMMMU == ARMMMU_710A) || (ARMMMU == ARMMMU_720T) || \
     (ARMMMU == ARMMMU_920T))
#define MMU_ENABLE_VALUE (MMUCR_M_ENABLE | MMUCR_ROM)
#endif

#if ((ARMMMU == ARMMMU_940T) || (ARMMMU == ARMMMU_740T) || \
     (ARMMMU == ARMMMU_946E))
#define MMU_ENABLE_VALUE (MMUCR_M_ENABLE)
#endif


/*
 * Values to be used when the MMU Control Register is initialised from
 * within romInit.s or sysALib.s (before the MMU is enabled). Note that
 * on 710A, this is also used to initialise the soft-copy of the MMU CR
 * that we need to keep (in order to emulate Read-Modify-Write
 * operations). So, 710A BSPs *MUST* initialise the MMU CR with the value
 * contained here, or else trouble will ensue. This restriction does
 * not apply to non-710A BSPs, but caution must be exercised in choosing a
 * value different to that defined here.
 */

#if (_BYTE_ORDER == _LITTLE_ENDIAN)
/* little-endian */

#if ((ARMMMU == ARMMMU_940T) || (ARMMMU == ARMMMU_946E))
/* W bit is Should Be One */
#define MMU_INIT_VALUE (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			MMUCR_W_ENABLE)
#endif

#if (ARMMMU == ARMMMU_740T)
#define MMU_INIT_VALUE	0
#endif

#if (ARMMMU == ARMMMU_920T)
/* W bit is Should Be One */
#define MMU_INIT_VALUE (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			MMUCR_ROM | MMUCR_W_ENABLE)
#endif

#else	/* (_BYTE_ORDER == _LITTLE_ENDIAN) */

/* big-endian */

#if ((ARMMMU == ARMMMU_710A) || (ARMMMU == ARMMMU_720T))
#define MMU_INIT_VALUE (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			MMUCR_BIGEND | MMUCR_ROM)
#endif

#if (ARMMMU == ARMMMU_810)
#define MMU_INIT_VALUE (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			MMUCR_BIGEND | MMUCR_ROM)
#endif

#if ((ARMMMU == ARMMMU_SA110) || (ARMMMU == ARMMMU_SA1100) || \
     (ARMMMU == ARMMMU_SA1500) || (ARMMMU == ARMMMU_XSCALE))
#define MMU_INIT_VALUE (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			MMUCR_BIGEND | MMUCR_ROM)
#endif

#if ((ARMMMU == ARMMMU_940T) || (ARMMMU == ARMMMU_946E))
#define MMU_INIT_VALUE (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			MMUCR_BIGEND | MMUCR_W_ENABLE)
#endif

#if (ARMMMU == ARMMMU_740T)
#define MMU_INIT_VALUE	MMUCR_BIGEND	
#endif

#if (ARMMMU == ARMMMU_920T)
#define MMU_INIT_VALUE (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			MMUCR_W_ENABLE | MMUCR_BIGEND | MMUCR_ROM)
#endif

#endif	/* _BYTE_ORDER == _LITTLE_ENDIAN */


#if (ARMMMU == ARMMMU_810)

/*
 * Definitions for register 15 of the MMU coprocessor (clock and test
 * configuration).
 */

#define MMUCTR_D 	(1<<0)	/* Enable Dynamic Clock switching */
#define MMUCTR_S 	(1<<1)	/* Enable Synchronous clock switching */
#define MMUCTR_F_BUS	(0<<2)	/* Fast clock source: ... bus clock */
#define MMUCTR_F_REF	(1<<2)	/* ... REFCLK */
#define MMUCTR_F_PLL	(3<<2)	/* ... PLL output clock */

#endif /* (ARMMMU == ARMMMU_810) */

#ifndef	_ASMLANGUAGE

#if (!ARM_HAS_MPU)

#if (_BYTE_ORDER == _LITTLE_ENDIAN)

/* little-endian */

typedef struct
{
UINT type	: 2;		/* descriptor type, 1 => page */
UINT pad1	: 2;		/* SBZ */
UINT pad2	: 1;		/* SBO */
UINT domain	: 4;		/* domain number */
UINT pad3	: 1;		/* SBZ */
UINT addr	: 22;		/* base address of page table */
} PAGE_DESC_FIELD;


/* Layout of Page Table Entries (PTEs), actually small page descriptors */

typedef struct
{
#if 0//(CPU==XSCALE)
UINT type	: 2;		/* page type, 3 => extended small page */
UINT cb	: 2;		/* cacheable/bufferable bits */
UINT ap	: 2;		/* access permission */
UINT tex 	: 4;		/* type extension field */
UINT sbz	: 2;		/* should be zero */
UINT addr	: 20;		/* page base address */
#else
  UINT type	: 2;		/* page type, 2 => small page */
  UINT cb	: 2;		/* cacheable/bufferable bits */
  UINT ap0	: 2;		/* access permission, sub-page 0 */
  UINT ap1	: 2;		/* access permission, sub-page 1 */
  UINT ap2	: 2;		/* access permission, sub-page 2 */
  UINT ap3	: 2;		/* access permission, sub-page 3 */
  UINT addr	: 20;		/* page base address */
#endif /* if CPU=XSCALE */
} PTE_FIELD;

#else /* (_BYTE_ORDER == _LITTLE_ENDIAN) */

/* big-endian */

typedef struct
    {
    UINT addr	: 22;		/* descriptor type, 1 => page */
    UINT pad3	: 1;		/* SBZ */
    UINT domain	: 4;		/* domain number */
    UINT pad2	: 1;		/* SBO */
    UINT pad1	: 2;		/* SBZ */
    UINT type	: 2;		/* base address of page table */
    } PAGE_DESC_FIELD;

/* Layout of Page Table Entries (PTEs), actually small page descriptors */

typedef struct
{
  #if (CPU==XSCALE)
      UINT addr	: 20;		/* page base address */
      UINT sbz	: 2;		/* should be zero */
      UINT tex 	: 4;		/* type extension field */
      UINT ap	: 2;		/* access permission */
      UINT cb	: 2;		/* cacheable/bufferable bits */
      UINT type	: 2;		/* page type, 3 => extended small page */
  #else
      UINT addr	: 20;		/* page base address */
      UINT ap3	: 2;		/* access permission, sub-page 3 */
      UINT ap2	: 2;		/* access permission, sub-page 2 */
      UINT ap1	: 2;		/* access permission, sub-page 1 */
      UINT ap0	: 2;		/* access permission, sub-page 0 */
      UINT cb	: 2;		/* cacheable/bufferable bits */
      UINT type	: 2;		/* page type, 2 => small page */
  #endif /* if CPU=XSCALE */
} PTE_FIELD;

#endif /* (_BYTE_ORDER == _LITTLE_ENDIAN) */

typedef union
    {
    PAGE_DESC_FIELD fields;
    UINT32 bits;
    } LEVEL_1_DESC;

typedef union
    {
    PTE_FIELD fields;
    UINT32 bits;
    } PTE;

typedef struct mmuTransTblStruct
    {
    LEVEL_1_DESC *pLevel1Table;
    } MMU_TRANS_TBL;

#else /* (!ARM_HAS_MPU) */

#define MPU_NUM_REGIONS 8

#if (_BYTE_ORDER == _LITTLE_ENDIAN)
typedef struct
    {
      UINT enable : 1;		/* 1 => region enabled */
      UINT size	: 5;		/* size  */
      UINT pad	: 6;		/* padding */
      UINT base	: 20;		/* region base address */
    } MPU_REGION_FIELDS;
#else /* (_BYTE_ORDER == _LITTLE_ENDIAN) */
/* Big-Endian */

typedef struct
    {
      UINT base	: 20;		/* region base address */
      UINT pad	: 6;		/* padding */
      UINT size	: 5;		/* size  */
      UINT enable : 1;		/* 1 => region enabled */
    } MPU_REGION_FIELDS;
#endif /* (_BYTE_ORDER == _LITTLE_ENDIAN) */

typedef union
{
  MPU_REGION_FIELDS fields;
  UINT32 bits;
} MPU_REGION_REG;

typedef struct mmuTransTblStruct
{
    MPU_REGION_REG regs[MPU_NUM_REGIONS];
} MMU_TRANS_TBL;

#endif /* (!ARM_HAS_MPU) */


/* Externally visible mmuLib, mmuMapLib and mmuALib2 routines */
IMPORT UINT32 mmuReadId (void);
IMPORT void * mmuVirtToPhys (void * addr);
IMPORT void * mmuPhysToVirt (void * addr);

IMPORT UINT32	(* mmuCrGet) (void);
IMPORT void	(* mmuModifyCr) (UINT32 value, UINT32 mask);
IMPORT UINT32	mmuHardCrGet (void);
IMPORT UINT32	mmuSoftCrGet (void);
IMPORT void	mmuModifyHardCr (UINT32 value, UINT32 mask);
IMPORT void	mmuModifySoftCr (UINT32 value, UINT32 mask);

#if	(ARMMMU == ARMMMU_710A)
#include "mmuArm710aLib.h"
#elif	(ARMMMU == ARMMMU_720T)
#include "mmuArm720tLib.h"
#elif	(ARMMMU == ARMMMU_740T)
#include "mmuArm740tLib.h"
#elif	(ARMMMU == ARMMMU_810)
#include "mmuArm810Lib.h"
#elif	(ARMMMU == ARMMMU_SA110)
#include "mmuArmSA110Lib.h"
#elif	(ARMMMU == ARMMMU_SA1100)
#include "mmuArmSA1100Lib.h"
#elif	(ARMMMU == ARMMMU_SA1500)
#include "mmuArmSA1500Lib.h"
#elif	(ARMMMU == ARMMMU_920T)
#include "mmuArm920tLib.h"
#elif	(ARMMMU == ARMMMU_940T)
#include "mmuArm940tLib.h"
#elif	(ARMMMU == ARMMMU_946E)
#include "mmuArm946eLib.h"
#elif	(ARMMMU == ARMMMU_XSCALE)
#include "mmuArmXSCALELib.h"
#endif	/* ARMMMU == ARMMMU_710A */


#if (!ARM_HAS_MPU)

/*
 * Pointer to a function that can be filled in by the BSP to point to a
 * function that returns a memory partition id for an area of memory to store
 * the Level 1 and Level 2 page tables. This area must be big enough for all
 * use. No provision is made to use that memory and then continue using
 * system memory once that has been filled.
 * N.B. at the time of writing, this feature has NEVER been tested at all.
 */

IMPORT PART_ID (* _func_armPageSource) (void);

#endif /* (!ARM_HAS_MPU) */

#endif	/* _ASMLANGUAGE */

#endif /* (ARMMMU != ARMMMU_NONE) */

#ifdef __cplusplus
}
#endif

#endif	/* __INCmmuArmLibh */
