/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_problem_bridge.c
 *
 * PURPOSE:
 *   Integration regression coverage for problem bridge.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ide_integration/problem_bridge.h"
int main(void)
{
    UmiDeveloperProblemStore *store = NULL;
    UmiDeveloperProblem problem = {0};
    UmiIdeNavigationTarget target;
    assert(umi_developer_problem_store_create(&store) == UMI_STATUS_OK);
    problem.problem_id = 1U;
    (void)strcpy(problem.source, "compiler");
    (void)strcpy(problem.message, "bad thing");
    (void)strcpy(problem.location.uri, "file:///tmp/a.c");
    problem.location.line = 7U;
    problem.severity = UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR;
    assert(umi_developer_problem_store_add(store, &problem) == UMI_STATUS_OK);
    assert(umi_ide_problem_target(store, 0U, &target) == UMI_STATUS_OK);
    assert(target.location.line == 7U);
    assert(target.reason == UMI_IDE_NAVIGATION_PROBLEM);
    umi_developer_problem_store_destroy(store);
    return 0;
}

