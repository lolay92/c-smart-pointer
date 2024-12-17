#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(Msg, ...)
#else 
#define debug(Msg, ...) fprintf(stderr, "[DEBUG] %s:%d " Msg "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(Msg, ...) fprintf(stderr, "[ERROR] %s:%d:errno: %s " Msg "\n", \
            __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(Msg, ...) fprintf(stderr, "[WARNING] %s:%d:errno: %s " Msg "\n", \
            __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(Msg, ...) fprintf(stderr, "[INFO] %s:%d:errno: %s " Msg "\n", \
            __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define check(A, Msg, ...) if(!A) { log_err(Msg, ##__VA_ARGS__); errno=0; \
        goto error;}

#define check_mem(A) check(A, "Out of memory!")

#endif