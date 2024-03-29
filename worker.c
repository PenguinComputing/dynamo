/* Copyright (C) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 */

/* Worker function
 *
 * 1. Get Work amount
 * 2. Call Work function
 * 3. Report actual time
 * 4. Wait for idle time
 * 5. Loop back to 1.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "global.h"
#include "control.h"
#include "worker.h"

extern double mysecond();

struct history {
    double  target ;
    long  work ;
    double  actual ;
    double  log ;  /* work/actual */
};

int
worker( struct task *task )
{
    double  task_start, task_end ;
    long  work ;
    int  j ;

    /* 1. Get Work amount */
    work = UpdateWork( -1.0 );

    for( j = 1 ; j < opt_loops ; ++j ) {
        /* Perform and Time the task */

        /* 2. Call Work function */
        task_start = mysecond();
        task->Task( work );
        task_end = mysecond();

        /* 3. Report actual time */
        task->ReportTime( opt_busy_sec, work, task_end - task_start );

        /* 4. Wait for idle time */
        /* 1. Get Work amount */
        work = task->Wait2Start( task_end - task_start ) ;
    };

    return 0 ;
};
