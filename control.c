/* Copyright (c) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "global.h"
extern double mysecond( );

#ifdef MPI
#include "mpi.h"
#endif

/* Forward Definition */
extern long UpdateWork( double actual );

long NoWait( double actual )
{
    return UpdateWork( actual );
}

long StaticWait( double actual )
{
    usleep( opt_idle_sec * 1E6 );
    return UpdateWork( actual );
}

/* Always define myMPI_RANK even when MPI is not selected */
int  myMPI_RANK = 0 ;

#ifdef MPI
static int master_sleep = 0 ;
static int numtasks = 1 ;
static double *actuals ;

void StartMPI( int argc, char ** argv )
{
   int  len, rc;
   char hostname[MPI_MAX_PROCESSOR_NAME];

   // Initialize MPI
   MPI_Init( &argc, &argv );

   // Get number of tasks
   MPI_Comm_size( MPI_COMM_WORLD, &numtasks );

   // Get my rank
   MPI_Comm_rank( MPI_COMM_WORLD, &myMPI_RANK );

   // Get processor type
   MPI_Get_processor_name(hostname, &len);
   printf ("Number of tasks= %d My rank= %d Running on %s\n", numtasks,myMPI_RANK,hostname);

   // Default starting wait time
   master_sleep = opt_idle_sec * 1E6 ;

   // Input array for task actuals reporting
   actuals = calloc( numtasks, sizeof(double) );
}

long MPIWait( double actual )
{
    double  before, after ;
    static int  msg_cnt = 0 ;
    long  work ;
    MPI_Status  status ;

    if( myMPI_RANK == 0 ) {  /* Are we the master rank? */
        int  node ;
        double  min, max, total ;

        // Get actuals from other nodes
        min = max = total = actual ;
        actuals[0] = actual ;
        for( node = 1 ; node < numtasks ; ++node ) {
            double nodeact ;
            MPI_Recv( &nodeact, 1, MPI_DOUBLE, node, 0, MPI_COMM_WORLD, &status );
#if 0
            if( opt_debug ) {
                fprintf( stderr, "Received %10.6f msec from %d\n", nodeact, node );
            };
#endif

            if( nodeact < min ) { min = nodeact ; };
            if( nodeact > max ) { max = nodeact ; };
            total += nodeact ;
            actuals[node] = nodeact ;
        };

        // Start timing idle period
        before = mysecond( );

        // Wait before executing Barrier
        if( master_sleep < 0 )  master_sleep = 0 ;
        usleep( master_sleep );

        // Wake other tasks
        MPI_Barrier( MPI_COMM_WORLD );

        // Inform tasks of next work value
        work = UpdateWork( max );
        MPI_Bcast( &work, 1, MPI_LONG, 0, MPI_COMM_WORLD );

        // End of Idle period
        after = mysecond( );

        // Update idle sleep time
        if( (after - before) > opt_idle_sec ) {
            --master_sleep ;
        } else {
            ++master_sleep ;
        }
#if 1
        if( ++msg_cnt > 10 ) {
            printf( "Actuals: Min: %10.6f msec, Avg: %10.6f msec, Max: %10.6f msec  -- New Work: %ld\n", min*1.0e3, 1.0e3 * total/numtasks , max*1.0e3, work );
            printf( "Timing sleep: %d usec, actual idle: %.2f usec\n", master_sleep, (after-before)*1.0e6 );
            msg_cnt = 0 ;
        }
#endif
    } else {
        MPI_Send( &actual, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD );

        MPI_Barrier( MPI_COMM_WORLD );

        MPI_Bcast( &work, 1, MPI_LONG, 0, MPI_COMM_WORLD );
#if 0
        if( opt_debug ) {
            fprintf( stderr, "Task %d received %ld work\n", myMPI_RANK, work );
        };
#endif
    }

    return work ;
}

void StopMPI( )
{
   // Done with MPI
   MPI_Finalize( );
}
#else
long MPIWait( double actual )
{
    static int warn_once = 0 ;
    if( !warn_once ) {
        fprintf( stderr, "%s: Compiled without MPI support, doing StaticWait\n", ARGV0 );
        warn_once = 1;
    }
    return StaticWait( actual );
}
#endif


/*** Reporting Functions ***/
void DropReport( double target, long work, double actual )
{
    return ;
}

void PrintReport( double target, long work, double actual )
{
    if( myMPI_RANK != 0 )  /* Are we the master rank? */
        return ;

    printf( "[%12.9f] Target: %10.6f msec, Work: %12ld  Actual: %10.6f msec\n",
            mysecond(), target * 1.0E3, work, actual * 1.0E3 );
}

/*** Target to Work value process ***/
static long lastwork = 10000 ;
long UpdateWork( double actual )
{
    if( actual < 0 ) {
        return lastwork ;
    };

    if( actual < opt_busy_sec/4.0 ) {
        lastwork *= 4.0 ;
    } else if( actual < opt_busy_sec/2.0 ) {
        lastwork *= 2.0 ;
    } else if( actual > opt_busy_sec*4.0 ) {
        lastwork /= 4.0 ;
    } else if( actual > opt_busy_sec*2.0 ) {
        lastwork /= 2.0 ;
    } else {
        long prevwork = lastwork ;
        lastwork *= sqrt(opt_busy_sec/actual) ;
        printf( "opt_busy_sec: %10.6f  actual: %10.6f  prev: %ld  curr: %ld  ratio: %10.6f\n", opt_busy_sec, actual, prevwork, lastwork, sqrt(1.0*lastwork/prevwork) );
    };

    return lastwork ;
}
