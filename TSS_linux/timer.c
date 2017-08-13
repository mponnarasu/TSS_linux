/******************************************************************************
* Copyright 2017
* Author: Ponnarasu M.
* No guarantees, warrantees, or promises, implied or otherwise.
* May be used for any purpose provided copyright notice remains intact.
*
*****************************************************************************/
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include "timer.h"
#include "rule_eval.h"


struct itimerval timervalue;

struct sigaction new_handler, old_handler;

void timer_sig_handler(int signal)
{
  rule_evaluator();
 
}


int timer_setup(unsigned int milliSec)
{

  timervalue.it_interval.tv_sec = milliSec / 1000;
  timervalue.it_interval.tv_usec = (milliSec % 1000) * 1000;
  timervalue.it_value.tv_sec = milliSec / 1000;
  timervalue.it_value.tv_usec = (milliSec % 1000) * 1000;
  if(setitimer(ITIMER_REAL, &timervalue, NULL))
  {
    printf("\nsetitimer() error\n");
    return(1);
  }

  new_handler.sa_handler = &timer_sig_handler;
  new_handler.sa_flags = SA_NOMASK;
  if(sigaction(SIGALRM, &new_handler, &old_handler))
  {
    printf("\nsigaction() error\n");
    return(1);
  }

  return(0);
}


void timer_stop(void)
{
  timervalue.it_interval.tv_sec = 0;
  timervalue.it_interval.tv_usec = 0;
  timervalue.it_value.tv_sec = 0;
  timervalue.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &timervalue, NULL);

  sigaction(SIGALRM, &old_handler, NULL);
}
