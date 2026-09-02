/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_perspective_design.c
 *
 * PURPOSE:
 *   Verify the built-in Design perspective contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/perspectives/design.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective =
        umi_developer_workbench_perspective_design();

    assert(perspective != NULL);
    assert(perspective->surface_count == 3U);
    assert(umi_developer_workbench_perspective_validate(perspective) ==
           UMI_STATUS_OK);
    return 0;
}
