/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/definition.c
 *
 * PURPOSE:
 *   Implement live source definition configuration, validation and sample acceptance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/definition.h"

#include <string.h>

void umi_workbench_context_source_definition_init(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *source_id)
{
    if (definition == NULL) return;
    memset(definition, 0, sizeof(*definition));
    definition->structure_size = (uint32_t)sizeof(*definition);
    definition->source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC;
    definition->trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT;
    definition->context_kind = UMI_CONTEXT_KIND_SELECTION;
    definition->coalescing_mode =
        UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_SUBJECT;
    definition->coalescing_window_ms = 40U;
    definition->minimum_interval_ms = 0U;
    definition->accepted_kinds_mask =
        UMI_WORKBENCH_CONTEXT_SOURCE_ALL_KINDS_MASK;
    definition->enabled = true;
    definition->revision = 1U;
    if (source_id != NULL) {
        (void)umi_workbench_context_source_copy_text(
            definition->source_id,
            sizeof(definition->source_id),
            source_id);
    }
}

UmiStatus umi_workbench_context_source_definition_set_identity(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *application_id,
    const char *panel_id,
    const char *display_name)
{
    UmiStatus status;
    if (definition == NULL || application_id == NULL ||
        panel_id == NULL || display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_copy_text(
        definition->application_id,
        sizeof(definition->application_id),
        application_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_copy_text(
        definition->panel_id,
        sizeof(definition->panel_id),
        panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_source_copy_text(
        definition->display_name,
        sizeof(definition->display_name),
        display_name);
    if (status == UMI_STATUS_OK) ++definition->revision;
    return status;
}

UmiStatus umi_workbench_context_source_definition_set_group(
    UmiWorkbenchContextSourceDefinition *definition,
    const char *group_id)
{
    UmiStatus status;
    if (definition == NULL || group_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_copy_text(
        definition->preferred_group_id,
        sizeof(definition->preferred_group_id),
        group_id);
    if (status == UMI_STATUS_OK) ++definition->revision;
    return status;
}

UmiStatus umi_workbench_context_source_definition_validate(
    const UmiWorkbenchContextSourceDefinition *definition)
{
    if (definition == NULL ||
        definition->structure_size != sizeof(*definition) ||
        definition->source_id[0] == '\0' ||
        definition->application_id[0] == '\0' ||
        definition->panel_id[0] == '\0' ||
        definition->source_kind < UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC ||
        definition->source_kind > UMI_WORKBENCH_CONTEXT_SOURCE_MEDIA ||
        definition->trigger < UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_ACTIVATE ||
        definition->trigger > UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_NAVIGATE ||
        definition->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        definition->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

bool umi_workbench_context_source_definition_accepts(
    const UmiWorkbenchContextSourceDefinition *definition,
    const UmiWorkbenchContextSourceSample *sample)
{
    uint64_t bit;
    if (definition == NULL || sample == NULL || !definition->enabled ||
        sample->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        sample->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return false;
    }
    if (strcmp(definition->source_id, sample->source_id) != 0 ||
        strcmp(definition->application_id, sample->application_id) != 0 ||
        strcmp(definition->panel_id, sample->panel_id) != 0) {
        return false;
    }
    bit = UINT64_C(1) << ((unsigned)sample->context_kind - 1U);
    return (definition->accepted_kinds_mask & bit) != 0U;
}
