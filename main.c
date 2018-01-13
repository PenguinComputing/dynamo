/* Copyright (c) 2017, Penguin Computing
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

double NoWait( )
{
/*
    sleep( 1 );
*/
    return 1E-3 ;  /* 1msec */
}

double Freq1kDuty50( )
{
    usleep( 500 );
    return 500E-6 ;
}

double Freq500Duty66( )
{
    usleep( 500 );
    return 1E-3 ;
}

void DropReport( double time )
{
    return ;
}

void PrintReport( double time )
{
    printf( "[%12.9f] Took: %10.6f msec\n", mysecond(), time * 1.0E3 );
}

int main( int argc, char ** argv ) {

    struct task  mytask ;

    /* Parse arguments, print usage if parse fails or usage requested */
    if( parse_args( argc, argv ) || opt_usage ) {
        usage( !opt_usage );
    }

    printf( "timeNow:  %20.9f\n", mysecond() );

    printf( "minDelta: %20.9f micro-seconds\n", checktick() );

#define ARRAY_SIZE 1000
    printf( "AllocArray(%ld) ...\n", (long) ARRAY_SIZE );
    if( !AllocArray( ARRAY_SIZE ) ) {
        fprintf( stderr, "%s: AllocArray() failed: %s\n", ARGV0, strerror(errno) );
        return 1 ;
    }

    printf( "SetScalar(%f) ...\n", 1.0 );
    SetScalar( 1.0 );
    printf( "TaskSet(%ld) ...\n", (long) ARRAY_SIZE );
    TaskSet( ARRAY_SIZE );

    printf( "task -> Freq500Duty66, PrintReport, TaskTriad\n" );
    mytask.Wait2Start = Freq500Duty66 ;
    mytask.ReportTime = PrintReport ;
    mytask.Task = TaskTriad ;

    printf( "worker()...\n" );
    worker( &mytask );

    return( 0 );
}
