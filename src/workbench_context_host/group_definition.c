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

void umi_workbench_context_host_group_definition_init(
    UmiWorkbenchContextHostGroupDefinition *definition,
    const char *group_id)
{
    if (definition == NULL) return;
    memset(definition, 0, sizeof(*definition));
    definition->structure_size = (uint32_t)sizeof(*definition);
    definition->colour = UMI_CONTEXT_COLOUR_NONE;
    definition->allowed_kinds_mask = UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK;
    definition->default_mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    definition->revision = 1U;
    if (group_id != NULL) {
        (void)umi_workbench_context_host_copy_text(
            definition->group_id, sizeof(definition->group_id), group_id);
    }
}

UmiStatus umi_workbench_context_host_group_definition_validate(
    const UmiWorkbenchContextHostGroupDefinition *definition)
{
    if (definition == NULL || definition->structure_size != sizeof(*definition)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_host_text_is_valid(
            definition->group_id, sizeof(definition->group_id)) ||
        definition->group_id[0] == '\0' ||
        !umi_workbench_context_host_text_is_valid(
            definition->title, sizeof(definition->title))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (definition->colour < UMI_CONTEXT_COLOUR_NONE ||
        definition->colour > UMI_CONTEXT_COLOUR_MAGENTA) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (definition->default_mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        definition->default_mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
