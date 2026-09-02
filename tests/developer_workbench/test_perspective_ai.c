/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_perspective_ai.c
 *
 * PURPOSE:
 *   Verify the built-in Ai perspective contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/perspectives/ai.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective =
        umi_developer_workbench_perspective_ai();

    assert(perspective != NULL);
    assert(perspective->surface_count == 4U);
    assert(umi_developer_workbench_perspective_validate(perspective) ==
           UMI_STATUS_OK);
    return 0;
}
