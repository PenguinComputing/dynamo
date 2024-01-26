/* Copyright (c) 2024  Penguin Solutions an SGH company
 *
 * All rights reserved
 */

#define USE_64  /* or USE_32_SCSI, or USE_32_ETHERNET */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "memorytask.h"
#include "global.h"


/* Reference POLY values from:
 *   https://users.ece.cmu.edu/~koopman/crc/index.html
 */

#ifdef USE_32_SCSI
/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78
typedef uint32_t word_t ;
#endif

#ifdef USE_32_ETHERNET
/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
#define POLY 0xedb88320
typedef uint32_t word_t ;
#endif

#ifdef USE_64
/* CRC-64 */
#define POLY 0x95ac9329ac4bc9b5
// #define POLY 0xea00000000000000
// #define POLY 0xc96c5795d7870f42
// #define POLY 0x8f00000000000000
typedef uint64_t word_t ;
#endif


/* From the web.  How to calculate a CRC bitwise (without a table) */
/* NOTE: the crc update equation is written to avoid a test/branch
 *    using "0-(x&1)" to map 0 -> all 1s and 1 -> all 0s
 */
/*
word_t crc32c(word_t crc, const uint_8 *buf, size_t len)
{
    int k;

    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = (crc >> 1) ^ (POLY & (0 - (crc & 1))); 
    }
    return ~crc;
}
*/

static word_t fillbuf(word_t seed, word_t *buf, size_t len)
{
    word_t  lfsr ;
    lfsr = ~seed ;
    while ( len -- ) {
        /* lfsr ^= 0 */
        *buf++ = lfsr ;
        lfsr = (lfsr >> 1) ^ (POLY & (0 - (lfsr & 1)));
    }
    return ~lfsr ;
}

static int updatebuf(word_t seed, word_t *buf, size_t len)
{
    word_t  lfsr ;
    int  check = 0;

    lfsr = ~seed ;
    while ( len -- ) {
        check += (lfsr ^ *buf) != 0 ;  /* Count non-zero */
        lfsr = (lfsr >> 1) ^ (POLY & (0 - (lfsr & 1)));
        *buf++ = lfsr ;
    };
    return check ;
}

static int fprintbuf(FILE *fp, word_t *buf, size_t len)
{
    size_t  i ;

    if ( len > 256 ) { len = 256; };
    for ( i = 0 ; i < len ; ++i ) {
        if ( i%8 == 0 ) { fprintf(fp, "%2lx:", i); };
        fprintf(fp, " %16lx", (uint64_t)*buf++);
        if ( i%8 == 7 ) { fputs("\n", fp); };
    };
    if ( i%8 != 0 ) { fputs("\n", fp); };

    return 0;
}

static void fprinttestbuf(FILE *fp, word_t *buf, size_t len)
{
    int one[sizeof(word_t)*8] ;
    int zero[sizeof(word_t)*8] ;
    int b, i;
    for( b = 0 ; b < sizeof(word_t)*8 ; ++b ) { one[b]=0 ; zero[b]=0; };
    for( i = 0 ; i < len ; ++i ) {
        word_t v = buf[i] ;
        for( b = 0 ; b < sizeof(word_t)*8 ; ++b ) {
            if( v & 1 ) { ++one[b]; } else { ++zero[b]; }
            v = v >>1 ;
        }
    }
    fprintf(fp, "bit  one   zero\n" );
    for( b = 0 ; b < sizeof(word_t)*8 ; ++b ) {
        fprintf(fp, "%3d  %5d %5d %5d\n", b, one[b], zero[b], one[b]-zero[b] );  
    }
}

/* Work arrays */
static word_t *buffer ;
static size_t buflen ;
static word_t seed = 0 ;
static word_t scratch ;
static size_t lastlen ;

int MemoryInit( size_t array_size )
{
    int retval = 0 ;

    buffer = calloc( array_size, sizeof(word_t) );
    if( NULL != buffer ) {
        buflen = array_size ;
        fillbuf(seed, buffer, buflen);
        lastlen = buflen ;
        retval = 1 ;
    }
    return retval ;
}

void MemoryTaskUpdate( long work )
{
    int  result = 0 ;

    if( opt_debug ) {
        fprintf( stderr, "Seed: %16lx  Work: %ld  buflen: %ld  lastlen: %ld\n", seed, work, buflen, lastlen );
        fprintbuf( stderr, buffer, 8 );
    };

    while( work > 0 ) {
        long limit ;
        if( work <= lastlen ) {
            limit = work ;
        } else {
            if( work > buflen ) {
                if( work > 2*buflen ) {
                    if( work > 3*buflen ) {
                        limit = buflen ;
                    } else {
                        limit = ((double) work + 2.0) / 3.0 ;
                    };
                } else {
                    limit = ((double) work + 1.0) / 2.0 ;
                };
            } else {
                limit = work ;
            };
        };
        if( limit <= lastlen ) {
            if( opt_debug ) { fputs( "u", stderr ); };
            result += updatebuf(seed, buffer, limit);
            seed = fillbuf(seed, &scratch, 1);
        } else {
            if( opt_debug ) { fputs( "f", stderr ); };
            seed = fillbuf(seed, &scratch, 1);
            fillbuf(seed, buffer, limit);
        }; 
        lastlen = limit ;
        work -= limit ;
    };
    if( opt_debug ) { fputs("\n", stderr); };

    if( result ) {
        fputs( "Update detected memory error\n", stderr );
    }
}
