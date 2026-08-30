/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/paging_record.c
 *
 * PURPOSE:
 *   Implement bounded mutation, validation and hashing for the provider paging record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/paging_record.h"

#include <string.h>

void umi_workbench_selection_provider_paging_record_init(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    const char *record_id)
{
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->provider_kind = UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC;
    record->state = UMI_WORKBENCH_SELECTION_PROVIDER_CREATED;
    record->selection_kind = UMI_WORKBENCH_SELECTION_GENERIC;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->revision = 1U;
    if (record_id != NULL) {
        (void)umi_workbench_selection_provider_copy_text(
            record->record_id,
            sizeof(record->record_id),
            record_id);
    }
}

UmiStatus umi_workbench_selection_provider_paging_record_validate(
    const UmiWorkbenchSelectionProviderPagingRecord *record)
{
    if (record == NULL ||
        record->structure_size != sizeof(*record) ||
        record->record_id[0] == '\0' ||
        record->provider_kind < UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC ||
        record->provider_kind > UMI_WORKBENCH_SELECTION_PROVIDER_MEDIA ||
        record->state < UMI_WORKBENCH_SELECTION_PROVIDER_CREATED ||
        record->state > UMI_WORKBENCH_SELECTION_PROVIDER_STOPPED ||
        record->selection_kind < UMI_WORKBENCH_SELECTION_GENERIC ||
        record->selection_kind > UMI_WORKBENCH_SELECTION_MEDIA ||
        record->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        record->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

#define UMI_PROVIDER_SETTER(fn, field) \
UmiStatus fn(UmiWorkbenchSelectionProviderPagingRecord *record, const char *value) \
{ \
    UmiStatus status; \
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    status = umi_workbench_selection_provider_copy_text( \
        record->field, sizeof(record->field), value); \
    if (status == UMI_STATUS_OK) ++record->revision; \
    return status; \
}

UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_paging_record_set_provider,
    provider_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_paging_record_set_source,
    source_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_paging_record_set_subject,
    subject_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_paging_record_set_related,
    related_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_paging_record_set_group,
    group_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_paging_record_set_description,
    description)

#undef UMI_PROVIDER_SETTER

uint64_t umi_workbench_selection_provider_paging_record_hash(
    const UmiWorkbenchSelectionProviderPagingRecord *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    if (record == NULL) return 0U;
    hash = umi_workbench_selection_provider_hash_text(
        hash, record->record_id, sizeof(record->record_id));
    hash = umi_workbench_selection_provider_hash_text(
        hash, record->provider_id, sizeof(record->provider_id));
    hash = umi_workbench_selection_provider_hash_text(
        hash, record->source_id, sizeof(record->source_id));
    hash = umi_workbench_selection_provider_hash_text(
        hash, record->subject_id, sizeof(record->subject_id));
    hash = umi_workbench_selection_provider_hash_text(
        hash, record->related_id, sizeof(record->related_id));
    hash = umi_workbench_selection_provider_hash_text(
        hash, record->group_id, sizeof(record->group_id));
    hash = umi_workbench_selection_provider_hash_text(
        hash, record->description, sizeof(record->description));
    hash ^= (uint64_t)record->provider_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->selection_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->context_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= record->flags;
    hash *= UINT64_C(1099511628211);
    hash ^= record->count;
    hash *= UINT64_C(1099511628211);
    return hash;
}

void umi_workbench_selection_provider_paging_record_touch(
    UmiWorkbenchSelectionProviderPagingRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}
