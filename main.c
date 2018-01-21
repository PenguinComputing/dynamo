/* Copyright (c) 2017,2018 -- Penguin Computing
 *
 * All rights reserved, excepting to parts copied from public domain and
 *    open sources
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "global.h"
#include "worker.h"
#include "streamtask.h"
#include "control.h"

#ifdef MPI
#include "mpi.h"
#endif

extern double mysecond();

int parse_args( int argc, char ** argv ) {

   return 0 ;
}

int usage( int rc ) {
   printf( "usage: %s\n", ARGV0 );

   exit( rc );
}


#define M (20)
double
checktick()
{
    int         i;
    double      minDelta, Delta ;
    double      t1, t2, timesfound[M];

/*  Collect a sequence of M unique time values from the system. */

    for (i = 0; i < M; i++) {
        t1 = mysecond();
        while( ((t2=mysecond()) - t1) <= 0 )
            ;
#if 0
        printf( "Tick: %20.9f\n", t2 - t1);
#endif
        timesfound[i] = t1 = t2;
    }

    /*
     * Determine the minimum difference between these M values.
     * This result will be our estimate (in microseconds) for the
     * clock granularity.
     */

    minDelta =  1.0E6 * (timesfound[1]-timesfound[0]);
    for (i = 2; i < M; i++) {
	Delta =  1.0E6 * (timesfound[i]-timesfound[i-1]);
	minDelta = MIN(minDelta, MAX(Delta,0));
    }

    return(minDelta);
}

int main( int argc, char ** argv ) {

    /* Parse arguments, print usage if parse fails or usage requested */
    if( parse_args( argc, argv ) || opt_usage ) {
        usage( !opt_usage );
    }

    printf( "timeNow:  %20.9f\n", mysecond() );

    printf( "minDelta: %20.9f micro-seconds\n", checktick() );

    printf( "OMP_NUM_THREADS: %s\n", getenv("OMP_NUM_THREADS") );

#ifdef MPI
    StartMPI( argc, argv );

    printf( "task -> MPIWait, PrintReport, %s\n", opt_task->Desc );
    opt_task->Wait2Start = MPIWait ;
    opt_task->ReportTime = PrintReport ;
#else
    printf( "task -> StaticWait, PrintReport, %s\n", opt_task->Desc );
    opt_task->Wait2Start = StaticWait ;
    opt_task->ReportTime = PrintReport ;
#endif

    printf( "Init(%ld) ...\n", (long) opt_init_size );
    if( !opt_task->Init( opt_init_size ) ) {
        fprintf( stderr, "%s: Init() failed: %s\n", ARGV0, strerror(errno) );
        return 1 ;
    }

    printf( "worker()...\n" );
    worker( opt_task );

#ifdef MPI
    StopMPI( );
#endif

    return( 0 );
}
