/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_workbench_problem_navigation_contract.c
 *
 * PURPOSE:
 *   Regression-test the declaration closure required by workbench_bridge.c for
 *   next/previous Problems navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/workbench_bridge.h"

typedef UmiStatus (*ProblemNavigationFunction)(
    const UmiDeveloperProblemStore *,
    size_t,
    UmiDeveloperProductivitySeverity,
    size_t *,
    UmiDeveloperProblem *);

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    ProblemNavigationFunction next_problem = umi_developer_problem_next;
    ProblemNavigationFunction previous_problem = umi_developer_problem_previous;

    assert(next_problem != NULL);
    assert(previous_problem != NULL);
    return 0;
}
