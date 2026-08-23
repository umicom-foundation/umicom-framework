/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_workbench_problem_navigation_contract.c
 *
 * PURPOSE:
 *   Regression-test the declaration closure required by workbench_bridge.c for
 *   next/previous Problems navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/workbench_bridge.h"

typedef UmiStatus (*ProblemNavigationFunction)(
    const UmiDeveloperProblemStore *,
    size_t,
    UmiDeveloperProductivitySeverity,
    size_t *,
    UmiDeveloperProblem *);

int main(void)
{
    ProblemNavigationFunction next_problem = umi_developer_problem_next;
    ProblemNavigationFunction previous_problem = umi_developer_problem_previous;

    assert(next_problem != NULL);
    assert(previous_problem != NULL);
    return 0;
}
