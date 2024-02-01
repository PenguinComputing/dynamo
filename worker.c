/* Copyright (C) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 */

/* Worker function
 *
 * 1. Call "Wait2Start"
 * 2. Calculate Work amount
 * 3. Call Work function
 * 4. Measure actual time
 * 5. Report time
 * 6. Loop back to 1.
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

    /* Get initial work value */
    work = UpdateWork( -1.0 );

    for( j = 1 ; j < opt_loops ; ++j ) {
        /* Perform and Time the task */
        task_start = mysecond();
        task->Task( work );
        task_end = mysecond();

        /* Report the time */
        task->ReportTime( opt_busy_sec, work, task_end - task_start );

        /* Wait for work to do */
        work = task->Wait2Start( task_end - task_start ) ;
    };

    return 0 ;
};
