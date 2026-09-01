/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/check.h
 *
 * PURPOSE:
 *   Give small native tests a diagnostic check that reports the failed source
 *   expression instead of terminating through an opaque runtime fast-fail.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CHECK_H
#define UMICOM_TEST_RUNTIME_CHECK_H

#include <stdio.h>
#include <stdlib.h>

/* Print one stable diagnostic that CTest can display with --output-on-failure. */
static inline int umi_test_runtime_report_failed_check(
    const char *file,
    int line,
    const char *expression)
{
    (void)fprintf(stderr, "%s:%d: test check failed: %s\n",
                  file, line, expression);
    return EXIT_FAILURE;
}

/* Evaluate an expression once. A false result returns from an int test entry
 * point with a normal failure code, making the actual broken contract visible.
 * This macro is intended for test functions that return int, especially main. */
#define UMI_TEST_REQUIRE(expression)                                         \
    do {                                                                     \
        if (!(expression)) {                                                 \
            return umi_test_runtime_report_failed_check(                    \
                __FILE__, __LINE__, #expression);                            \
        }                                                                    \
    } while (0)

#endif
