#!/bin/sh
gcc rule_eval.c timer.c display.c -Wall -s -o2 -o TSS -lpthread
gcc ambulance.c -Wall -s -o2 -o ambulance
