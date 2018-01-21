/* Copyright (c) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 */

extern double NoWait( );
extern double StaticWait( );

#ifdef MPI
extern void StartMPI( int argc, char ** argv );
extern void StopMPI( );
#endif
extern double MPIWait( );

extern void DropReport( double target, long work, double actual );
extern void PrintReport( double target, long work, double actual );
