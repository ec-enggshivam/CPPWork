/*******************************************************************************
**                                                                            **
**  FILENAME     : code_test.h                                                **
**                                                                            **
**  VERSION      : 1.0.0                                                      **
**                                                                            **
**  DATE         : 2017-01-22                                                 **
**                                                                            **
**  AUTHOR       : Varun Kumar                                                **
*******************************************************************************/
#ifndef CODE_TEST_H
#define CODE_TEST_H

/* type definitions */
typedef unsigned int uint32;


/* Defines to control scale of simulation */

#define CODE_TEST_NO_OF_STEPS             ((uint32)100U)

#define CODE_TEST_NO_OF_WORKER_SETS       ((uint32)3U)

#define CODE_TEST_NO_TICKS_JOB_COMPLETE   ((uint32)4U)



typedef enum
{
  COMP_NIL    = 0x0U,
  COMP_A      = 0x1U,
  COMP_B      = 0x2U,
  COMP_PRODCT = 0x3U
}Code_Test_CompType;

typedef enum
{
  WRKR_FREE          = 0x0U,
  WRKER_B_COMPA      = 0x1U,
  WRKER_B_COMPB      = 0x2U,
  WRKER_B_PROD_PROG  = 0x3U,
  WRKER_B_PROD_RDY   = 0x4U
}Code_Test_WorkerJobStatus;


typedef struct
{
  /*to store the current job status of the worker*/
  Code_Test_WorkerJobStatus JobState;
  /*to count number of ticks needed to make product*/
  uint32                    JobCycle; 
}Code_Test_WorkerType;

typedef struct
{
  /* Top worker on the Belt position */
  Code_Test_WorkerType WorkerTop;
  /* Bottom worker on the Belt position */
  Code_Test_WorkerType WorkerBottom;
  /* Current component on at the Belt position */
  Code_Test_CompType CurrentComp; 
}Code_Test_BeltPosType;
  
/* API prototype to run the simulation in application */
void Code_Test_RunSimulation(void);


#endif /* #ifndef CODE_TEST_H */

