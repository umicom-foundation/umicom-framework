/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cmake/test_build_contract/test_check_header.c
 *
 * PURPOSE:
 *   Exercise the real diagnostic-check header through target-owned include
 *   requirements, including release builds and single-evaluation behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_runtime/check.h"

#include <string.h>

/* These tests must not disappear when NDEBUG disables ordinary assertions. */
static int check_single_evaluation(void)
{
    int evaluations = 0;
    UMI_TEST_REQUIRE(++evaluations == 1);
    UMI_TEST_REQUIRE(evaluations == 1);
    UMI_TEST_REQUIRE(1);
    return EXIT_SUCCESS;
}

/* The macro must remain one statement when used by an unbraced conditional. */
static int check_conditional_statement(void)
{
    int branch = 0;
    if (branch == 0)
        UMI_TEST_REQUIRE(branch == 0);
    else
        branch = 1;
    UMI_TEST_REQUIRE(branch == 0);
    return EXIT_SUCCESS;
}

/* A separate process checks the failing diagnostic without aborting a suite. */
int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--fail") == 0) {
        UMI_TEST_REQUIRE(0);
        (void)fputs("UNREACHABLE_AFTER_FAILED_CHECK\n", stderr);
        return EXIT_SUCCESS;
    }
    if (argc != 1) {
        (void)fputs("Usage: test-check-header [--fail]\n", stderr);
        return EXIT_FAILURE;
    }
    UMI_TEST_REQUIRE(check_single_evaluation() == EXIT_SUCCESS);
    UMI_TEST_REQUIRE(check_conditional_statement() == EXIT_SUCCESS);
    (void)puts("Framework diagnostic header contract passed.");
    return EXIT_SUCCESS;
}
