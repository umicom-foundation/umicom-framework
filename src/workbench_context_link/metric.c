/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/metric.c
 *
 * PURPOSE:
 *   Implement validation, copying, hashing and mutation for the context-link metric sample.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/metric.h"

#include <string.h>

void umi_workbench_context_link_metric_init(UmiWorkbenchContextLinkMetric *record,
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
            record->metric_id, sizeof(record->metric_id), identity);
    }
}

UmiStatus umi_workbench_context_link_metric_validate(
    const UmiWorkbenchContextLinkMetric *record)
{
    if (record == NULL || record->structure_size != sizeof(*record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_link_text_is_valid(
            record->metric_id, sizeof(record->metric_id)) ||
        record->metric_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!umi_workbench_context_link_text_is_valid(
            record->name, sizeof(record->name)) ||
        !umi_workbench_context_link_text_is_valid(
            record->unit, sizeof(record->unit))) {
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

UmiStatus umi_workbench_context_link_metric_copy(
    UmiWorkbenchContextLinkMetric *destination,
    const UmiWorkbenchContextLinkMetric *source)
{
    if (destination == NULL || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_context_link_metric_validate(source) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *destination = *source;
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_context_link_metric_hash(
    const UmiWorkbenchContextLinkMetric *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    if (record == NULL) return 0U;
    hash = umi_workbench_context_link_hash_text(
        hash, record->metric_id, sizeof(record->metric_id));
    hash = umi_workbench_context_link_hash_text(
        hash, record->name, sizeof(record->name));
    hash = umi_workbench_context_link_hash_text(
        hash, record->unit, sizeof(record->unit));
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

UmiStatus umi_workbench_context_link_metric_set_primary(
    UmiWorkbenchContextLinkMetric *record,
    const char *value)
{
    UmiStatus status;
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->name, sizeof(record->name), value);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

UmiStatus umi_workbench_context_link_metric_set_secondary(
    UmiWorkbenchContextLinkMetric *record,
    const char *value)
{
    UmiStatus status;
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_link_copy_text(
        record->unit, sizeof(record->unit), value);
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

void umi_workbench_context_link_metric_touch(
    UmiWorkbenchContextLinkMetric *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
