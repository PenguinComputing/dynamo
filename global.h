/* Copyright (c) 2017,2018 -- Penguin Computing
 * All rights reserved
 *
 * Declare global variables
 */

/* Command line command name argv[0] */
extern char * ARGV0 ;

/* Global state/option variables */
extern int opt_usage ;
extern int opt_debug ;

extern double opt_idle_sec ;
extern double opt_busy_sec ;

extern int opt_loops ;

extern size_t opt_init_size ;

extern struct task *opt_task ;

/* Useful macros */
# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif
# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif
