/*
Copyright 2026 Henryk S. Holtman - LeaoMartelo (github.com/LeaoMartelo2)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LOGNEST_H_
#define LOGNEST_H_

#define LOGNEST_VERSION_MAJOR 3
#define LOGNEST_VERSION_MINOR 0
#define LOGNEST_VERSION_PATCH 0

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef LOGNEST_TRACE_PREFIX
    #define LOGNEST_TRACE_PREFIX "[LOG]  "
#endif

#ifndef LOGNEST_WARN_PREFIX
    #define LOGNEST_WARN_PREFIX  "[WARN] "
#endif

#ifndef LOGNEST_ERROR_PREFIX
    #define LOGNEST_ERROR_PREFIX "[ERROR]"
#endif

#ifndef LOGNEST_DEBUG_PREFIX
    #define LOGNEST_DEBUG_PREFIX "[DEBUG]"
#endif

#ifndef LOGNEST_API
    #define LOGNEST_API
#endif


#define LOGNEST_TS_BUFFER_SZ 1024

typedef enum {
    LN_TRACE,
    LN_WARN,
    LN_ERROR,
    LN_DEBUG,

} LOGNEST_LOGTYPES;


bool lognest_init(const char *filename);
void lognest_shutdown(void);

LOGNEST_API void lognest(LOGNEST_LOGTYPES level, const char *fmt, ...);

#define lognest_trace(...) lognest(LN_TRACE, __VA_ARGS__)
#define lognest_warn(...) lognest(LN_WARN, __VA_ARGS__)
#define lognest_error(...) lognest(LN_ERROR, __VA_ARGS__)
#define lognest_debug(...) lognest(LN_DEBUG, __VA_ARGS__)

#ifdef __cplusplus
}
#endif 

#endif /* LOGNEST_H_ */

#ifdef LOGNEST_IMPLEMENTATION


static FILE *lognest__file = NULL;

LOGNEST_API bool lognest_init(const char *filename) {

    if(lognest__file != NULL) return false;

    lognest__file = fopen(filename ? filename : "latest.log", "a");

    if(lognest__file == NULL) {
        fprintf(stderr, "[LogNest]: failed to open logfile.\n");
        return false;
    }

    return true;
}

LOGNEST_API void lognest_shutdown(void) {
    if(lognest__file) {
        fclose(lognest__file);
        lognest__file = NULL;
    }
}

static void lognest__get_timestamp(char *buffer, size_t len) {
    if(buffer == NULL || len < 12) return;

    time_t now = time(NULL);

    struct tm *t = localtime(&now);

    if(t) strftime(buffer, len, "[%H:%M:%S]", t);
}

static void lognest__get_datestamp(char *buffer, size_t len) {
    if(buffer == NULL || len < 12) return;

    time_t now = time(NULL);

    struct tm *t = localtime(&now);

    if(t) strftime(buffer, len, "[%y/%m/%d]", t);
}


static void lognest__to_file(const char *level, const char *fmt, va_list args) {

    FILE *out = lognest__file ? lognest__file : stdout;


    char timestamp[32] = {};
    char datestamp[32] = {};


#ifndef LOGNEST_DISABLE_TIMESTAMP
    lognest__get_timestamp(timestamp, sizeof(timestamp));
#endif /* LOGNEST_DISABLE_TIMESTAMP */


#ifndef LOGNEST_DISABLE_DATESTAMP
    lognest__get_datestamp(datestamp, sizeof(datestamp));
#endif /* LOGNEST_DISABLE_DATESTAMP */
    
    fprintf(out, "%s%s%s: ", datestamp, timestamp, level);
    vfprintf(out, fmt, args);
    fprintf(out, "\n");

    if(lognest__file) fflush(lognest__file);
}

static const char *lognest__level_to_cstr(LOGNEST_LOGTYPES level) {
    switch(level) {
        case LN_TRACE:
            return LOGNEST_TRACE_PREFIX;
        break;
        case LN_WARN:
            return LOGNEST_WARN_PREFIX;
        break;
        case LN_ERROR:
            return LOGNEST_ERROR_PREFIX;
        break;
        case LN_DEBUG:
            return LOGNEST_DEBUG_PREFIX;
        break;
        default:
            return NULL;
    }

    return NULL;
}

LOGNEST_API void lognest(LOGNEST_LOGTYPES level, const char *fmt, ...) {

    
#ifdef LOGNEST_DISABLE_TRACE
    if(level == LN_TRACE) return;
#endif

#ifdef LOGNEST_DISABLE_WARN
    if(level == LN_WARN) return;
#endif

#ifdef LOGNEST_DISBLE_ERROR
    if(level == LN_ERROR) return;
#endif

#ifdef LOGNEST_DISABLE_DEBUG
    if(level == LN_DEBUG) return;
#endif

    va_list args; va_start(args, fmt);
    lognest__to_file(lognest__level_to_cstr(level), fmt, args);
    va_end(args);
}

#endif /* LOGNEST_IMPLEMENTATION */
