/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_search_types.c
 *
 * PURPOSE:
 *   Verify deterministic Search Everywhere scoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/search_types.h"

int main(void)
{
    const int32_t prefix = umi_developer_workbench_search_score(
        "build", "Build Project", "build.build");
    const int32_t middle = umi_developer_workbench_search_score(
        "project", "Build Project", "build.build");
    const int32_t missing = umi_developer_workbench_search_score(
        "xyz", "Build Project", "build.build");

    assert(prefix > middle);
    assert(middle >= 0);
    assert(missing < 0);
    return 0;
}
