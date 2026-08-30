/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_perspective_delivery.c
 *
 * PURPOSE:
 *   Verify the built-in Delivery perspective contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/perspectives/delivery.h"

int main(void)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective =
        umi_developer_workbench_perspective_delivery();

    assert(perspective != NULL);
    assert(perspective->surface_count == 4U);
    assert(umi_developer_workbench_perspective_validate(perspective) ==
           UMI_STATUS_OK);
    return 0;
}
