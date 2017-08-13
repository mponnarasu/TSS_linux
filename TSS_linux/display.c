/******************************************************************************
* Copyright 2017
* Author: Ponnarasu M.
* No guarantees, warrantees, or promises, implied or otherwise.
* May be used for any purpose provided copyright notice remains intact.
*
*****************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include "tss_config.h"
#include "display.h"
#include "rule_eval.h"

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
* Function Name: void Display_module( void)
*
* Description:
* This function is the Display module function 
*
* Parameters: none
*
* Returns: void
*
*****************************************************************************/

void Display_module(void)
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





