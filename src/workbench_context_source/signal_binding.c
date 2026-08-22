/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/signal_binding.c
 *
 * PURPOSE:
 *   Implement validation, bounded mutation and stable hashing for the native signal binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/signal_binding.h"

#include <string.h>

void umi_workbench_context_source_signal_binding_init(
    UmiWorkbenchContextSourceSignalBinding *record,
    const char *record_id)
{
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC;
    record->trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT;
    record->state = UMI_WORKBENCH_CONTEXT_SOURCE_STATE_CREATED;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->revision = 1U;
    if (record_id != NULL) {
        (void)umi_workbench_context_source_copy_text(
            record->record_id, sizeof(record->record_id), record_id);
    }
}

UmiStatus umi_workbench_context_source_signal_binding_validate(
    const UmiWorkbenchContextSourceSignalBinding *record)
{
    if (record == NULL || record->structure_size != sizeof(*record) ||
        record->record_id[0] == '\0' ||
        record->source_kind < UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC ||
        record->source_kind > UMI_WORKBENCH_CONTEXT_SOURCE_MEDIA ||
        record->trigger < UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_ACTIVATE ||
        record->trigger > UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_NAVIGATE ||
        record->state < UMI_WORKBENCH_CONTEXT_SOURCE_STATE_CREATED ||
        record->state > UMI_WORKBENCH_CONTEXT_SOURCE_STATE_STOPPED ||
        record->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        record->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

#define UMI_CONTEXT_SOURCE_SETTER(function_name, field_name) \
UmiStatus function_name(UmiWorkbenchContextSourceSignalBinding *record, const char *value) \
{ \
    UmiStatus status; \
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    status = umi_workbench_context_source_copy_text( \
        record->field_name, sizeof(record->field_name), value); \
    if (status == UMI_STATUS_OK) ++record->revision; \
    return status; \
}

UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_signal_binding_set_source, source_id)
UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_signal_binding_set_panel, panel_id)
UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_signal_binding_set_subject, subject_id)
UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_signal_binding_set_group, group_id)
UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_signal_binding_set_label, label)

#undef UMI_CONTEXT_SOURCE_SETTER

uint64_t umi_workbench_context_source_signal_binding_hash(
    const UmiWorkbenchContextSourceSignalBinding *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    if (record == NULL) return 0U;
    hash = umi_workbench_context_source_hash_text(
        hash, record->record_id, sizeof(record->record_id));
    hash = umi_workbench_context_source_hash_text(
        hash, record->source_id, sizeof(record->source_id));
    hash = umi_workbench_context_source_hash_text(
        hash, record->panel_id, sizeof(record->panel_id));
    hash = umi_workbench_context_source_hash_text(
        hash, record->subject_id, sizeof(record->subject_id));
    hash = umi_workbench_context_source_hash_text(
        hash, record->group_id, sizeof(record->group_id));
    hash = umi_workbench_context_source_hash_text(
        hash, record->label, sizeof(record->label));
    hash ^= (uint64_t)record->source_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->trigger;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->context_kind;
    hash *= UINT64_C(1099511628211);
    return hash;
}

void umi_workbench_context_source_signal_binding_touch(
    UmiWorkbenchContextSourceSignalBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
