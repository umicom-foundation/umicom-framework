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

int test_language_support_all_tools(void);
int test_language_support_no_tools(void);
int test_language_support_windows_c(void);
int test_language_support_generic_c(void);
int test_language_support_assembly_x86(void);
int test_language_support_python(void);
int test_language_support_typescript(void);
int test_language_support_document(void);
int test_language_support_find_missing(void);
int test_language_support_custom_missing_binding(void);
int test_language_support_partial_tools(void);

int main(void)
{
    if (test_language_support_all_tools() != 0) return 1;
    if (test_language_support_no_tools() != 0) return 2;
    if (test_language_support_windows_c() != 0) return 3;
    if (test_language_support_generic_c() != 0) return 4;
    if (test_language_support_assembly_x86() != 0) return 5;
    if (test_language_support_python() != 0) return 6;
    if (test_language_support_typescript() != 0) return 7;
    if (test_language_support_document() != 0) return 8;
    if (test_language_support_find_missing() != 0) return 9;
    if (test_language_support_custom_missing_binding() != 0) return 10;
    if (test_language_support_partial_tools() != 0) return 11;
    puts("language support acceptance tests passed");
    return 0;
}
