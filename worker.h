/* Copyright (C) 2017  Penguin Computing
 *
 * All rights reserved
 */

#include <stdint.h>
#include <stdlib.h>

struct task {
    double (*Wait2Start)();
    void (*Task)( long work );
    void (*ReportTime)( double elapsed );
};


int worker( struct task *task );

/* Task methods */
int AllocArray( size_t array_size );
void SetScalar( double scalar );
void TaskSet( long work );
void TaskCopy( long work );
void TaskScale( long work );
void TaskAdd( long work );
void TaskTriad( long work );
