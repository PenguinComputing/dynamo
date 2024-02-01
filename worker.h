/* Copyright (C) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 */

#include <stdlib.h>

struct task {
    long (*Wait2Start)( double );
    void (*ReportTime)( double target, long work, double actual );
    char *Desc ;
    int (*Init)( size_t size );
    void (*Task)( long work );
};


int worker( struct task *task );
