/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_perspective_testing.c
 *
 * PURPOSE:
 *   Verify the built-in Testing perspective contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/perspectives/testing.h"

int main(void)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective =
        umi_developer_workbench_perspective_testing();

    assert(perspective != NULL);
    assert(perspective->surface_count == 4U);
    assert(umi_developer_workbench_perspective_validate(perspective) ==
           UMI_STATUS_OK);
    return 0;
}
