/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_support_acceptance/test_main.c
 *
 * PURPOSE:
 *   Run focused language/toolchain support-matrix acceptance tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>

/*
 * Exercise test language support all tools and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_language_support_all_tools(void);
/*
 * Exercise test language support no tools and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_language_support_no_tools(void);
/*
 * Exercise test language support windows c and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_language_support_windows_c(void);
/*
 * Exercise test language support generic c and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_language_support_generic_c(void);
/*
 * Exercise test language support assembly x86 and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_language_support_assembly_x86(void);
/*
 * Exercise test language support python and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_language_support_python(void);
/*
 * Exercise test language support typescript and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_language_support_typescript(void);
/*
 * Exercise test language support document and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_language_support_document(void);
/*
 * Exercise test language support find missing and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_language_support_find_missing(void);
/*
 * Exercise test language support custom missing binding and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_support_custom_missing_binding(void);
/*
 * Exercise test language support partial tools and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_language_support_partial_tools(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_all_tools() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_no_tools() != 0) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_windows_c() != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_generic_c() != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_assembly_x86() != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_python() != 0) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_typescript() != 0) return 7;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_document() != 0) return 8;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_find_missing() != 0) return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_custom_missing_binding() != 0) return 10;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_language_support_partial_tools() != 0) return 11;
    puts("language support acceptance tests passed");
    return 0;
}
