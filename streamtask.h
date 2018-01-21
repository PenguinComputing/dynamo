/* Copyright (C) 2017,2018 -- Penguin Computing
 *
 * All rights reserved
 *
 * Stream based tasks
 */

/* Stream based Task methods */
int StreamInit( size_t array_size );
void StreamSetScalar( double scalar );
void StreamTaskSet( long work );
void StreamTaskCopy( long work );
void StreamTaskScale( long work );
void StreamTaskAdd( long work );
void StreamTaskTriad( long work );
