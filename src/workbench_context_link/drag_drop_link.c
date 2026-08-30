/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/drag_drop_link.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the drag-and-drop context-link operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/drag_drop_link.h"

#include <string.h>

void umi_workbench_context_link_drag_drop_link_init(UmiWorkbenchContextLinkDragDropLink *record,
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
            record->operation_id, sizeof(record->operation_id), identity);
    }
}

UmiStatus umi_workbench_context_link_drag_drop_link_validate(
    const UmiWorkbenchContextLinkDragDropLink *record)
{
    if (record == NULL || record->structure_size != sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_link_text_is_valid(
            record->operation_id, sizeof(record->operation_id)) ||
        record->operation_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_link_text_is_valid(
            record->source_panel_id, sizeof(record->source_panel_id)) ||
        !umi_workbench_context_link_text_is_valid(
            record->target_panel_id, sizeof(record->target_panel_id))) {
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

UmiStatus umi_workbench_context_link_drag_drop_link_copy(
    UmiWorkbenchContextLinkDragDropLink *destination,
    const UmiWorkbenchContextLinkDragDropLink *source)
{
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_context_link_drag_drop_link_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_context_link_drag_drop_link_hash(
    const UmiWorkbenchContextLinkDragDropLink *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->operation_id, sizeof(record->operation_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->source_panel_id, sizeof(record->source_panel_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->target_panel_id, sizeof(record->target_panel_id));
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

UmiStatus umi_workbench_context_link_drag_drop_link_set_primary(
    UmiWorkbenchContextLinkDragDropLink *record,
    const char *value)
{
    UmiStatus status;
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->source_panel_id, sizeof(record->source_panel_id), value);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

UmiStatus umi_workbench_context_link_drag_drop_link_set_secondary(
    UmiWorkbenchContextLinkDragDropLink *record,
    const char *value)
{
    UmiStatus status;
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->target_panel_id, sizeof(record->target_panel_id), value);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

void umi_workbench_context_link_drag_drop_link_touch(
    UmiWorkbenchContextLinkDragDropLink *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
