/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_source_control_operation_pull.c
 *
 * PURPOSE:
 *   Verify the built-in Pull source-control operation definition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/source_control_operations/pull.h"

int main(void)
{
    assert(umi_developer_source_control_operation_validate(
        umi_developer_source_control_operation_pull()) == UMI_STATUS_OK);
    return 0;
}
