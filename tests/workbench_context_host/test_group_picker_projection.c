/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_group_picker_projection.c
 *
 * PURPOSE:
 *   Verify group rows, active selection and member counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/workbench_context_host/group_picker_projection.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostGroupPickerProjection projection;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_group_picker_projection_build(
        fixture.host, &projection) == UMI_STATUS_OK);
    assert(projection.count == 2U);
    assert(projection.rows[0].active);
    assert(strcmp(projection.rows[0].group_id, "blue") == 0);
    assert(projection.rows[0].member_count == 2U);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
