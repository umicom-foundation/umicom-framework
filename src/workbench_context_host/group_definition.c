/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/group_definition.c
 *
 * PURPOSE:
 *   Implement product-composition group-definition initialisation and validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/group_definition.h"

#include <string.h>

/*
 * Initialise workbench context host group definition from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_group_definition_init(
    UmiWorkbenchContextHostGroupDefinition *definition,
    const char *group_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL) return;
    memset(definition, 0, sizeof(*definition));
    definition->structure_size = (uint32_t)sizeof(*definition);
    definition->colour = UMI_CONTEXT_COLOUR_NONE;
    definition->allowed_kinds_mask = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    definition->default_mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    definition->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (group_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            definition->group_id, sizeof(definition->group_id), group_id);
    }
}

/*
 * Check that workbench context host group definition satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_host_group_definition_validate(
    const UmiWorkbenchContextHostGroupDefinition *definition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL || definition->structure_size != sizeof(*definition)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_workbench_context_host_text_is_valid(
            definition->group_id, sizeof(definition->group_id)) ||
        definition->group_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            definition->title, sizeof(definition->title))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->colour < UMI_CONTEXT_COLOUR_NONE ||
        definition->colour > UMI_CONTEXT_COLOUR_MAGENTA) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (definition->default_mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        definition->default_mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
