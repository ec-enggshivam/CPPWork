/* pciIomapShow.c - Show routines of PCI bus(IO mapped) library */

/*
 * Copyright (c) 1984-2005 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
01d,27sep05,h_k  cleaned up Diab compiler warning. (SPR #112110)
01c,20dec04,rec  correct more compiler warnings
01b,16aug04,scm  correct diab warnings...
01a,13jan00,pr   Adapted from cma220.
*/


/*
DESCRIPTION

This module contains show routine to see all devices and bridges on the PCI bus.
This module works in conjunction with pciIomapLib.o.
There are two ways to find out an empty device.
  - check Master Abort bit after the access.
  - check whether the read value is 0xffff.
It uses the second method, since I didn't see the Master Abort bit of
the host/PCI bridge changing.

*/

#if (defined(INCLUDE_PCI) && defined(INCLUDE_SHOW_ROUTINES))

#include "vxWorks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dllLib.h>
/*
 * We currently wish to include this from locally rather than from there as that
 * version is out-dated. However, as it has been included in sysLib.c already,
 * this will do no harm.
 */
#include "pciIomapLib.h"


/* defines */


/* externs */

IMPORT int pciLibInitStatus;
IMPORT int pciConfigMech;


/* globals */


/* locals */


/* forward declarations */

LOCAL void pciDheaderPrint	(PCI_HEADER_DEVICE * pD);
LOCAL void pciBheaderPrint	(PCI_HEADER_BRIDGE * pB);


/*******************************************************************************
*
* pciIomapShowInit - initialize the show routines.
*
* This routine is used to pull in all routines in this library.
*
* NOMANUAL
* 
* RETURNS: N/A
*/

void pciIomapShowInit (void)
    {
    }

/*******************************************************************************
*
* pciDeviceShow - print information about PCI devices
*
* This routine prints information about PCI devices
* There are two ways to find out an empty device.
*   - check Master Abort bit after the access.
*   - check whether the read value is 0xffff.
* It uses the second method, since I didn't see the Master Abort bit of
* the host/PCI bridge changing.
*
* RETURNS:
* OK, or ERROR if the library is not initialized.
*/

STATUS pciDeviceShow(  int	busNo		/* bus number */)
{
	  int deviceNo;
	  int devices;
	  ushort_t vendorId;
	  ushort_t deviceId;
	  union
	{
		int classCode;
		char array[4];
	} u;

	  if (pciLibInitStatus != OK)			/* sanity check */
	      return (ERROR);

	  printf ("Scanning function 0 of each PCI device on bus %d\n", busNo);
	  printf ("Using configuration mechanism %d\n", pciConfigMech);
	  printf ("bus       device    function  vendorID  deviceID  class\n");

	  if (pciConfigMech == PCI_MECHANISM_1)
			 devices = 0x1f;
	  else
		devices = 0x0f;

	  if (busNo != 0)
	{
	if (devices > 0x0f)
	    devices = 0x0f;
	}

	  if (devices > PCI_MAX_DEV)
	devices = PCI_MAX_DEV;

	  for (deviceNo=0; deviceNo < devices; deviceNo++)
	{
	pciConfigInWord (busNo, deviceNo, 0, PCI_CFG_VENDOR_ID, (short *) &vendorId);
	pciConfigInWord (busNo, deviceNo, 0, PCI_CFG_DEVICE_ID, (short *) &deviceId);
	#if (_BYTE_ORDER == _BIG_ENDIAN)
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_PROGRAMMING_IF,
			 &u.array[3]);
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_SUBCLASS, &u.array[2]);
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_CLASS, &u.array[1]);
	u.array[0] = 0;
	#else
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_PROGRAMMING_IF,
			 &u.array[0]);
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_SUBCLASS, &u.array[1]);
	pciConfigInByte (busNo, deviceNo, 0, PCI_CFG_CLASS, &u.array[2]);
	u.array[3] = 0;
	#endif
	/*
	 * There are two ways to find out an empty device.
	 *   1. check Master Abort bit after the access.
	 *   2. check whether the read value is 0xffff.
	 * Since I didn't see the Master Abort bit of the host/PCI bridge
	 * changing, I use the second method.
	 */

	if (vendorId != 0xffff)
	    printf ("%.8x  %.8x  %.8x  %.8x  %.8x  %.8x\n",
		    busNo, deviceNo, 0, vendorId, deviceId, u.classCode);
	}

	  return (OK);
}

/*******************************************************************************
*
* pciHeaderShow - print a header of the specified PCI device
*
* This routine prints a header of the PCI device specified by busNo, deviceNo,
* and funcNo.
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciHeaderShow
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo		/* function number */
    )
    {
    PCI_HEADER_DEVICE headerDevice;
    PCI_HEADER_BRIDGE headerBridge;
    PCI_HEADER_DEVICE * pD = &headerDevice;
    PCI_HEADER_BRIDGE * pB = &headerBridge;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
		     &pD->headerType);

    if (pD->headerType & 0x01)		/* PCI-to-PCI bridge */
	{
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID, 
			 &pB->vendorId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID, 
			 &pB->deviceId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_COMMAND, 
			 &pB->command);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_STATUS, 
			 &pB->status);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_REVISION, 
			 &pB->revisionId);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_PROGRAMMING_IF, 
			 &pB->progIf);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SUBCLASS, 
			 &pB->subClass);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_CLASS, 
			 &pB->classCode);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_CACHE_LINE_SIZE, 
			 &pB->cacheLine);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_LATENCY_TIMER, 
			 &pB->latency);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
			 &pB->headerType);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_BIST, 
			 &pB->bist);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_0, 
			 &pB->base0);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_1, 
			 &pB->base1);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_PRIMARY_BUS, 
			 &pB->priBus);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SECONDARY_BUS, 
			 &pB->secBus);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SUBORDINATE_BUS, 
			 &pB->subBus);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SEC_LATENCY, 
			 &pB->secLatency);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_IO_BASE, 
			 &pB->ioBase);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_IO_LIMIT, 
			 &pB->ioLimit);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_SEC_STATUS, 
			 &pB->secStatus);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_MEM_BASE, 
			 &pB->memBase);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_MEM_LIMIT, 
			 &pB->memLimit);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_PRE_MEM_BASE, 
			 &pB->preBase);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_PRE_MEM_LIMIT, 
			 &pB->preLimit);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_PRE_MEM_BASE_U, 
			 &pB->preBaseUpper);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_PRE_MEM_LIMIT_U, 
			 &pB->preLimitUpper);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_IO_BASE_U, 
			 &pB->ioBaseUpper);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_IO_LIMIT_U, 
			 &pB->ioLimitUpper);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_ROM_BASE, 
			 &pB->romBase);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_BRG_INT_LINE, 
			 &pB->intLine);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_BRG_INT_PIN, 
			 &pB->intPin);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_BRIDGE_CONTROL, 
			 &pB->control);
        pciBheaderPrint (pB);
	}
    else					/* PCI device */
	{
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID, 
			 &pD->vendorId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID, 
			 &pD->deviceId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_COMMAND, 
			 &pD->command);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_STATUS, 
			 &pD->status);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_REVISION, 
			 &pD->revisionId);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_PROGRAMMING_IF, 
			 &pD->progIf);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_SUBCLASS, 
			 &pD->subClass);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_CLASS, 
			 &pD->classCode);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_CACHE_LINE_SIZE, 
			 &pD->cacheLine);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_LATENCY_TIMER, 
			 &pD->latency);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
			 &pD->headerType);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_BIST, 
			 &pD->bist);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_0, 
			 &pD->base0);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_1, 
			 &pD->base1);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_2, 
			 &pD->base2);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_3, 
			 &pD->base3);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_4, 
			 &pD->base4);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_BASE_ADDRESS_5, 
			 &pD->base5);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_CIS, 
			 &pD->cis);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_SUB_VENDER_ID, 
			 &pD->subVendorId);
        pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_SUB_SYSTEM_ID, 
			 &pD->subSystemId);
        pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_EXPANSION_ROM, 
			 &pD->romBase);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_DEV_INT_LINE, 
			 &pD->intLine);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_DEV_INT_PIN, 
			 &pD->intPin);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_MIN_GRANT, 
			 &pD->minGrant);
        pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_MAX_LATENCY, 
			 &pD->maxLatency);
        pciDheaderPrint (pD);
	}

    return (OK);
    }

/*******************************************************************************
*
* pciFindDeviceShow - find a device by deviceId, then print information.
*
* This routine finds a device by deviceId, then print information.
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciFindDeviceShow
    (
    int	vendorId,	/* vendor ID */
    int	deviceId,	/* device ID */
    int	index		/* desired instance of device */
    )
    {
    int busNo;
    int deviceNo;
    int funcNo;

    if (pciFindDevice (vendorId, deviceId, index, &busNo, &deviceNo, &funcNo)
	== OK)
	{
	printf ("deviceId = 0x%.8x\n", deviceId);
	printf ("vendorId = 0x%.8x\n", vendorId);
	printf ("index =    0x%.8x\n", index);
	printf ("busNo =    0x%.8x\n", busNo);
	printf ("deviceNo = 0x%.8x\n", deviceNo);
	printf ("funcNo =   0x%.8x\n", funcNo);
	return (OK);
	}
    return (ERROR);
    }

/*******************************************************************************
*
* pciFindClassShow - find a device by 24-bit class code
*
* This routine finds a device by its 24-bit PCI class code, then prints its
* information.
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciFindClassShow
    (
    int	classCode,	/* 24-bit class code */
    int	index		/* desired instance of device */
    )
    {
    int busNo;
    int deviceNo;
    int funcNo;

    if (pciFindClass (classCode, index, &busNo, &deviceNo, &funcNo) == OK)
	{
	printf ("class code = 0x%.8x\n", classCode);
	printf ("index =      0x%.8x\n", index);
	printf ("busNo =      0x%.8x\n", busNo);
	printf ("deviceNo =   0x%.8x\n", deviceNo);
	printf ("funcNo =     0x%.8x\n", funcNo);
	return (OK);
	}
    return (ERROR);
    }

/*******************************************************************************
*
* pciDheaderPrint - print a PCI device header
*
* This routine prints a PCI device header.
*
* RETURNS: N/A
*
*/

LOCAL void pciDheaderPrint
    (
    PCI_HEADER_DEVICE * pD
    )
    {
    printf ("vendor ID =                   0x%.4x\n", (ushort_t)pD->vendorId);
    printf ("device ID =                   0x%.4x\n", (ushort_t)pD->deviceId);
    printf ("command register =            0x%.4x\n", (ushort_t)pD->command);
    printf ("status register =             0x%.4x\n", (ushort_t)pD->status);	
    printf ("revision ID =                 0x%.2x\n", (uchar_t)pD->revisionId);
    printf ("class code =                  0x%.2x\n", (uchar_t)pD->classCode);	
    printf ("sub class code =              0x%.2x\n", (uchar_t)pD->subClass);
    printf ("programming interface =       0x%.2x\n", (uchar_t)pD->progIf);	
    printf ("cache line =                  0x%.2x\n", (uchar_t)pD->cacheLine);
    printf ("latency time =                0x%.2x\n", (uchar_t)pD->latency);
    printf ("header type =                 0x%.2x\n", (uchar_t)pD->headerType);
    printf ("BIST =                        0x%.2x\n", (uchar_t)pD->bist);	
    printf ("base address 0 =              0x%.8x\n", pD->base0);	
    printf ("base address 1 =              0x%.8x\n", pD->base1);	
    printf ("base address 2 =              0x%.8x\n", pD->base2);	
    printf ("base address 3 =              0x%.8x\n", pD->base3);	
    printf ("base address 4 =              0x%.8x\n", pD->base4);	
    printf ("base address 5 =              0x%.8x\n", pD->base5);	
    printf ("cardBus CIS pointer =         0x%.8x\n", pD->cis);	
    printf ("sub system vendor ID =        0x%.4x\n", (ushort_t)pD->subVendorId);
    printf ("sub system ID =               0x%.4x\n", (ushort_t)pD->subSystemId);
    printf ("expansion ROM base address =  0x%.8x\n", pD->romBase);
    printf ("interrupt line =              0x%.2x\n", (uchar_t)pD->intLine);
    printf ("interrupt pin =               0x%.2x\n", (uchar_t)pD->intPin);	
    printf ("min Grant =                   0x%.2x\n", (uchar_t)pD->minGrant);
    printf ("max Latency =                 0x%.2x\n", (uchar_t)pD->maxLatency);
    }

/*******************************************************************************
*
* pciBheaderPrint - print a PCI-to-PCI bridge header
*
* This routine prints a PCI-to-PCI bridge header.
*
* RETURNS: N/A
*
*/

LOCAL void pciBheaderPrint
    (
    PCI_HEADER_BRIDGE * pB
    )
    {
    printf ("vendor ID =                   0x%.4x\n", (ushort_t)pB->vendorId);
    printf ("device ID =                   0x%.4x\n", (ushort_t)pB->deviceId);
    printf ("command register =            0x%.4x\n", (ushort_t)pB->command);
    printf ("status register =             0x%.4x\n", (ushort_t)pB->status);	
    printf ("revision ID =                 0x%.2x\n", (uchar_t)pB->revisionId);
    printf ("class code =                  0x%.2x\n", (uchar_t)pB->classCode);	
    printf ("sub class code =              0x%.2x\n", (uchar_t)pB->subClass);
    printf ("programming interface =       0x%.2x\n", (uchar_t)pB->progIf);	
    printf ("cache line =                  0x%.2x\n", (uchar_t)pB->cacheLine);
    printf ("latency time =                0x%.2x\n", (uchar_t)pB->latency);
    printf ("header type =                 0x%.2x\n", (uchar_t)pB->headerType);
    printf ("BIST =                        0x%.2x\n", (uchar_t)pB->bist);
    printf ("base address 0 =              0x%.8x\n", pB->base0);	
    printf ("base address 1 =              0x%.8x\n", pB->base1);	
    printf ("primary bus number =          0x%.2x\n", (uchar_t)pB->priBus);	
    printf ("secondary bus number =        0x%.2x\n", (uchar_t)pB->secBus);	
    printf ("subordinate bus number =      0x%.2x\n", (uchar_t)pB->subBus);	
    printf ("secondary latency timer =     0x%.2x\n", (uchar_t)pB->secLatency);
    printf ("IO base =                     0x%.2x\n", (uchar_t)pB->ioBase);
    printf ("IO limit =                    0x%.2x\n", (uchar_t)pB->ioLimit);
    printf ("secondary status =            0x%.4x\n", (ushort_t)pB->secStatus);
    printf ("memory base =                 0x%.4x\n", (ushort_t)pB->memBase);
    printf ("memory limit =                0x%.4x\n", (ushort_t)pB->memLimit);	
    printf ("prefetch memory base =        0x%.4x\n", (ushort_t)pB->preBase);
    printf ("prefetch memory limit =       0x%.4x\n", (ushort_t)pB->preLimit);
    printf ("prefetch memory base upper =  0x%.8x\n", pB->preBaseUpper);
    printf ("prefetch memory limit upper = 0x%.8x\n", pB->preLimitUpper);
    printf ("IO base upper 16 bits =       0x%.4x\n", (ushort_t)pB->ioBaseUpper);
    printf ("IO limit upper 16 bits =      0x%.4x\n", (ushort_t)pB->ioLimitUpper);
    printf ("expansion ROM base address =  0x%.8x\n", pB->romBase);
    printf ("interrupt line =              0x%.2x\n", (uchar_t)pB->intLine);
    printf ("interrupt pin =               0x%.2x\n", (uchar_t)pB->intPin);	
    printf ("bridge control =              0x%.4x\n", (ushort_t)pB->control);
    }

#endif /* (defined(INCLUDE_PCI) && defined(INCLUDE_SHOW_ROUTINES)) */
