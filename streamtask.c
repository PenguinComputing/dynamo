/* Copyright (C) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 *
 * Some types of tasks to burn CPU cycles, power and time
 */

#include <stdlib.h>
#include <stdio.h>
#include "streamtask.h"

static double * a ;
static double * b ;
static double * c ;
static double   s ;
static size_t   len ;

int StreamInit( size_t array_size )
{

    len = array_size ;

    a = calloc( array_size, sizeof(double) );
    b = calloc( array_size, sizeof(double) );
    c = calloc( array_size, sizeof(double) );

#if 0
    printf( "StreamSetScalar(%f) ...\n", 1.0 );
    printf( "StreamTaskSet(%ld) ...\n", (long) array_size  );
    printf( "StreamTaskCopy(%ld) ...\n", (long) array_size  );
    printf( "StreamTaskScale(%ld) ...\n", (long) array_size  );
#endif
    StreamSetScalar( 1.0 );  /* Init s */
    StreamTaskSet( array_size );  /* Init a[] */
    StreamTaskCopy( array_size );  /* Copy a[] to c[] */
    StreamTaskScale( array_size );  /* Copy c[] to b[] */

    return( NULL != a && NULL != b && NULL != c );
}

void StreamSetScalar( double scalar )
{
    s = scalar ;
}
 
void StreamTaskSet( long work )
{
    ssize_t j, limit;

    while( work > 0 ) {
        if( work > len ) {
            limit = (double) work * len / (work + len);
        } else {
            limit = work ;
        }

#pragma omp parallel for
        for (j=0; j<limit; j++)
            a[j] = s ;

        work -= limit ;
    }
}

void StreamTaskCopy( long work )
{
    ssize_t j, limit;

    while( work > 0 ) {
        if( work < 5*len ) {
            if( work > 4*len ) {
                limit = work / 5 ;
            } else if( work > 3*len ) {
                limit = work / 4 ;
            } else if( work > 2*len ) {
                limit = work / 3 ;
            } else if( work > len ) {
                limit = work / 2 ;
            } else
                limit = work ;
        } else {
            limit = len ;
        }

#pragma omp parallel for
        for (j=0; j<limit; j++)
            c[j] = a[j];

        work -= limit ;
    }
}

void StreamTaskScale( long work )
{
    ssize_t j, limit;
    register double S = s ;

    while( work > 0 ) {
        if( work > len ) {
            limit = (double) work * len / (work + len);
        } else {
            limit = work ;
        }

#pragma omp parallel for
        for (j=0; j<limit; j++)
            b[j] = S*c[j];

        work -= limit ;
    }
}

void StreamTaskAdd( long work )
{
    ssize_t j, limit;

    while( work > 0 ) {
        if( work > len ) {
            limit = (double) work * len / (work + len);
        } else {
            limit = work ;
        }

#pragma omp parallel for
        for (j=0; j<limit; j++)
            c[j] = a[j]+b[j];

        work -= limit ;
    }
}

void StreamTaskTriad( long work )
{
    ssize_t j, limit;
    register double S = s ;

    while( work > 0 ) {
        if( work > len ) {
            limit = (double) work * len / (work + len);
        } else {
            limit = work ;
        }

#pragma omp parallel for
        for (j=0; j<limit; j++)
            a[j] = b[j]+S*c[j];

        work -= limit ;
    }
}
