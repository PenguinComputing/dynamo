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
    int option ;
    int retval = 0 ; /* Assume success */

    ARGV0 = argv[0] ;

    while( (option = getopt( argc, argv, "uhdA:i:b:l:T:R:W:" )) > 0) {
        switch( option ) {
        case 'A' : /* Task Init value aka Array size */
            opt_init_size = strtol(optarg, NULL, 0);
            if( opt_debug )
                printf( "opt_init_size = %ld\n", opt_init_size );
            break ;
        case 'i' : /* Idle time */
            opt_idle_sec = strtod( optarg, NULL );
            if( opt_debug )
                printf( "opt_idle_sec = %.6f msec\n", opt_idle_sec * 1E3 );
            break ;
        case 'b' : /* Busy time */
            opt_busy_sec = strtod( optarg, NULL );
            if( opt_debug )
                printf( "opt_busy_sec = %.6f msec\n", opt_busy_sec * 1E3 );
            break ;
        case 'l' : /* Loops */
            opt_loops = strtol(optarg, NULL, 0);
            if( opt_debug )
                printf( "opt_loops = %d\n", opt_loops );
            break ;
        case 'T' : /* Task */
            printf( "%s: Task selection not implemented yet\n", ARGV0 );
            break ;
        case 'R' : /* Report */
            if( 0 == strcmp( "Drop", optarg ) ) {
                opt_task->ReportTime = DropReport ;
            } else if( 0 == strcmp( "Print", optarg ) ) {
                opt_task->ReportTime = PrintReport ;
            } else {
                printf( "%s: Unrecognized Report: %s\n", ARGV0, optarg );
                retval = 1 ;
            };
            break ;
        case 'W' : /* Wait */
            if( 0 == strcmp( "Static", optarg ) ) {
                opt_task->Wait2Start = StaticWait ;
            } else if( 0 == strcmp( "MPI", optarg ) ) {
                opt_task->Wait2Start = MPIWait ;
            } else if( 0 == strcmp( "No", optarg ) ) {
                opt_task->Wait2Start = NoWait ;
            } else {
                printf( "%s: Unrecognized Wait: %s\n", ARGV0, optarg );
                retval = 1 ;
            };
            break ;
        case 'd' : /* Debug */
            ++opt_debug ;
            break ;
        case '?' : /* Unlisted option */
            if( opterr == 0 )
                fprintf( stderr, "%s: Unrecognized option: %c\n", ARGV0, optopt );
            retval = 1 ;
        case 'u' : /* Usage */
        case 'h' : /* Help */
            opt_usage = 1 ;
            break ;
        default :
            printf( "%s: Internal error. Option %c not handled\n", ARGV0, option );
            retval = 1 ;
            break ;
        };
    }; /* While */

    return retval ;
}

int usage( int rc ) {
    printf( "usage: %s -u -h -d -A init -T task -R report -W wait -l len -i idle -b busy\n", ARGV0 );
    fputs(  "where:\n"
            "   -u|-h  prints this usage message\n"
            "   -d  increments the debug level\n"
            "   init  is the value passed to the Init funciton (array length)\n"
            "   task  is the name of the Task function (not implemented)\n"
            "   report  is the name of the Report function (Drop, Print)\n"
            "   wait  is the name of the Wait function (No, Static, MPI)\n"
            "   len   length of array (elements)\n"
            "   idle  is the idle time in seconds\n"
            "   busy  is the busy time in seconds\n",
            stdout );

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

    minDelta = timesfound[1]-timesfound[0];
    for (i = 2; i < M; i++) {
        Delta = timesfound[i]-timesfound[i-1];
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

    printf( "minDelta: %20.9f usec\n", checktick() * 1E6 );

    printf( "OMP_NUM_THREADS: %s\n", getenv("OMP_NUM_THREADS") );

#ifdef MPI
    StartMPI( argc, argv );
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
