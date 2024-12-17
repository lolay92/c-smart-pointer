#include "debug.h"
#undef NDEBUG
#ifndef _UNITTEST_H
#define _UNITTEST_H

#define test_suite_start() char* message=NULL;
#define unittest_assert(test_result, message) if(!(test_result)){\
    log_err(message); return message; }

#define unittest_run(test_) debug("----- RUNNING: %s ------\n", #test_); \
    message=test_(); if(message) { return message; }

#endif

// #define RUN_TESTS(tests) int main(int argc, char* argv[]) {\
//     char* result=tests(); if(result) {\
//     debug("FAILED: ‰s\n", result); \
//     printf("FAILED: ‰s\n", result); } \
//     else { debug("ALL TESTS HAVE PASSED!"); \
//     printf("ALL TESTS HAVE PASS@@@ED!"); } \
//     return result!=0; }