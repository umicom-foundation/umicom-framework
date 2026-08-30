/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_types.c
 *
 * PURPOSE:
 *   Verify structured selection enum text, bounded copy and hashing helpers.
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

#include "umicom/workbench_selection/types.h"

int main(void)
{
    char text[16U];
    assert(umi_workbench_selection_copy_text(
        text, sizeof(text), "project") == UMI_STATUS_OK);
    assert(strcmp(text, "project") == 0);
    assert(umi_workbench_selection_copy_text(
        text, 4U, "project") == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(strcmp(umi_workbench_selection_kind_text(
        UMI_WORKBENCH_SELECTION_DIAGNOSTIC), "diagnostic") == 0);
    assert(strcmp(umi_workbench_selection_activation_text(
        UMI_WORKBENCH_SELECTION_ACTIVATION_NAVIGATE), "navigate") == 0);
    return 0;
}
