/* Copyright (C) 2017  Penguin Computing
 *
 * All rights reserved
 *
 * Some types of tasks to burn CPU cycles, power and time
 */

#include <stdlib.h>

static double * a ;
static double * b ;
static double * c ;
static double   s ;
static size_t   len ;

int AllocArray( size_t array_size )
{

    len = array_size ;

    a = calloc( array_size, sizeof(double) );
    b = calloc( array_size, sizeof(double) );
    c = calloc( array_size, sizeof(double) );

    return( NULL != a && NULL != b && NULL != c );
}

void SetScalar( double scalar )
{
    s = scalar ;
}
 
void TaskSet( long work )
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

void TaskCopy( long work )
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

void TaskScale( long work )
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
            b[j] = s*c[j];

        work -= limit ;
    }
}

void TaskAdd( long work )
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

void TaskTriad( long work )
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
            a[j] = b[j]+s*c[j];

        work -= limit ;
    }
}
