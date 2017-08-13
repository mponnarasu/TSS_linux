/******************************************************************************
* Copyright 2017
* Author: Ponnarasu M.
* No guarantees, warrantees, or promises, implied or otherwise.
* May be used for any purpose provided copyright notice remains intact.
*
*****************************************************************************/

#include <signal.h> // for sigqueue API
#include <stdio.h>  // for printf
#include <stdlib.h> // for atoi
#include "tss_config.h" // for NUM_ROAD_INTERSECTION

/******************************************************************************
*
* Function Name: int main(int argc, char**argv)
*
* Description:
* This function sends signal SIGUSR1 with a value to process 
* whose process ID is passed as argument 
* Usage: <executable name> <PID> <road num>
*
* Parameters:
* argc: Number of arguments (should be 3)
* argv: two dimentional character array containing arguments
*
* Returns:
* int
*
*****************************************************************************/
int main(int argc, char**argv)
{
  union sigval val; //Data type used to pass value via signal
  pid_t pid_val;
  /* Check for correct usage (number of arguments) */ 
  if (argc != 3)
  {
    printf("Usage ambulance <PID> <road num>\n");
    return (1);
  }
  else
  {
    /* Read the value into DataStructure */ 
    val.sival_int = atoi(argv[2]);
    /* Check the data passed is in correct range */
    if((val.sival_int > 0 )&&(val.sival_int <= NUM_ROAD_INTERSECTION))
    { 
      /* read the PID value */
      pid_val = (pid_t)atoi(argv[1]);
      printf("Ensure you have passed the correct process ID\n");
      /* Send Signal to process ID */
      sigqueue(pid_val, SIGUSR1, val);
      printf("Signal sent to process with ID %d \n",pid_val);
    }
    else
    {
      printf("Road number is wrong \n");
      return (1);
    }
  }
  return (0);
}
