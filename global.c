/* Copyright (c) 2017,2018 -- Penguin Computing
 * All rights reserved
 *
 * Declare global variables default values and initializers
 */

#include <stddef.h>
#include "global.h"
#include "control.h"
#include "worker.h"
#include "streamtask.h"

/* Command line command name argv[0] */
char * ARGV0 = "ARGV0" ;

int opt_usage = 0 ;
int opt_debug = 0 ;

double opt_idle_sec = 0.000500 ;
double opt_busy_sec = 0.000500 ;

int opt_loops = 100 ;

size_t opt_init_size = 10000000 ;

static struct task t = {
        .Wait2Start = StaticWait,
        .ReportTime = DropReport,
        .Desc = "Stream w/Triad",
        .Init = StreamInit,
        .Task = StreamTaskTriad
    };
struct task *opt_task = &t ;
