/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_main.c
 *
 * PURPOSE:
 *   Execute focused application-experience contract tests.
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
 * Exercise test experience catalogue and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_experience_catalogue(void);
/*
 * Exercise test experience registry and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_experience_registry(void);
/*
 * Exercise test experience status and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_experience_status(void);
/*
 * Exercise test experience priority and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_experience_priority(void);
/*
 * Exercise test experience profiles and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_experience_profiles(void);
/*
 * Exercise test experience portfolio alignment and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_experience_portfolio_alignment(void);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_experience_catalogue() != 0) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_experience_registry() != 0) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (test_experience_status() != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_experience_priority() != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_experience_profiles() != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (test_experience_portfolio_alignment() != 0) return 6;
    puts("application experience tests passed");
    return 0;
}
