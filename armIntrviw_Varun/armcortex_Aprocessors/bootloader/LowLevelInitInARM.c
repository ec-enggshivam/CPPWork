#include <stdint.h> /* C-99 standard exact-width integer types */

void low_level_init(void (*reset_addr)(), void (*return_addr)())
{
  extern uint8_t __ram_start;
  static uint32_t const LDR_PC_PC = 0xE59FF000U;
  static uint32_t const MAGIC = 0xDEADBEEFU;
  AT91PS_PMC pPMC;
  /* Set flash wait sate FWS and FMCN */
  AT91C_BASE_MC->MC_FMR = ( (AT91C_MC_FMCN) & ( (MCK + 500000) / 1000000 << 16))
                                                          | AT91C_MC_FWS_1FWS;
  AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS; /* Disable the watchdog */
  /* Enable the Main Oscillator */. . .
  /* Set the PLL and Divider and wait for PLL stabilization */. . .
  /* Select Master Clock and CPU Clock select the PLL clock / 2 */. . .
  /* Setup the exception vectors in RAM.
   * NOTE: the exception vectors must be in RAM *before* the remap
   * in order to guarantee that the ARM core is provided with valid vectors
   * during the remap operation.
   */
  /* setup the primary vector table in RAM */
  *(uint32_t volatile *)(&__ram_start + 0x00)  = (LDR_PC_PC | 0x18);
  *(uint32_t volatile *) (&__ram_start + 0x04) = (LDR_PC_PC | 0x18);
  *(uint32_t volatile *) (&__ram_start + 0x08) = (LDR_PC_PC | 0x18);
  *(uint32_t volatile *) (&__ram_start + 0x0C) = (LDR_PC_PC | 0x18);
  *(uint32_t volatile *) (&__ram_start + 0x10) = (LDR_PC_PC | 0x18);
  *(uint32_t volatile *) (&__ram_start + 0x14) = MAGIC;
  *(uint32_t volatile *) (&__ram_start + 0x18) = (LDR_PC_PC | 0x18);
  *(uint32_t volatile *) (&__ram_start + 0x1C) = (LDR_PC_PC | 0x18);
  /* setup the secondary vector table in RAM */
  * (uint32_t volatile *) (&__ram_start + 0x20) = (uint32_t) reset_addr;
  *(uint32_t volatile *) (&__ram_start + 0x24) = 0x04U;
  *(uint32_t volatile *) (&__ram_start + 0x28) = 0x08U;
  *(uint32_t volatile *) (&__ram_start + 0x2C) = 0x0CU;
  *(uint32_t volatile *) (&__ram_start + 0x30) = 0x10U;
  *(uint32_t volatile *) (&__ram_start + 0x34) = 0x14U;
  *(uint32_t volatile *) (&__ram_start + 0x38) = 0x18U;
  *(uint32_t volatile *) (&__ram_start + 0x3C) = 0x1CU;
  /* check if the Memory Controller has been remapped already */
  if (MAGIC != (*(uint32_t volatile *) 0x14))
  {
    AT91C_BASE_MC->MC_RCR = 1; /* perform Memory Controller remapping */
  }
}
