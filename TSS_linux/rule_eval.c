/******************************************************************************
* Copyright 2017
* Author: Ponnarasu M.
* No guarantees, warrantees, or promises, implied or otherwise.
* May be used for any purpose provided copyright notice remains intact.
*
*****************************************************************************/

#include <stdio.h>   // for printf
#include <unistd.h>  // for pause
#include <pthread.h> // thread: create, locks
#include <signal.h>  // to handle sigusr2
#include "timer.h"   // Public API for Timer Subsystem
#include "display.h" // Public API and global data for Display
#include "tss_config.h" // System configuration

/* Shared data to provide access to Intersection */
/* Range: 0-4 */
/* 0-Pedestrian ,1-Road1, 2-Road2, 3-Road3, 4-Road4 */
int IntersectionAccess = 0;  

/* Read Write lock to protect shared variable for intersection Access */
/* 
 * Writer: TSS co-ordinator 
 * Readers: Multiple Displays
 */
pthread_rwlock_t displaylock = PTHREAD_RWLOCK_INITIALIZER;

/* Main functions:
 * Responsibility: Setup the system and initialize the timer
 * go idle when no activity
 */

/******************************************************************************
*
* Function Name: void Rule_eval_SigHandleCA( int sig, siginfo_t *sigInf, void *context)
*
* Description:
* This function is a signal handler which updates 
* shared variable used for signalling when critical signal is passed
*
* Parameters:
* argc: Number of arguments (should be 3)
* argv: two dimentional character array containing arguments
*
* Returns:
* int
*
*****************************************************************************/

void Rule_eval_SigHandleCA( int sig, siginfo_t *sigInf, void *context)
{

  pthread_rwlock_wrlock(&displaylock);
    IntersectionAccess = sigInf->si_value.sival_int;
  pthread_rwlock_unlock(&displaylock);
  Display_module();

}


/******************************************************************************
*
* Function Name: void rule_evaluator( void )
*
* Description:
* This function evaluated the rule for traffic signalling. 
* currently only round robin based signalling is used.
*
* Parameters: void
*
* Returns: void
*
*****************************************************************************/

void rule_evaluator(void)
{

  if(IntersectionAccess == 0)
  {
    printf("\n Intersection Access to pedestrians\n");
  }
  else
  {
    printf("\n Intersection Access to Road%d\n",IntersectionAccess);
  }
  /* Update the Display */
  Display_module();

  /* update the next road to access intersection */
  pthread_rwlock_wrlock(&displaylock);
    IntersectionAccess++;
    if (IntersectionAccess > NUM_ROAD_INTERSECTION) IntersectionAccess = 0;
  pthread_rwlock_unlock(&displaylock);

}



/******************************************************************************
*
* Function Name: int main(int argc, char**argv)
*
* Description:
* This is the main thread of Traffic Signaling System
* It sets up the timer submodule and Traffic system co-ordinator
* It also set the critical signal handling functionality.
*
* Parameters: none
*
* Returns:
* int
*
*****************************************************************************/

int main(void)
{
  
  struct sigaction Sact;

  /* Setup signal for critical access */
  Sact.sa_flags = (SA_RESTART | SA_SIGINFO);

  Sact.sa_sigaction = Rule_eval_SigHandleCA;

  sigaction(SIGUSR1, &Sact, NULL);

  /* Initialize interval timer for the configured timeout duartion */
  if (timer_setup(TIMEOUT))
  {
    printf("\n timer error\n");
    return (1);
  }

  while (1)
  {
    printf("\npress ctl-c to quit.\n");
    pause();
  }
  /* unreachable function, but added to avoid undesired interrupts */
  timer_stop();

  return (0);
}




