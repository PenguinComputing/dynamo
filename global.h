/* Command line command name argv[0] */
char * ARGV0 ;

/* Global state/option variables */
int opt_usage ;
int opt_debug ;

/* Useful macros */
# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif
# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif
