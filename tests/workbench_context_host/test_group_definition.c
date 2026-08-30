/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_group_definition.c
 *
 * PURPOSE:
 *   Verify group-definition initialization and validation.
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
#include "test_support.h"

int main(void)
{

    UmiWorkbenchContextHostGroupDefinition group;
    umi_workbench_context_host_group_definition_init(&group, "blue");
    assert(umi_workbench_context_host_copy_text(
        group.title, sizeof(group.title), "Development") == UMI_STATUS_OK);
    group.colour = UMI_CONTEXT_COLOUR_BLUE;
    group.allowed_kinds_mask =
        umi_workbench_context_host_kind_mask(UMI_CONTEXT_KIND_PROJECT);
    group.default_active = true;
    assert(umi_workbench_context_host_group_definition_validate(
        &group) == UMI_STATUS_OK);
    group.group_id[0] = '\0';
    assert(umi_workbench_context_host_group_definition_validate(
        &group) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
