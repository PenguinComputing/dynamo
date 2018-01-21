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
#include "worker.h"

extern double mysecond();

struct history {
    double  target ;
    long  work ;
    double  actual ;
    double  log ;  /* work/actual */
};

#define HIST_SIZE  (20)
#define MIN_TGT (10E-6)
#define MAX_TGT (10E-3)

int
worker( struct task *task )
{
    double  target_time ;
    double  task_start, task_end ;
    long  work ;
    int  j ;

    struct history  history[HIST_SIZE];
    size_t  hcnt ;

    /* Run the task a few times with different work values to
     *    populate the history array
     */
    work = 10000 ;  /* A guess */
    target_time = 0 ;

    for( hcnt = 0 ; hcnt < HIST_SIZE ; ++hcnt ) {

        /* Time the task */
        task_start = mysecond();
        task->Task(work);
        task_end = mysecond();

        /* Report the time */
        task->ReportTime( target_time, work, task_end - task_start );

        /* Save the result */
        history[hcnt].target = target_time ;
        history[hcnt].work = work ;
        target_time = history[hcnt].actual = task_end - task_start ;
        if( target_time > 0 )
            history[hcnt].log = log( work/target_time );

        if( target_time <= 0 ) {
            /* Whoops, probably way to little work */
            fprintf( stderr, "Task completed too quickly (%10.9f sec for %ld work)\n",
                        target_time, work );
            target_time = MIN_TGT ;
            work += work ;  /* Exponential increase */
            --hcnt ;  /* Overwrite this on the next pass */
        } else if( target_time > MAX_TGT ) {
            /* Whoops, probably way to long */
            fprintf( stderr, "Task took too long (%10.6f sec for %ld work)\n",
                        target_time, history[hcnt].work );
            work = work * MAX_TGT / target_time ;
            if( work < 10 ) work = 10 ;
            target_time = MAX_TGT ;
            --hcnt ;  /* Overwrite this on the next pass */
            /* NOTE: This can cause an infinite loop if the Task selected
             *    can't complete in less than MAX_TGT time with work = 10
             */
        } else if( target_time < MIN_TGT ) {
            /* Hmm..  Too short */
            work = work * MIN_TGT / target_time ;
            target_time = MIN_TGT ;
        } else {
            if( MAX_TGT / target_time > target_time / MIN_TGT ) {
                work = work * sqrt(MAX_TGT/target_time);
                target_time = target_time * sqrt(MAX_TGT/target_time);
            } else {
                work = work / sqrt(target_time/MIN_TGT);
                target_time = target_time / sqrt(target_time/MIN_TGT);
            }
        };
        if( work < 10 ) work = 10 ;
    };

    for( j = 1 ; j < opt_loops ; ++j ) {
        double  log_work ;
        size_t  cnt_work ;

        /* Wait for work to do */
        target_time = task->Wait2Start() ;
#if 0
        printf( "Target Time: %10.9f msec, ", target_time *1E3 );
#endif

        /* Use history to find good work estimate for the target_time
         *   while making room for the next entry.
         */
	log_work = history[0].log ;
        for( cnt_work = 1 ; cnt_work < hcnt ; ++cnt_work ) {
            log_work += history[cnt_work].log ;
            if( hcnt >= HIST_SIZE ) {
                /* Make room */
                history[cnt_work-1] = history[cnt_work] ;
            };
        };
        if( hcnt >= HIST_SIZE ) { --hcnt; };

        /* Geometric mean of work history */
        work = exp(log_work/cnt_work) * target_time ;
#if 0
printf(" log_work: %f, cnt_work: %ld, work: %ld\n", log_work, cnt_work, work);
#endif

        /* Perform and Time the task */
        task_start = mysecond();
        task->Task(work);
        task_end = mysecond();

        /* Report the time */
        task->ReportTime( target_time, work, task_end - task_start );

	if( hcnt < HIST_SIZE ) {
            register double actual ;
            /* Add to the end */
            history[hcnt].target = target_time ;
            history[hcnt].work = work ;
            history[hcnt].actual = actual = task_end - task_start ;
            history[hcnt].log = log(work/actual) ;
            ++hcnt ;
        };

    } /* while */

    return 0 ;
};
