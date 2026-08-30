/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/workspace_binding.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the workspace context binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/workspace_binding.h"

#include <string.h>

void umi_workbench_context_link_workspace_binding_init(UmiWorkbenchContextLinkWorkspaceBinding *record,
                                           const char *identity)
{
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->context_kind = UMI_CONTEXT_KIND_GENERIC;
    record->colour = UMI_CONTEXT_COLOUR_NONE;
    record->mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE;
    record->state = UMI_WORKBENCH_CONTEXT_LINK_STATE_DETACHED;
    record->origin = UMI_WORKBENCH_CONTEXT_LINK_ORIGIN_USER;
    record->priority = UMI_WORKBENCH_CONTEXT_LINK_PRIORITY_NORMAL;
    record->revision = 1U;
    if (identity != NULL) {
        (void)umi_workbench_context_link_copy_text(
            record->binding_id, sizeof(record->binding_id), identity);
    }
}

UmiStatus umi_workbench_context_link_workspace_binding_validate(
    const UmiWorkbenchContextLinkWorkspaceBinding *record)
{
    if (record == NULL || record->structure_size != sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_link_text_is_valid(
            record->binding_id, sizeof(record->binding_id)) ||
        record->binding_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_link_text_is_valid(
            record->workspace_id, sizeof(record->workspace_id)) ||
        !umi_workbench_context_link_text_is_valid(
            record->group_id, sizeof(record->group_id))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (record->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        record->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (record->colour < UMI_CONTEXT_COLOUR_NONE ||
        record->colour > UMI_CONTEXT_COLOUR_MAGENTA) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (record->mode < UMI_WORKBENCH_CONTEXT_LINK_MODE_NONE ||
        record->mode > UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_link_workspace_binding_copy(
    UmiWorkbenchContextLinkWorkspaceBinding *destination,
    const UmiWorkbenchContextLinkWorkspaceBinding *source)
{
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_context_link_workspace_binding_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_context_link_workspace_binding_hash(
    const UmiWorkbenchContextLinkWorkspaceBinding *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->binding_id, sizeof(record->binding_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->workspace_id, sizeof(record->workspace_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->group_id, sizeof(record->group_id));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->context_kind, sizeof(record->context_kind));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->colour, sizeof(record->colour));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->mode, sizeof(record->mode));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->state, sizeof(record->state));
    hash = umi_workbench_context_link_hash_bytes(
        hash, &record->flags, sizeof(record->flags));
    return hash;
}

UmiStatus umi_workbench_context_link_workspace_binding_set_primary(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    const char *value)
{
    UmiStatus status;
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->workspace_id, sizeof(record->workspace_id), value);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

UmiStatus umi_workbench_context_link_workspace_binding_set_secondary(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    const char *value)
{
    UmiStatus status;
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->group_id, sizeof(record->group_id), value);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

void umi_workbench_context_link_workspace_binding_touch(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
