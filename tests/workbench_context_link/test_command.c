/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_command.c
 *
 * PURPOSE:
 *   Verify the context-link command record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/command.h"

int main(void)
{
    UmiWorkbenchContextLinkCommand record;
    UmiWorkbenchContextLinkCommand copy;
    uint64_t first_hash;
    umi_workbench_context_link_command_init(&record, "command-id");
    assert(umi_workbench_context_link_command_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_command_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_command_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_command_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_command_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_command_hash(&copy) == first_hash);
    umi_workbench_context_link_command_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.command_id, record.command_id) == 0);
    return 0;
}
