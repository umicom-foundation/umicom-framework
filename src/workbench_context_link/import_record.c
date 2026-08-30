/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/import_record.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the context-link import record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/import_record.h"

#include <string.h>

void umi_workbench_context_link_import_record_init(UmiWorkbenchContextLinkImportRecord *record,
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
            record->record_id, sizeof(record->record_id), identity);
    }
}

UmiStatus umi_workbench_context_link_import_record_validate(
    const UmiWorkbenchContextLinkImportRecord *record)
{
    if (record == NULL || record->structure_size != sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_link_text_is_valid(
            record->record_id, sizeof(record->record_id)) ||
        record->record_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_link_text_is_valid(
            record->source_uri, sizeof(record->source_uri)) ||
        !umi_workbench_context_link_text_is_valid(
            record->workspace_id, sizeof(record->workspace_id))) {
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

UmiStatus umi_workbench_context_link_import_record_copy(
    UmiWorkbenchContextLinkImportRecord *destination,
    const UmiWorkbenchContextLinkImportRecord *source)
{
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_context_link_import_record_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_context_link_import_record_hash(
    const UmiWorkbenchContextLinkImportRecord *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->record_id, sizeof(record->record_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->source_uri, sizeof(record->source_uri));
    hash = umi_workbench_context_link_hash_text(
        hash, record->workspace_id, sizeof(record->workspace_id));
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

UmiStatus umi_workbench_context_link_import_record_set_primary(
    UmiWorkbenchContextLinkImportRecord *record,
    const char *value)
{
    UmiStatus status;
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->source_uri, sizeof(record->source_uri), value);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

UmiStatus umi_workbench_context_link_import_record_set_secondary(
    UmiWorkbenchContextLinkImportRecord *record,
    const char *value)
{
    UmiStatus status;
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->workspace_id, sizeof(record->workspace_id), value);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

void umi_workbench_context_link_import_record_touch(
    UmiWorkbenchContextLinkImportRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
