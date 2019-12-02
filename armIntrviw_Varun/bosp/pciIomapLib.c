/* pciIomapLib.c - Support for PCI drivers */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,16aug04,scm  correct diab warnings...
01g,25apr02,to   use more appropriate macro.
01f,03dec01,rec  fix compiler warnings
01e,10sep01,jb   Fix PCI memory mapping
01d,17may00,jgn  fix pciFindDevice.
01c,08mar00,jpd  changed include of pciIomapLib.h to be from local directory.
01b,17feb00,jpd  made more style-guide-conformant.
01a,13jan00,pr   adapted from cma220.
*/


/*
DESCRIPTION

This library is PCI Revision 2.1 compliant.
This module contains routines to support PCI bus mapped on IO address space
for x86 and PowerPC architecture.  Functions in this library should not be
called from the interrupt level, except pciInt(), since it uses a mutual
exclusion semaphore for consecutive register access.
The functions addressed here include:

  - Initialize the library.
  - Locate the device by deviceID and vendorID.
  - Locate the device by classCode.
  - Generate the special cycle.
  - Access its configuration registers.
  - Connect a shared interrupt handler.
  - Disconnect a shared interrupt handler.
  - Master shared interrupt handler.

There are functions to access the IO address space.  In x86 architecture,
they are sysInXXX() and sysOutXXX().  Macro PCI_IN_XXX() and PCI_OUT_XXX()
are provided to use other IO address space functions.

The PCI IO base address is defined by the static variable pciIoBase.
Each new IO space will increment this variable (see function
pciAllocateIOSpace().

The V3 chip provides three windows through which accesses can be made by
system bus masters to the PCI bus. We use overlaping windows so to access
the IO space the mapping needs to be changed otherwise accesses will result in
a data error. For this window 1 is re-used and remapped to CPU_PCI_CNFG_ADRS.
However, before that window 0 must be remapped to ensure that application
software can access the address space normally assigned to window 1 while
window 1 is re-used for configuration cycles (cf. v3OpenConfigWindow())

Shared PCI interrupt are supported by three functions: pciInt(),
pciIntConnect(), pciIntDisconnect().  pciIntConnect() adds the specified
interrupt handler to the link list and pciIntDisconnect() removes it from
the link list.  Master interrupt handler pciInt() executes these interrupt
handlers in the link list for a PCI interrupt.  Each interrupt handler must
check the device dependent interrupt status bit to determine the source of
the interrupt, since it simply execute all interrupt handlers in the link
list.  pciInt() should be attached by intConnect() function in the BSP
initialization with its parameter. The parameter is an IRQ associated
to the PCI interrupt.

The maximum number of Type-1 Configuration Space buses supported in the 2.1
Specifications is 256 (0x00 - 0xFF), far greater than most systems currently
support.  Most buses are numbered sequentially from 0.  An optional define
called PCI_MAX_BUS may be declared in config.h to override the default
definition of 256.  Similarly, the default number of devices and functions
may be overriden by defining PCI_MAX_DEV and/or PCI_MAX_FUNC.  Note that
the number of devices applies only to bus zero, all others being restricted
to 16 by the 2.1 spec.
*/

#if defined(INCLUDE_PCI)

#include "vxWorks.h"
#include "config.h"
#include "dllLib.h"
#include "sysLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "intLib.h"
/*
 * We currently wish to include this from locally rather than from
 * there as that version is out-dated. However, as it has been included
 * in sysLib.c already, this will do no harm.
 */
#include "pciIomapLib.h"


/* defines */

/*
 * The following defines specify, by default, the maximum number of busses,
 * devices and functions allowed by the PCI 2.1 Specification.
 *
 * Any or all may be overriden by defining them in config.h.
 */

#ifndef PCI_MAX_BUS
#  define PCI_MAX_BUS	256
#endif  /* PCI_MAX_BUS */

#ifndef PCI_MAX_DEV
#  define PCI_MAX_DEV	21
#endif  /* PCI_MAX_DEV */

#ifndef PCI_MAX_FUNC
#  define PCI_MAX_FUNC	8
#endif  /* PCI_MAX_FUNC */

/* globals */

STATUS	pciLibInitStatus = NONE;	/* initialization done */
DL_LIST	pciIntList[PCI_IRQ_LINES];	/* link list of int handlers */
int	pciConfigMech = NONE;		/* 1=mechanism-1, 2=mechanism-2 */

static UINT32 pciIoBase = PCI_IO_ADRS; 	/* value for start of PCI IO */
static UINT32 pciMemBase = PCI_MEM_ADRS;/* value for start of PCI memory */

#define MAX(val1, val2)         ( ((val1) > (val2)) ? val1 : val2 )

#define DEFAULT_MEMORY_ALIGNMENT 	0x1000
#define DEFAULT_IO_ALIGNMENT 		0x800

/* locals */

LOCAL int pciConfigAddr0;		/* config-addr-reg, CSE-reg*/
LOCAL int pciConfigAddr1;		/* config-data-reg, forward-reg */
LOCAL int pciConfigAddr2;		/* not-used, base-addr */


/* V3 access macros */

#define V3_WRITE16(o,v) (*(volatile UINT16 *)(V3_BASE + (UINT32)(o)) = \
								(UINT16)(v))
#define V3_READ16(o)    (*(volatile UINT16 *)(V3_BASE + (UINT32)(o)))

#define V3_WRITE32(o,v) (*(volatile UINT32 *)(V3_BASE + (UINT32)(o)) = \
								(UINT32)(v))
#define V3_READ32(o)    (*(volatile UINT32 *)(V3_BASE + (UINT32)(o)))

/*******************************************************************************
*
* align - returns an aligned value
*
* RETURNS: aligned value
*/

static unsigned int align
    (
    unsigned int val,	/* value to align */
    unsigned int align
    )
    {
    /*
     * Some heuristics:
     * if the value is zero, it is aligned, no matter what the size is.
     */
    if (val == 0)
	return val;
    /* if the value is less than the alignment, return the alignment */
    if (val < align)
	return align;
    /* finally, if there is need to move the value upwards, do so... */
    if ((val & ~(align - 1)) != 0)
	return (((val) + ((align) - 1)) & ~((align) - 1));
    else
	return val;
    }

/*******************************************************************************
*
* v3OpenConfigWindow - open V3 configuration window
*
* RETURNS: N/A
*/
void v3OpenConfigWindow(void)
    {
    /*
     * Set up base0 to see all 512Mbytes of memory space (not
     * prefetchable), this frees up base1 for re-use by configuration
     * memory
     */

    V3_WRITE32 (V3_LB_BASE0, ((INTEGRATOR_PCI_BASE & 0xFFF00000) |
			     0x90 | V3_LB_BASE_M_ENABLE));
    /*
     * Set up base1 to point into configuration space, note that MAP1
     * register is set up by pciMakeConfigAddress().
     */

    V3_WRITE32 (V3_LB_BASE1, ((CPU_PCI_CNFG_ADRS & 0xFFF00000) |
			     0x40 | V3_LB_BASE_M_ENABLE));
    }

/*******************************************************************************
*
* v3CloseConfigWindow - close V3 configuration window
*
* RETURNS: N/A
*/

void v3CloseConfigWindow(void)
    {
    /* Reassign base1 for use by prefetchable PCI memory */

    V3_WRITE32 (V3_LB_BASE1, (((INTEGRATOR_PCI_BASE + SZ_256M) & 0xFFF00000)
					| 0x84 | V3_LB_BASE_M_ENABLE));
    V3_WRITE16 (V3_LB_MAP1,
	    (((INTEGRATOR_PCI_BASE + SZ_256M) & 0xFFF00000) >> 16) | 0x0006);

    /* And shrink base0 back to a 256M window (NOTE: MAP0 already correct) */

    V3_WRITE32 (V3_LB_BASE0, ((INTEGRATOR_PCI_BASE & 0xFFF00000) |
			     0x80 | V3_LB_BASE_M_ENABLE));
    }

/*******************************************************************************
*
* pciMapInterrupt - map a PCI interrupt
*
* This routine caclculates the interrupt number that this slot/pin
* combination will use.
*
* RETURNS: board-specific interrupt number
*/

UINT8 pciMapInterrupt
    (
    UINT8 pin, 	/* pin number (A=1, B=2, C=3, D=4) */
    UINT8 slot	/* slot number (IDSEL = slot + 11) */
    )
    {

#define INTA INT_LVL_PCI_0
#define INTB INT_LVL_PCI_1
#define INTC INT_LVL_PCI_2
#define INTD INT_LVL_PCI_3

  /*
   * DANGER! For now this is the SDM interrupt table...
   */
    char irq_tab[12][4] =
        {
             /* INTA  INTB  INTC  INTD */
             {INTA, INTB, INTC, INTD},	/* idsel 20, slot  9  */
             {INTB, INTC, INTD, INTA},	/* idsel 21, slot 10  */
             {INTC, INTD, INTA, INTB},	/* idsel 22, slot 11  */
             {INTD, INTA, INTB, INTC},	/* idsel 23, slot 12  */
             {INTA, INTB, INTC, INTD},	/* idsel 24, slot 13  */
             {INTB, INTC, INTD, INTA},	/* idsel 25, slot 14  */
             {INTC, INTD, INTA, INTB},	/* idsel 26, slot 15  */
             {INTD, INTA, INTB, INTC},	/* idsel 27, slot 16  */
             {INTA, INTB, INTC, INTD},	/* idsel 28, slot 17  */
             {INTB, INTC, INTD, INTA},	/* idsel 29, slot 18  */
             {INTC, INTD, INTA, INTB},	/* idsel 30, slot 19  */
             {INTD, INTA, INTB, INTC}	/* idsel 31, slot 20  */
        };

    /* if PIN = 0, default to A */

    if (pin == 0)
        pin = 1;

    /* return the magic number */

    return irq_tab[slot - 9][pin - 1];
    }

/*******************************************************************************
*
* pciAllocateMemSpace - allocate space in PCI memory
*
* This routine allocates space within PCI memory space.
*
* RETURNS: Where in PCI memory allocated
*
* NOTE: Calling this code has a side effect of moving the global
*       Memory base on.
*/

static unsigned int pciAllocateMemSpace
    (
    unsigned int size
    )
    {
    unsigned int alignto;
    unsigned int base;

    /* align in minimum sized chunks of DEFAULT_MEMORY_ALIGNMENT */

    alignto	= MAX (size, DEFAULT_MEMORY_ALIGNMENT);
    base	= align (pciMemBase, alignto);
    pciMemBase	= base + size;

    return base;
    }

/*******************************************************************************
*
* pciAllocateIOSpace - allocate space in PCI I/O memory
*
* This routine allocates space requested within the PCI I/O space.
*
* RETURNS: where in PCI memory allocated
*
* NOTE: Calling this code has a side effect of moving the global
*       I/O base on.
*/

static unsigned int pciAllocateIOSpace
    (
    unsigned int size
    )
    {
    unsigned int alignto;
    unsigned int base;

    /* align in minimum sized chunks of DEFAULT_IO_ALIGNMENT */

    alignto 	= MAX (size, DEFAULT_IO_ALIGNMENT);
    base	= align (pciIoBase, alignto);
    pciIoBase 	= base + size;

    return base;
    }

/*******************************************************************************
*
* pciAllocateInterrupt - allocate an interrupt number
*
* This routine allocates an interrupt number.
*
* RETURNS: N/A
*/

void pciAllocateInterrupt
    (
    int bus,		/* bus number */
    int slot,		/* device number */
    int func		/* function number */
    )
    {
    char pin;
    unsigned char interrupt;

    /* read the interrupt pin from the device */

    pciConfigInByte (bus, slot, func, PCI_CFG_DEV_INT_PIN, &pin);

    /* work out the interrupt number from the slot/pin */

    interrupt = pciMapInterrupt (pin, slot);

    /* write it to the device (this will be read later by a device driver) */

    pciConfigOutByte (bus, slot, func, PCI_CFG_DEV_INT_LINE, interrupt);
    }

/*******************************************************************************
*
* pciAssignResources - assign PCI resources
*
* This routine will attempt to glean resource requirements from the
* device specified.
*
* RETURNS: N/A
*/

void pciAssignResources
    (
    int bus,		/* bus number */
    int slot,		/* device number */
    int func		/* function number */
    )
    {
    unsigned int offset = PCI_MEM_BAR;
    unsigned int base, mask, size;
    int i, barFlag = 0;

    /*
     * set up the bus, slot and func
     *
     * Disable PCI I/O and memory accesses
     */

    pciConfigOutWord (bus, slot, func, PCI_CFG_COMMAND, 0);

    /* allocate an irq number */

    pciAllocateInterrupt (bus, slot, func);

    /* Scan each of the BARS for this device */

    for (i = 0; i < PCI_MAX_BAR; i++)
        {
	/* write PCI_INVALID to BAR to get size, then read it back */

	pciConfigOutLong (bus, slot, func, offset, (int) PCI_INVALID);
	pciConfigInLong (bus, slot, func, offset, (int *) &base);

	/* check if this is an implemented BAR (size != 0) */

	if (base != 0)
	    {
	    /* Assign some PCI memory (IO or Memory) space to this BAR */

	    if (base & PCI_CMD_IO_ENABLE)
	       {
	       /*	-- IO space BAR --
		*
		* The bottom 2 bits of the BAR for PCI IO look like this:
	        *     0    must be 1 (0 = memory, 1 = IO)
		*     1    reserved
		*
		* figure out how much memory is wanted
		*/

		base &= (~0x3);
		mask  = (~base << 1) | 0x1;
		size  = (mask & base) & 0xFFFFFFFF;

		/* allocate (aligned) space in IO */

		base = pciAllocateIOSpace (size);
		pciConfigOutLong(bus, slot, func, offset, base | 0x1);
		barFlag |= PCI_CMD_IO_ENABLE;
		}
	    else
		{
		/* 	-- MEM space BAR --
		 *
		 * WARNING: we assume only one type of memory - dangerous!
		 */

		unsigned int type;

		/*
		 * The bottom 4 bits of the BAR for PCI memory look like this:
		 *        0    must be zero (0 = memory, 1 = IO)
		 *       1:2   type of space needed
		 *       (00 = 32 bit, 01 = below 1M, 10 = 64 bit address)
		 *       3     prefetchable
		 */

		type = base & 0x6;
		base &= (~0xf);
		mask = (~base << 1) | 0x1;
		size = (mask & base) & 0xFFFFFFFF;

		/* cope (or rather don't) with different flavours of memory */

		switch (type)
		    {
		    case 0x00:		/* 32 bit */
			 break;
		    case 0x02:		/* below 1M */
			 continue;
		    case 0x04:		/* 64 bit */
			 offset += 4;
			 continue;
		    default:
			 continue;
		    }

		/* allocate some (aligned) space in PCI memory */

		base = pciAllocateMemSpace (size);
		pciConfigOutLong(bus, slot, func, offset, base);
		barFlag |= PCI_CMD_MEM_ENABLE;
		}
	    }

	  /* increment the BAR offset value */

	  offset += 4;
        }

    /* now do the ROM stuff */

    pciConfigOutLong (bus, slot, func, PCI_ROM_BAR, (int) PCI_INVALID);
    pciConfigInLong (bus, slot, func, PCI_ROM_BAR, (int *) &base);
    if (base & 0x1)
      {
      /* valid ROM space, figure out how much it wants/needs */

      base &= ~0x1;
      size = (~base + 1);

      /* Allocate some (aligned) memory space */

      base = pciAllocateMemSpace (size);

      /*
       * Let the device see it, but don't enable accesses.
       * This is because, according to the specification,
       * the device may share an address decoder between
       * the Expansion ROM base and other base address
       * registers.  In other words, any device driver wishing
       * to access the ROM must set the enable bit (bit 0) before
       * proceeding and clear it afterwards.
       */

      pciConfigOutLong (bus, slot, func, PCI_ROM_BAR, base);

      /* ROM gets added to PCI memory, so allow accesses */

      barFlag |= PCI_CMD_MEM_ENABLE;
      }

    /* Enable PCI IO and memory accesses as found by scan, with MASTER */

    barFlag |= PCI_CMD_MASTER_ENABLE;
    pciConfigOutWord (bus, slot, func, PCI_CFG_COMMAND, barFlag);
    pciConfigOutByte (bus, slot, func, PCI_CFG_LATENCY_TIMER, 0xFF);
    }

/*******************************************************************************
*
* pciMakeConfigAddress - make a Daconfiguration address()
*
* This routine will generate a platform dependent configuration address.
*
* To generate the appropriate PCI configuration cycles in the PCI
* configuration address space, you present the V3 with the following pattern
* (which is very nearly a type 1 (except that the lower two bits are 00 and
* not 01).   In order for this mapping to work you need to set up one of
* the local to PCI aperatures to 16Mbytes in length translating to
* PCI configuration space starting at 0x0000.0000.
*
* PCI configuration cycles look like this:
*
* Type 0:
*
*  3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1
*  3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* | | |D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|D|F|F|F|R|R|R|R|R|R|0|0|
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*	31:11	Device select bit.
* 	10:8	Function number
* 	 7:2	Register number
*
* Type 1:
*
*  3 3|3 3 2 2|2 2 2 2|2 2 2 2|1 1 1 1|1 1 1 1|1 1
*  3 2|1 0 9 8|7 6 5 4|3 2 1 0|9 8 7 6|5 4 3 2|1 0 9 8|7 6 5 4|3 2 1 0
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* | | | | | | | | | | |B|B|B|B|B|B|B|B|D|D|D|D|D|F|F|F|R|R|R|R|R|R|0|1|
* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*
*	31:24	reserved
*	23:16	bus number (8 bits = 128 possible buses)
*	15:11	Device number (5 bits)
*	10:8	function number
*	 7:2	register number
*
* RETURNS: configuration address on the PCI bus
*/

UINT32 pciMakeConfigAddress
    (
    int bus,	 	/* bus number */
    int slot, 		/* slot number */
    int function, 	/* function number */
    int offset		/* offset into the configuration space */
    )
    {
    UINT32 address, devicebit;
    UINT16 mapaddress;

    if (bus == 0)
        {
        /*
	 * local bus segment so need a type 0 config cycle
         * build the PCI configuration "address" with one-hot in A31-A11
	 */

        address = CPU_PCI_CNFG_ADRS;
        address |= ((function & 0x07) << 8);
        address |= offset & 0xFF;

	/* 101=>config cycle, 0=>A1=A0=0 */

        mapaddress = 0x000A;

        devicebit = (1 << (slot + 11));

        if ((devicebit & 0xFF000000) != 0)
            {
            /* high order bits are handled by the MAP register */

            mapaddress |= (devicebit >> 16);
            }
        else
            {
            /* low order bits handled directly in the address */

            address |= devicebit;
            }
        }
    else
        {
	/* 	-- bus !=0 --
	 *
         * not the local bus segment so need a type 1 config cycle
         * A31-A24 are don't care (so clear to 0)
	 */

        mapaddress = 0x000B;	/* 101=>config cycle, 1=>A1&A0 from PCI_CFG */

        address = CPU_PCI_CNFG_ADRS;
        address |= ((bus & 0xFF) << 16);	/* bits 23..16 = bus number */
        address |= ((slot & 0x1F) << 11);  	/* bits 15..11 = slot number */
        address |= ((function & 0x07) << 8);  	/* bits 10..8  = function num */
        address |= offset & 0xFF;  		/* bits  7..0  = register num */
        }

    V3_WRITE16 (V3_LB_MAP1, mapaddress);

    return address;
    }

/*******************************************************************************
*
* pciConfigInByte - read a byte from PCI config register.
*
* This routine reads a byte from the given PCI config register.
*
*
* RETURNS: the data value read.
*/

STATUS pciConfigInByte
    (
    int bus,		/* bus number */
    int slot,		/* slot number */
    int function,	/* function number */
    int offset,		/* offset into the configuration space */
    char *data		/* data read from the offset */
    )
    {
    char *pAddress;
    int	  key;

    /* mutual exclusion start */

    key = intLock ();

    /* open the (closed) configuration window from local bus memory */

    v3OpenConfigWindow();

    /* generate the address of correct configuration space */

    pAddress = (char *)pciMakeConfigAddress(bus, slot, function, offset);

    /* now that we have valid params, go read the config space data */

    *data = *pAddress;

    /* close the window */

    v3CloseConfigWindow();

    /* end of mutual exclusion */

    intUnlock (key);

    return (OK);
    }

/*******************************************************************************
*
* pciConfigInWord - read a word from PCI config register.
*
* This routine reads a word from the given PCI config register.
*
* RETURNS: the data value read.
*/

STATUS pciConfigInWord
    (
    int bus,		/* bus number */
    int slot,		/* slot number */
    int function,	/* function number */
    int offset,		/* offset into the configuration space */
    short *data		/* data read from the offset */
    )
    {
    short *	pAddress;
    int		key;

    /* mutual exclusion start */

    key = intLock ();

    /* open the (closed) configuration window from local bus memory */

    v3OpenConfigWindow();

    /* generate the address of correct configuration space */

    pAddress = (short *)pciMakeConfigAddress(bus, slot, function, offset);

    /* now that we have valid params, go read the config space data */

    *data = *pAddress;

    /* close the window */

    v3CloseConfigWindow();

    /* end of mutual exclusion */

    intUnlock (key);

    return (OK);
    }

/*******************************************************************************
*
* pciConfigInLong - read a long from PCI config register.
*
* This routine reads a long from the given PCI config register.
*
* RETURNS: the data value read.
*/

STATUS pciConfigInLong
    (
    int bus,		/* bus number */
    int slot,		/* slot number */
    int function,	/* function number */
    int offset,		/* offset into the configuration space */
    int *data		/* data read from the offset */
    )
    {
    int *	pAddress;
    int		key;

    /* mutual exclusion start */

    key = intLock();

    /* open the (closed) configuration window from local bus memory */

    v3OpenConfigWindow();

    /* generate the address of correct configuration space */

    pAddress = (int *)pciMakeConfigAddress(bus, slot, function, offset);

    /* now that we have valid params, go read the config space data */

    *data = *pAddress;

    /* close the window */

    v3CloseConfigWindow();

    /* mutual exclusion stop */

    intUnlock(key);

    return (OK);
    }

/*******************************************************************************
*
* pciConfigOutByte - write a byte to configuration space
*
* This routine writes a byte value to the given PCI config register.
*
* RETURNS: N/A
*/

STATUS pciConfigOutByte
     (
     int bus,		/* bus number */
     int slot,	 	/* slot number */
     int function,	/* function number */
     int offset, 	/* offset into the configuration space */
     char data		/* data written to the offset */
     )
    {
    char *pAddress;
    int   key;

    /* mutual exclusion start */

    key = intLock();

    /* open the (closed) configuration window from local bus memory */

    v3OpenConfigWindow();

    if (bus != 0)
        {
        /* workaround for problem with type 0 write */

        pAddress  = (UINT8 *)CPU_PCI_CNFG_ADRS;
        *pAddress = data;
        }
     else
	{
	/* generate the address of correct configuration space */

	pAddress = (char *)pciMakeConfigAddress(bus, slot, function, offset);

	/* now that we have valid params, go write the config space data */

	*pAddress = data;
	}

    /* close the window */

    v3CloseConfigWindow();

    /* mutual exclusion stop */

    intUnlock(key);

    return OK;
    }

/*******************************************************************************
*
* pciConfigOutWord - write a word to configuration space
*
* This routine writes a word value to the given PCI config register.
*
* RETURNS: N/A
*/

STATUS pciConfigOutWord
    (
    int bus,		/* bus number */
    int slot,	 	/* slot number */
    int function, 	/* function number */
    int offset, 	/* offset into the configuration space */
    short data		/* data written to the offset */
    )
    {
    short *	pAddress;
    int		key;

    /* mutual exclusion start */

    key = intLock();

    /* open the (closed) configuration window from local bus memory */

    v3OpenConfigWindow();

    if (bus != 0)
        {
	/* workaround for problem with type 0 write */

	pAddress  = (short *)CPU_PCI_CNFG_ADRS;
	*pAddress = data;
	}
    else
        {
        /* generate the address of correct configuration space */

	pAddress = (short *)pciMakeConfigAddress(bus, slot, function, offset);

	/* now that we have valid params, go write the config space data */

	*pAddress = data;
	}

    /* close the window */

    v3CloseConfigWindow();

    /* mutual exclusion stop */

    intUnlock(key);

    return OK;
    }

/*******************************************************************************
*
* pciConfigOutLong - write a long to configuration space
*
* This routine writes a long value to the given PCI config register.
*
* RETURNS: N/A
*/

STATUS pciConfigOutLong
    (
    int bus,	 	/* bus number */
    int slot,	 	/* slot number */
    int function, 	/* function number */
    int offset, 	/* offset into the configuration space */
    int data		/* data written to the offset */
    )
    {
    int *	pAddress;
    int		key;

    /* mutual exclusion start */

    key = intLock();

    /* open the (closed) configuration window from local bus memory */

    v3OpenConfigWindow();

    if (bus != 0)
        {
	/* workaround for problem with type 0 write */

	pAddress  = (int *)CPU_PCI_CNFG_ADRS;
	*pAddress = data;
	}
    else
        {
	/* generate the address of correct configuration space */

	pAddress = (int *)pciMakeConfigAddress(bus, slot, function, offset);

	/* now that we have valid params, go write the config space data */

	*pAddress = data;
	}

    /* close the window */

    v3CloseConfigWindow();

    /* mutual exclusion stop */

    intUnlock(key);

    return OK;
    }


/*******************************************************************************
*
* pciIomapLibInit - initialize the configuration access-method and addresses
*
* This routine initializes the configuration access-method and addresses.
*
* Configuration mechanism one utilizes two 32-bit IO ports located at addresses
* 0x0cf8 and 0x0cfc. These two ports are:
*   - 32-bit configuration address port, at 0x0cf8
*   - 32-bit configuration data port, at 0x0cfc
* Accessing a PCI function's configuration port is two step process.
*   - Write the bus number, physical device number, function number and
*     register number to the configuration address port.
*   - Perform an IO read from or an write to the configuration data port.
*
* Configuration mechanism two uses following two single-byte IO ports.
*   - Configuration space enable, or CSE, register, at 0x0cf8
*   - Forward register, at 0x0cfa
* To generate a PCI configuration transaction, the following actions are
* performed.
*   - Write the target bus number into the forward register.
*   - Write a one byte value to the CSE register at 0x0cf8.  The bit
*     pattern written to this register has three effects: disables the
*     generation of special cycles; enables the generation of configuration
*     transactions; specifies the target PCI functional device.
*   - Perform a one, two or four byte IO read or write transaction within
*     the IO range 0xc000 through 0xcfff.
*
* Configuration mechanism three is for non-PC/PowerPC environments
* where an area of address space produces PCI configuration
* transactions. No support for special cycles is included.
*
* RETURNS: OK or ERROR if a mechanism is not 1, 2 or 3.
*
*/

STATUS pciIomapLibInit
    (
    int mechanism,	/* configuration mechanism: 1, 2 or 3 */
    int addr0,		/* config-addr-reg / CSE-reg */
    int addr1,		/* config-data-reg / Forward-reg */
    int addr2		/* none            / Base-address */
    )
    {
    int ix;

    if (pciLibInitStatus != NONE)
	return (pciLibInitStatus);

    switch (mechanism)
	{
	case PCI_MECHANISM_1:
	case PCI_MECHANISM_2:
	case PCI_MECHANISM_3:
	    pciConfigMech	= mechanism;
	    pciConfigAddr0	= addr0;
	    pciConfigAddr1	= addr1;
	    pciConfigAddr2	= addr2;
	    pciLibInitStatus	= OK;
	    break;

	default:
    	    pciLibInitStatus	= ERROR;
	    break;
	}

    for (ix = 0; ix < PCI_IRQ_LINES; ix++)
        dllInit (&pciIntList[ix]);

    return (pciLibInitStatus);
    }

/*******************************************************************************
*
* pciFindDevice - find the nth device with the given device & vendor ID
*
* This routine finds the nth device with the given device & vendor ID.
*
* RETURNS: OK or ERROR if the deviceId and vendorId didn't match.
*
*/

STATUS pciFindDevice
    (
    int    vendorId,	/* vendor ID */
    int    deviceId,	/* device ID */
    int    index,	/* desired instance of device */
    int *  pBusNo,	/* bus number */
    int *  pDeviceNo,	/* device number */
    int *  pFuncNo	/* function number */
    )
    {
    STATUS status = ERROR;
    BOOL   cont   = TRUE;
    int    busNo;
    int    deviceNo;
    int    funcNo;
    UINT16 device;
    UINT16 vendor;
    UINT8  header;
    UINT32 devClass;
    int    is_a_multi_function_device = 0;

    if (pciLibInitStatus != OK)
        cont = FALSE;

    for (busNo = 0; cont == TRUE && busNo < PCI_MAX_BUS; busNo++)
        for (deviceNo = 0; cont == TRUE && deviceNo < 31; ++deviceNo)
	    {

	    is_a_multi_function_device = 0;

            for (funcNo = 0; cont == TRUE && funcNo < PCI_MAX_FUNC; funcNo++)
		{

		if ( (funcNo != 0) && !is_a_multi_function_device)
		   continue;

		pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID,
				 (short *) &vendor);

		/* If nonexistent device, skip to next (PCI_NOTFITTED)*/

		if (vendor == 0xFFFF)
		    break;

		pciConfigInByte(busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE,
				&header);

		if (funcNo == 0)
		    {
		    is_a_multi_function_device = ((header & 0x80) != 0);
		    }

		pciConfigInWord(busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID,
				(short *) &device);

		if ((vendor == (UINT16)vendorId) &&
		    (device == (UINT16)deviceId) &&
		    (index-- == 0))
		    {
		      pciConfigInLong (busNo, deviceNo, funcNo,
		      		       PCI_CFG_REVISION, (int *) &devClass);
		      devClass = (devClass & 0xFFFFFF00) >> 8;
		      if ( (devClass >> 16) == 0x06) {
			/* BRIDGE... */

		      }

		    *pBusNo	= busNo;
		    *pDeviceNo	= deviceNo;
		    *pFuncNo	= funcNo;
		    status	= OK;

		    /* terminate all loops */

		    cont	= FALSE;
		    continue;
		    }
		}
	    }
    return (status);
    }

/*******************************************************************************
*
* pciFindClass - find the nth occurence of a device by PCI class code.
*
* This routine finds the nth device with the given 24-bit PCI class code
* (class subclass prog_if).
*
* RETURNS: OK or ERROR if the class didn't match.
*
*/

STATUS pciFindClass
    (
    int    classCode,	/* 24-bit class code */
    int	   index,	/* desired instance of device */
    int *  pBusNo,	/* bus number */
    int *  pDeviceNo,	/* device number */
    int *  pFuncNo	/* function number */
    )
    {
    STATUS status = ERROR;
    BOOL   cont   = TRUE;
    int    busNo;
    int    deviceNo;
    int    funcNo;
    int    classCodeReg;
    char   header;

    if (pciLibInitStatus != OK)
        cont = FALSE;

    for (busNo = 0; cont == TRUE && busNo < PCI_MAX_BUS; busNo++)
        for (deviceNo = 0;
	     cont == TRUE && deviceNo < ((busNo == 0) ? PCI_MAX_DEV : 16);
	     ++deviceNo)
            for (funcNo = 0; cont == TRUE && funcNo < PCI_MAX_FUNC; funcNo++)
		{
#if (PCI_MAX_DEV > 31) && (PCI_MAX_FUNC > 7)
		/* avoid a special bus cycle */

		if ((deviceNo == 0x1f) && (funcNo == 0x07))
		    continue;

#endif
		pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_REVISION,
				 &classCodeReg);

		/* If nonexistent device, skip to next */

		if (((UINT32)classCodeReg == 0xFFFFFFFF) && (funcNo == 0))
		    break;

		if ((((classCodeReg >> 8) & 0x00ffffff) == classCode) &&
		    (index-- == 0))
		    {
		    *pBusNo	= busNo;
		    *pDeviceNo	= deviceNo;
		    *pFuncNo	= funcNo;
		    status	= OK;

		    /* terminate all loops */

		    cont 	= FALSE;
		    continue;
		    }

		/* goto next if current device is single function */

		pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE,
				 &header);
		if ((header & PCI_HEADER_MULTI_FUNC) != PCI_HEADER_MULTI_FUNC &&
		    funcNo == 0)
		    break;
		}

    return (status);
    }


/******************************************************************************
*
* pciDevConfig - configure a device on a PCI bus
*
* This routine configures a device that is on a Peripheral Component
* Interconnect (PCI) bus by writing to the configuration header of the
* selected device.
* It first disables the device by clearing the command register in the
* configuration header.  It then sets the I/O and/or memory space base
* address registers, the latency timer value and the cache line size.
* Finally, it re-enables the device by loading the command register with
* the specified command.
*
* NOTE: This routine is designed for Type 0 PCI Configuration Headers ONLY.
*       It is NOT usable for configuring, for example, a PCI-to-PCI bridge.
*
* RETURNS: OK always.
*/

STATUS pciDevConfig
    (
    int		pciBusNo,		/* PCI bus number */
    int		pciDevNo,		/* PCI device number */
    int		pciFuncNo,		/* PCI function number */
    ULONG       devIoBaseAdrs,          /* device IO base address */
    ULONG       devMemBaseAdrs,         /* device memory base address */
    ULONG       command                 /* command to issue */
    )
    {
    INT32       ix;
    UINT32      tmp32;

    /*
     * Disable device by clearing its command register field in its
     * configuration header.
     */

    pciConfigOutWord (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_COMMAND, 0);

    /*
     *  Due to system constraints, this is a partial implementation
     *  of enabling the Base Address Registers (BARs).
     *  It is hoped that all PCI devices only require at most one
     *  I/O space and/or one memory space.
     *  If not, this code will re-allocate the device's memory to
     *  each BAR implemented.  Sounds like future trouble!
     */

    for (ix = PCI_CFG_BASE_ADDRESS_0; ix <= PCI_CFG_BASE_ADDRESS_5; ix+=4)
        {
        /* Write all f's and read back value */

        pciConfigOutLong (pciBusNo, pciDevNo, pciFuncNo, ix, (int) 0xffffffff);
        pciConfigInLong  (pciBusNo, pciDevNo, pciFuncNo, ix, (int *) &tmp32);

        /* BAR implemented? */

        if (tmp32 == 0)
           {
           /*
            * No! According to the spec, BARs must be implemented
            * in sequence starting at BAR 0.  So, all done.
            */
           break;
           }

        if (tmp32 & 0x1)
	   {
	   /* I/O space requested, set specified I/O space base address */

           pciConfigOutLong (pciBusNo, pciDevNo, pciFuncNo, ix,
			     devIoBaseAdrs | 0x1);
	   }
        else
           {
	   /* Memory space required, set specified base address */

           pciConfigOutLong (pciBusNo, pciDevNo, pciFuncNo, ix,
			     devMemBaseAdrs & ~0x1);
           }
        }

    /* Configure Latency Timer */

    pciConfigOutByte (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_LATENCY_TIMER,
		      0xFF);

    /* Enable the device's capabilities as specified */

    pciConfigOutWord (pciBusNo, pciDevNo, pciFuncNo, PCI_CFG_COMMAND,
  		      (UINT16)command);

    return (OK);
    }

/*******************************************************************************
*
* pciPack - pack parameters for the Configuration Address Register
*
* This routine packs three parameters into one integer for accessing the
* Configuration Address Register
*
* RETURNS: packed integer
*
*/

LOCAL int pciPack
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo		/* function number */
    )
    {
    return (((busNo    << 16) & 0x00ff0000) |
	    ((deviceNo << 11) & 0x0000f800) |
	    ((funcNo   << 8)  & 0x00000700));
    }

/*******************************************************************************
*
* pciSpecialCycle - generate a special cycle with a message
*
* This routine generates a special cycle with a message.
*
* RETURNS: OK or ERROR if this library is not initialized.
*
*/

STATUS pciSpecialCycle
    (
    int	busNo,		/* bus number */
    int message		/* data on AD[31:0] during the special cycle */
    )
    {
    int deviceNo	= 0x0000001f;
    int funcNo		= 0x00000007;
    int key;

    if (pciLibInitStatus != OK)
        return (ERROR);

    /* mutual exclusion start */

    key = intLock();

    switch (pciConfigMech)
	{
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciPack (busNo, deviceNo, funcNo) |
		          0x80000000);
	    PCI_OUT_LONG (pciConfigAddr1, message);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xff);
	    PCI_OUT_BYTE (pciConfigAddr1, 0x00);
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8)),
			  message);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    /* mutual exclusion stop */

    intUnlock(key);

    return (OK);
    }

/*******************************************************************************
*
* pciInt - interrupt handler for shared PCI interrupt.
*
* This routine executes multiple interrupt handlers for a PCI interrupt.
* Each interrupt handler must check the device dependent interrupt status bit
* to determine the source of the interrupt, since it simply execute all
* interrupt handlers in the link list.
*
* RETURNS: N/A
*
*/

VOID pciInt
    (
    int irq		/* IRQ associated to the PCI interrupt */
    )
    {
    PCI_INT_RTN *pRtn;

    for (pRtn = (PCI_INT_RTN *)DLL_FIRST (&pciIntList[irq]); pRtn != NULL;
	 pRtn = (PCI_INT_RTN *)DLL_NEXT (&pRtn->node))
	(* pRtn->routine) (pRtn->parameter);
    }

/*******************************************************************************
*
* pciIntConnect - connect the interrupt handler to the PCI interrupt.
*
* This routine connects an interrupt handler to the PCI interrupt line(A - D).
* Link list is created if multiple handlers are assigned to the single PCI
* interrupt.
*
* RETURNS: OK or ERROR if the interrupt handler cannot be built.
*
*/

STATUS pciIntConnect
    (
    VOIDFUNCPTR *vector,        /* interrupt vector to attach to     */
    VOIDFUNCPTR routine,        /* routine to be called              */
    int parameter               /* parameter to be passed to routine */
    )
    {
    int irq = IVEC_TO_INUM ((int)vector) - INT_NUM_IRQ0;
    PCI_INT_RTN *pRtn;
    int oldLevel;

    if (pciLibInitStatus != OK)
	return (ERROR);

    pRtn = (PCI_INT_RTN *)malloc (sizeof (PCI_INT_RTN));
    if (pRtn == NULL)
	return (ERROR);

    pRtn->routine   = routine;
    pRtn->parameter = parameter;

    oldLevel = intLock ();			/* LOCK INTERRUPT */
    dllAdd (&pciIntList[irq], &pRtn->node);
    intUnlock (oldLevel);			/* UNLOCK INTERRUPT */

    return (OK);
    }

/*******************************************************************************
*
* pciIntDisconnect - disconnect the interrupt handler from the PCI interrupt.
*
* This routine disconnects the interrupt handler from the PCI interrupt line.
*
* RETURNS: OK or ERROR if the interrupt handler cannot be removed.
*
*/

STATUS pciIntDisconnect
    (
    VOIDFUNCPTR *vector,        /* interrupt vector to attach to     */
    VOIDFUNCPTR routine         /* routine to be called              */
    )
    {
    int irq = IVEC_TO_INUM ((int)vector) - INT_NUM_IRQ0;
    PCI_INT_RTN *pRtn;
    int oldLevel;

    if (pciLibInitStatus != OK)
	return (ERROR);

    for (pRtn = (PCI_INT_RTN *)DLL_FIRST (&pciIntList[irq]); pRtn != NULL;
	 pRtn = (PCI_INT_RTN *)DLL_NEXT (&pRtn->node))
	{
	if (pRtn->routine == routine)
	    {
	    oldLevel = intLock ();			/* LOCK INTERRUPT */
	    dllRemove (&pciIntList[irq], &pRtn->node);
	    intUnlock (oldLevel);			/* UNLOCK INTERRUPT */

	    free ((char *)pRtn);
	    return (OK);
	    }
	}

    return (ERROR);
    }

#endif /* defined(INCLUDE_PCI) */
