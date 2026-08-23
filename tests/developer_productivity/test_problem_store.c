/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_problem_store.c
 *
 * PURPOSE:
 *   Verify normalized Problem statistics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/developer_productivity/problem_store.h"

int main(void)
{
    UmiDeveloperProblemStore *store = NULL;
    UmiDeveloperProblem problem;
    UmiDeveloperProblemStoreSnapshot snapshot;

    assert(umi_developer_problem_store_create(&store) == UMI_STATUS_OK);
    (void)memset(&problem, 0, sizeof(problem));
    (void)strcpy(problem.source, "gcc");
    (void)strcpy(problem.message, "error");
    problem.severity = UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR;

    assert(umi_developer_problem_store_add(
        store, &problem) == UMI_STATUS_OK);
    snapshot = umi_developer_problem_store_snapshot(store);
    assert(snapshot.count == 1U);
    assert(snapshot.errors == 1U);

    umi_developer_problem_store_destroy(store);
    return 0;
}
