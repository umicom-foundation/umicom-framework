/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_source_control_operation_unstage.c
 *
 * PURPOSE:
 *   Verify the built-in Unstage source-control operation definition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/developer_productivity/source_control_operations/unstage.h"

int main(void)
{
    assert(umi_developer_source_control_operation_validate(
        umi_developer_source_control_operation_unstage()) == UMI_STATUS_OK);
    return 0;
}
