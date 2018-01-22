/* Copyright (c) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 */

#include <stdio.h>
#include <unistd.h>
#include "global.h"
extern double mysecond( );

#ifdef MPI
#include "mpi.h"
#endif

double NoWait( )
{
    return 1E-3 ;  /* 1msec */
}

double StaticWait( )
{
    usleep( opt_idle_sec * 1E6 );
    return opt_busy_sec ;
}

/* Completion function for StaticWait and NoWait */
void NoComplete( )
{
    return ;
}

#ifdef MPI
int  myMPI_RANK ;
void StartMPI( int argc, char ** argv )
{
   int  numtasks, len, rc; 
   char hostname[MPI_MAX_PROCESSOR_NAME];

   // initialize MPI  
   MPI_Init( &argc, &argv );

   // get number of tasks 
   MPI_Comm_size( MPI_COMM_WORLD, &numtasks );

   // get my rank  
   MPI_Comm_rank( MPI_COMM_WORLD, &myMPI_RANK );

   // this one is obvious  
   MPI_Get_processor_name(hostname, &len);
   printf ("Number of tasks= %d My rank= %d Running on %s\n", numtasks,myMPI_RANK,hostname);

}

double MPIWait( )
{
    static int master_sleep = 0 ;
    double before, after ;

    if( myMPI_RANK == 0 ) {  /* Are we the master rank? */
        if( master_sleep <= 0 )  master_sleep = opt_idle_sec * 1E6 ;
        before = mysecond( );
        usleep( master_sleep );
        MPI_Barrier( MPI_COMM_WORLD );
        after = mysecond( );
        if( (after - before) > opt_idle_sec ) {
            --master_sleep ;
        } else {
            ++master_sleep ;
        }
#if 1
        printf( "Timing sleep: %d usec\n", master_sleep );
#endif
    } else {
        MPI_Barrier( MPI_COMM_WORLD );
    }

    return opt_busy_sec ;
}

void StopMPI( )
{
   // done with MPI  
   MPI_Finalize( );
}
#else
double MPIWait( )
{
    fprintf( stderr, "%s: Compiled without MPI support, doing StaticWait\n", ARGV0 );
    return StaticWait( );
}
#endif


/*** Reporting Functions ***/
void DropReport( double target, long work, double actual )
{
    return ;
}

void PrintReport( double target, long work, double actual )
{
    printf( "[%12.9f] Target: %10.6f msec, Work: %12ld  Actual: %10.6f msec\n",
            mysecond(), target * 1.0E3, work, actual * 1.0E3 );
}
