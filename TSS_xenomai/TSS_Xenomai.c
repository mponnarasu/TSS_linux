/******************************************************************************
* Copyright 2017
* Author: Ponnarasu M.
* No guarantees, warrantees, or promises, implied or otherwise.
* May be used for any purpose provided copyright notice remains intact.
*
*****************************************************************************/

#include <stdio.h>   // for printf
#include <unistd.h>  // for pause
#include <signal.h>  // to handle sigusr2
#include <pthread.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include "tss_config.h" // System configuration

RT_TASK Timer_task;
RT_TASK Display_task;

/* Shared data to provide access to Intersection */
/* Range: 0-4 */
/* 0-Pedestrian ,1-Road1, 2-Road2, 3-Road3, 4-Road4 */
int IntersectionAccess = 0;  

/* Read Write lock to protect shared variable for intersection Access */
/* 
 * Writer: TSS co-ordinator 
 * Readers: Multiple Displays
 */
pthread_rwlock_t displaylock;


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

}


/******************************************************************************
*
*******************************************************************************/
int  Road_ids[NUM_ROAD_INTERSECTION];

/******************************************************************************
*
* Function Name: void *VehicleDisplay(void *idp)
*
* Description:
* This is a thread function
* which is spawned one per road connected to intersection
*
* Parameters: *idp (road ID)
*
* Returns: void *
*
*****************************************************************************/
void *VehicleDisplay(void *idp) 
{
  int *my_id = idp;
  int local_IntersectionAccess;
  /* Display yellow before actuating the actual signal */
//  printf("Road%d : Yellow \n", *my_id);

  /* acquire read access */
  pthread_rwlock_rdlock(&displaylock);
  /* make a local copy */
  local_IntersectionAccess = IntersectionAccess;
  /* release read access */ 
  pthread_rwlock_unlock(&displaylock);

  /* check if the current road has access to intersection */
  if(local_IntersectionAccess == *my_id)
  {
    /* yes: set green light */
    printf("Road%d : Green \n", *my_id);
  }
  else
  { 
    /* No: set red light */
    printf("Road%d : Red \n", *my_id);
  }
  pthread_exit(NULL);
}

/******************************************************************************
*
* Function Name: void Display_subsystem( void)
*
* Description:
* This function is the Display subsystem function 
*
* Parameters: none
*
* Returns: void
*
*****************************************************************************/

void Display_module(void * arg)
{
  int i;
  pthread_t threads[NUM_ROAD_INTERSECTION];
  pthread_attr_t attr;

  /* For portability, explicitly create threads in a joinable state */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  
  /* for ever road leading to intersection create a seperate display thread */
  for (i = 0; i < NUM_ROAD_INTERSECTION; i++) 
  {
    Road_ids[i]=i+1;
    pthread_create(&threads[i], &attr, VehicleDisplay, (void *)&Road_ids[i]);
    pthread_join(threads[i], NULL);
  }

  /* Wait for all threads to complete */
  for (i = 0; i < NUM_ROAD_INTERSECTION; i++) 
  {
    pthread_join(threads[i], NULL);
  }
  /* Clean up and exit */
  pthread_attr_destroy(&attr);
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
   rt_task_create(&Display_task, "Display_Task", 0, 89, 0);
  /* Update the Display */
   rt_task_start(&Display_task, &Display_module, NULL);

   rt_task_join(&Display_task);
   rt_task_delete(&Display_task);
  /* update the next road to access intersection */
   pthread_rwlock_wrlock(&displaylock);
    /* ... Critical section ... */
    IntersectionAccess++;
    if (IntersectionAccess > NUM_ROAD_INTERSECTION) IntersectionAccess = 0;
   pthread_rwlock_unlock(&displaylock);

}


void Timer_module(void *arg)
{
        RTIME now, previous;

        /*
         * Arguments: &task (NULL=self),
         *            start time,
         *            period (here: 1 s)
         */
        rt_task_set_periodic(NULL, TM_NOW, TIMEOUT);
        previous = rt_timer_read();

        while (1) {
                rt_task_wait_period(NULL);
                now = rt_timer_read();
                rule_evaluator();

                /*
                 * NOTE: printf may have unexpected impact on the timing of
                 *       your program. It is used here in the critical loop
                 *       only for demonstration purposes.
                 */
                printf("Time since last turn: %ld.%06ld ms\n",
                       (long)(now - previous) / 1000000,
                       (long)(now - previous) % 1000000);
                       previous = now;
        }
}


/******************************************************************************
*
* Function Name: int main(int argc, char**argv)
*
* Description:
* This is the main thread of Traffic Signaling System
* It sets up tasks and activates timer task
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

  /* Avoids memory swapping for this program */
  mlockall(MCL_CURRENT|MCL_FUTURE);

  /* Create Tasks
   * Arguments: &task,
   *            name,
   *            stack size (0=default),
   *            priority,
   *            mode (FPU, start suspended, ...)
   */
   rt_task_create(&Timer_task, "Timer_Task", 0, 88, 0);

   /*
    * Arguments: &task,
    *            task function,
    *            function argument
    */
   rt_task_start(&Timer_task, &Timer_module, NULL);

        pause();

   rt_task_delete(&Timer_task);

  return (0);
}


