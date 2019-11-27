/*******************************************************************************
**                                                                            **
**  FILENAME     : Code_test.c                                                **
**                                                                            **
**  VERSION      : 1.0.0                                                      **
**                                                                            **
**  DATE         : 2019-11-26                                                 **
**                                                                            **
**  AUTHOR       : Shivam Shukla                                                **
*******************************************************************************/

/*
Problem Description:
=========
Question:
=========
There is a factory production line around a single a conveyor belt.
Components (of type A and B) come onto the start of the belt at random intervals;
workers must take one component of each type from the belt as they come past, 
and combine them to make a finished product (P).
The belt is divided into fixed-size slots; each slot can hold only one component
 or one finished product. There are a number of worker stations on either side 
 of the belt, spaced to match the size of the slots on the belt, like this 
 (fixed-width font ASCII pic):
       v   v   v   v   v          workers
     ---------------------
  -> | A |   | B | A | P | ->     conveyor belt
     ---------------------
       ^   ^   ^   ^   ^          workers
In each unit of time, the belt moves forwards one position, and there is time 
for a worker on one side of each slot to EITHER take an item from the slot or 
replace an item onto the belt. The worker opposite them can't touch the same belt
 slot while they do this.(So you can't have one worker picking something from a 
 slot while their counterpart puts something down in the same place).
Once a worker has collected one of both types of component, they can begin 
assembling the finished product. This takes an amount of time, so they will only
 be ready to place the assembled product back on the belt on the fourth subsequent
  slot. While they are assembling the product, they can't touch the conveyor belt.
   Workers can only hold two items (component or product) at a time: one in each
    hand.
Create a simulation of this, with three pairs of workers. At each time interval,
 the slot at the start of the conveyor belt should have an equal (1/3) chance of
  containing nothing, a component A or a component B.
Run the simulation for 100 steps, and compute how many finished products come off
 the production line, and how many components of each type go through the production
  line without being picked up by any workers.
A few pointers:
Your implementation can be in a language of your choosing, but should be ready 
to compile/run.
The code does not have to be 'production quality', but we will be looking for 
evidence that it's written to be somewhat flexible, and that a third party would
 be able to read and maintain it.
Be sure to state (or comment) your assumptions.
During the interview, we may ask about the effect of changing certain aspects of
 the simulation. (E.g. the length of the conveyor belt.)
Flexibility in the solution is preferred, but we are also looking for a sensible
decision on where this adds too much complexity (where would it be better to 
rewrite the code for a different scenario, rather than spending much more than 
the allotted time creating an overly complicated, but very flexible simulation 
engine?)

*/

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "code_test.h"

#define CODE_TEST_NXT_NUM_ERR                 ((uint32)255U)
#define CODE_TEST_RDM_GEN_PARAM1              ((uint32)0xDEECE66DUL)
#define CODE_TEST_RDM_GEN_PARAM2              ((uint32)0xBUL)

/*variable to store possible component types - can go to flash */
static Code_Test_CompType Code_Test_CompOpt[3] = {COMP_NIL, COMP_A, COMP_B};

/* Variable to store Current Belt status - raminit area */
static Code_Test_BeltPosType Code_Test_Belt[CODE_TEST_NO_OF_WORKER_SETS];
/*variable to store current seed value for random generation*/
static uint32  Code_Test_Seed;

/*Global variable to store the total number of products as outcome of simulation*/
uint32 Code_Test_TotalProd =0U;
/*
Local function declarations
*/
static inline uint32 Code_Test_NextNum(uint32 num);
static inline uint32 Code_Test_NextSeed(uint32 BitWidth);
static uint32 Code_Test_ComplCompCheck(uint32 currPos, \
                                       Code_Test_CompType currCompType);
static void Code_Test_Main_Function(void);


/* API to run the simulation in application */
void Code_Test_RunSimulation(void)
{
  uint32 fLoop;

  for(fLoop=0U; fLoop < CODE_TEST_NO_OF_STEPS; fLoop++)
  {
    Code_Test_Main_Function();
  }

}


/* Schedular main function for Belt and worker state management */
static void Code_Test_Main_Function(void)
{
	
  uint32 index, fLoop;
  Code_Test_CompType newComp;
  
  /* Find out the new component at the beginning of this cycle - tick (Time T)*/
  index   = Code_Test_NextNum(3U);
  newComp = Code_Test_CompOpt[index];
  
  for(fLoop=0U; fLoop < CODE_TEST_NO_OF_WORKER_SETS; fLoop++)
  {
    
    /* Moving the Belt Forward at each tick */
    if(fLoop == 0U)
    {
      if(COMP_NIL != newComp)
      {
        /*Place new component at start of the belt*/
        Code_Test_Belt[fLoop].CurrentComp = newComp;
      }
      else
      {
        if(WRKER_B_PROD_RDY == Code_Test_Belt[fLoop].WorkerTop.JobState )
        {
          Code_Test_Belt[fLoop].CurrentComp = COMP_PRODCT;
          Code_Test_Belt[fLoop].WorkerTop.JobState = WRKR_FREE;
        }
        else if(WRKER_B_PROD_RDY == Code_Test_Belt[fLoop].WorkerBottom.JobState)
        {
          Code_Test_Belt[fLoop].CurrentComp = COMP_PRODCT;
          Code_Test_Belt[fLoop].WorkerBottom.JobState = WRKR_FREE;
        }
        else
        {
          Code_Test_Belt[fLoop].CurrentComp = newComp;
        }
      }
      
    }
    else
    {
      if(COMP_NIL != Code_Test_Belt[(fLoop-1U)].CurrentComp)
      {
        /*Place new component at start of the belt*/
        Code_Test_Belt[fLoop].CurrentComp = Code_Test_Belt[(fLoop-1U)].CurrentComp;
      }
      else
      {
        if(WRKER_B_PROD_RDY == Code_Test_Belt[fLoop].WorkerTop.JobState )
        {
          Code_Test_Belt[fLoop].CurrentComp = COMP_PRODCT;
          Code_Test_Belt[fLoop].WorkerTop.JobState = WRKR_FREE;
        }
        else if(WRKER_B_PROD_RDY == Code_Test_Belt[fLoop].WorkerBottom.JobState)
        {
          Code_Test_Belt[fLoop].CurrentComp = COMP_PRODCT;
          Code_Test_Belt[fLoop].WorkerBottom.JobState = WRKR_FREE;
        }
        else
        {
          Code_Test_Belt[fLoop].CurrentComp = Code_Test_Belt[(fLoop-1U)].CurrentComp;;
        }
      }
      
    }
    
    /* Assignment of the Job */
    
    /*
    1. Check if no workders on to the right of current position of the blet 
       slots are waiting for the current component as 'complimentory' to
       complete their job
    2. If No, check if bottom worker of current slot  is waiting for same, 
        then check if Top workder is waiting for it.
    3. If No, check if top worker is free then allocate it to it, otherwise
       allocate it to bottom workder if free.
    
    */
    
    if((COMP_A == Code_Test_Belt[fLoop].CurrentComp ) || \
       (COMP_B ==  Code_Test_Belt[fLoop].CurrentComp))
    {
      if(0U == Code_Test_ComplCompCheck(fLoop,Code_Test_Belt[fLoop].CurrentComp))
      {
        if (COMP_A == Code_Test_Belt[fLoop].CurrentComp)
        {
          if(Code_Test_Belt[fLoop].WorkerBottom.JobState == WRKER_B_COMPB)
          {
            Code_Test_Belt[fLoop].WorkerBottom.JobState = WRKER_B_PROD_PROG;
          }
          else
          {
            if(Code_Test_Belt[fLoop].WorkerTop.JobState == WRKER_B_COMPB)
            {
              Code_Test_Belt[fLoop].WorkerTop.JobState = WRKER_B_PROD_PROG;
            }
            else if ( Code_Test_Belt[fLoop].WorkerTop.JobState == WRKR_FREE)
            {
              Code_Test_Belt[fLoop].WorkerTop.JobState = WRKER_B_COMPA;
            }
            else if ( Code_Test_Belt[fLoop].WorkerBottom.JobState == WRKR_FREE)
            {
              Code_Test_Belt[fLoop].WorkerTop.JobState = WRKER_B_COMPA;
            }
          }
          
        }
        else
        {
          if(Code_Test_Belt[fLoop].WorkerBottom.JobState == WRKER_B_COMPA)
          {
            Code_Test_Belt[fLoop].WorkerBottom.JobState = WRKER_B_PROD_PROG;
          }
          else
          {
            if(Code_Test_Belt[fLoop].WorkerTop.JobState == WRKER_B_COMPA)
            {
              Code_Test_Belt[fLoop].WorkerTop.JobState = WRKER_B_PROD_PROG;
            }
            else if ( Code_Test_Belt[fLoop].WorkerTop.JobState == WRKR_FREE)
            {
              Code_Test_Belt[fLoop].WorkerTop.JobState = WRKER_B_COMPB;
            }
            else if ( Code_Test_Belt[fLoop].WorkerBottom.JobState == WRKR_FREE)
            {
              Code_Test_Belt[fLoop].WorkerTop.JobState = WRKER_B_COMPB;
            }
          }
        }
      }
    }
    
    
    /* Increment the worker counts if job in progress */
    
    if( Code_Test_Belt[fLoop].WorkerTop.JobState == WRKER_B_PROD_PROG)
    {
      Code_Test_Belt[fLoop].WorkerTop.JobCycle++;
      if(Code_Test_Belt[fLoop].WorkerTop.JobCycle > \
                    CODE_TEST_NO_TICKS_JOB_COMPLETE)
      {
        Code_Test_Belt[fLoop].WorkerTop.JobState = WRKER_B_PROD_RDY;
        Code_Test_Belt[fLoop].WorkerTop.JobCycle = 0U;
      }
    }
    
    if( Code_Test_Belt[fLoop].WorkerBottom.JobState == WRKER_B_PROD_PROG)
    {
      Code_Test_Belt[fLoop].WorkerBottom.JobCycle++;
      if(Code_Test_Belt[fLoop].WorkerBottom.JobCycle > \
                    CODE_TEST_NO_TICKS_JOB_COMPLETE)
      {
        Code_Test_Belt[fLoop].WorkerBottom.JobState = WRKER_B_PROD_RDY;
        Code_Test_Belt[fLoop].WorkerBottom.JobCycle = 0U;
      }
      
    }
    
    
    /*count the fruits - total number of products at end of belt*/
    if(fLoop == (CODE_TEST_NO_OF_WORKER_SETS-1U))
    {
      if( Code_Test_Belt[fLoop].CurrentComp == COMP_PRODCT)
      {
        Code_Test_TotalProd++;
      }
    }
    
  }
  
}


/* Local function to check if any worker on the right side of current belt
   position, waiting for new component as complementory component to 
   finish the job
*/
static uint32 Code_Test_ComplCompCheck(uint32 currPos,
			 																				Code_Test_CompType currCompType)
{
  uint32 fLoop;
  uint32 status =0U;
  
  for (fLoop=(currPos+1U); fLoop < CODE_TEST_NO_OF_WORKER_SETS; fLoop++)
  {
    if (COMP_A == currCompType)
    {
      if((Code_Test_Belt[fLoop].WorkerTop.JobState == WRKER_B_COMPB) || \
         (Code_Test_Belt[fLoop].WorkerBottom.JobState == WRKER_B_COMPB))
      {
        status = 1U;
        break;
      }
    }
    else
    {
      if((Code_Test_Belt[fLoop].WorkerTop.JobState == WRKER_B_COMPA) || \
         (Code_Test_Belt[fLoop].WorkerBottom.JobState == WRKER_B_COMPA))
      {
        status = 1U;
        break;
      }
    }
  }
  
  return status;
  
}




/* Local function to create a random number between 0 - 2 
   Taken from internet : Not my own implementation
*/
static inline uint32 Code_Test_NextNum(uint32 num)
{
   uint32 nextNum;
   
   uint32 bits;

   if ((num & (num-1U)) == 0U)
   {// i.e., n is a power of 2
     nextNum =  (uint32)((num * (long)Code_Test_NextSeed(31U)) >> 31U);
   
   }
   else
   {
     do 
     {
       bits = Code_Test_NextSeed(31);
       nextNum = bits % num;
     } while ((bits - nextNum + (num-1U)) < 0);
   }
   return nextNum;
}

/*  Function to aide the random function */
static inline uint32 Code_Test_NextSeed(uint32 BitWidth)
{
  uint32 nextSeed;
  
  Code_Test_Seed = (uint32)(((Code_Test_Seed * CODE_TEST_RDM_GEN_PARAM1) + \
                         CODE_TEST_RDM_GEN_PARAM2) & ((1UL << 31U) - 1UL));
  nextSeed = (uint32)(Code_Test_Seed >> (31U - BitWidth));
  return nextSeed;
}

