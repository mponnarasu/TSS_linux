/******************************************************************************
* Copyright 2017
* Author: Ponnarasu M.
* No guarantees, warrantees, or promises, implied or otherwise.
* May be used for any purpose provided copyright notice remains intact.
*
*****************************************************************************/
#ifndef RULE_EVAL_INCLUDED
#define RULE_EVAL_INCLUDED

/* Shared data to provide access to Intersection */
/* Range: 0-4 */
/* 0-Pedestrian ,1-Road1, 2-Road2, 3-Road3, 4-Road4, ... */
extern int IntersectionAccess;  

/* Read Write lock to protect shared variable for intersection Access */
/* 
 * Writer: TSS co-ordinator 
 * Readers: Multiple Displays
 */
extern pthread_rwlock_t displaylock;


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

void rule_evaluator(void);


#endif


