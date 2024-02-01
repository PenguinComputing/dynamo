/* Copyright (c) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 */

extern int myMPI_RANK ;

extern long NoWait( double actual );
extern long StaticWait( double actual );
extern long MPIWait( double actual );

#ifdef MPI
extern void StartMPI( int argc, char ** argv );
extern void StopMPI( );
#endif

extern void DropReport( double target, long work, double actual );
extern void PrintReport( double target, long work, double actual );

extern long UpdateWork( double actual );
