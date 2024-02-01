/* A gettimeofday routine to give access to the wall
   clock timer on most UNIX-like systems.

   This version defines two entry points -- with
   and without appended underscores, so it *should*
   automagically link with FORTRAN */

#include <sys/time.h>
#include <time.h>

#ifdef MYSEC_CLASSIC
double mysecond()
{
/* If for some reason, you don't have <sys/time.h> or just to
 *     refresh your memory
struct timeval { long   tv_sec;
            long        tv_usec;        };

struct timezone { int   tz_minuteswest;
             int        tz_dsttime;      };
 */

        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

#else
double mysecond()
{
        struct timespec tp;
        int retval;

        /* See manpage, but recommend CLOCK_MONOTONIC, CLOCK_MONOTONIC_COARSE
         * or CLOCK_BOOTTIME
         */
        retval = clock_gettime(CLOCK_MONOTONIC,&tp);
        return ( (double) tp.tv_sec + (double) tp.tv_nsec * 1.e-9 );
}
#endif

/*** Fortran binding ... ***/
double mysecond_() {return mysecond();}
