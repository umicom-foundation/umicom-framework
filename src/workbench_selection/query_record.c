/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/query_record.c
 *
 * PURPOSE:
 *   Implement bounded mutation, validation and hashing for the selection query record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/query_record.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench selection query record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_query_record_init(
    UmiWorkbenchSelectionQueryRecord *record,
    const char *record_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->selection_kind = UMI_WORKBENCH_SELECTION_GENERIC;
    record->activation = UMI_WORKBENCH_SELECTION_ACTIVATION_SELECT;
    record->state = UMI_WORKBENCH_SELECTION_STATE_CREATED;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record_id != NULL) {
        (void)umi_workbench_selection_copy_text(
            record->record_id, sizeof(record->record_id), record_id);
    }
}

/*
 * Check that workbench selection query record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_query_record_validate(
    const UmiWorkbenchSelectionQueryRecord *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->record_id, '\0', sizeof(record->record_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_id, '\0', sizeof(record->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subject_id, '\0', sizeof(record->subject_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->related_id, '\0', sizeof(record->related_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->group_id, '\0', sizeof(record->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->description, '\0', sizeof(record->description)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size != sizeof(*record) ||
        record->record_id[0] == '\0' ||
        record->selection_kind < UMI_WORKBENCH_SELECTION_GENERIC ||
        record->selection_kind > UMI_WORKBENCH_SELECTION_MEDIA ||
        record->activation < UMI_WORKBENCH_SELECTION_ACTIVATION_SELECT ||
        record->activation > UMI_WORKBENCH_SELECTION_ACTIVATION_PREVIEW ||
        record->state < UMI_WORKBENCH_SELECTION_STATE_CREATED ||
        record->state > UMI_WORKBENCH_SELECTION_STATE_STALE ||
        record->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        record->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

#define UMI_SELECTION_EXTRA_SETTER(fn, field) \
UmiStatus fn(UmiWorkbenchSelectionQueryRecord *record, const char *value) \
{ \
    UmiStatus status; \
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    status = umi_workbench_selection_copy_text( \
        record->field, sizeof(record->field), value); \
    if (status == UMI_STATUS_OK) ++record->revision; \
    return status; \
}

UMI_SELECTION_EXTRA_SETTER(umi_workbench_selection_query_record_set_source, source_id)
UMI_SELECTION_EXTRA_SETTER(umi_workbench_selection_query_record_set_subject, subject_id)
UMI_SELECTION_EXTRA_SETTER(umi_workbench_selection_query_record_set_related, related_id)
UMI_SELECTION_EXTRA_SETTER(umi_workbench_selection_query_record_set_group, group_id)
UMI_SELECTION_EXTRA_SETTER(umi_workbench_selection_query_record_set_description, description)

#undef UMI_SELECTION_EXTRA_SETTER

/*
 * Provide the workbench selection query record hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_query_record_hash(
    const UmiWorkbenchSelectionQueryRecord *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_selection_hash_text(
        hash, record->record_id, sizeof(record->record_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->source_id, sizeof(record->source_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->subject_id, sizeof(record->subject_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->related_id, sizeof(record->related_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->group_id, sizeof(record->group_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->description, sizeof(record->description));
    hash ^= (uint64_t)record->selection_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->context_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= record->count;
    hash *= UINT64_C(1099511628211);
    return hash;
}

/*
 * Provide the workbench selection query record touch operation used by this module and its
 * client applications.
 */
void umi_workbench_selection_query_record_touch(
    UmiWorkbenchSelectionQueryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchSelectionQueryRecordArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xde76148fcd56e503);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->record_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->subject_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->related_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->group_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->description)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchSelectionQueryRecordArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->record_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->source_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->subject_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->related_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->group_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionQueryRecord *)0)->description) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchSelectionQueryRecordArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchSelectionQueryRecord *value)
{
    UmiArchiveWriteText(writer, value->record_id, sizeof(value->record_id));
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteText(writer, value->related_id, sizeof(value->related_id));
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteText(writer, value->description, sizeof(value->description));
    UmiArchiveWriteSigned(writer, (int64_t)value->selection_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->activation);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteSigned(writer, (int64_t)value->context_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchSelectionQueryRecordArchiveRead(UmiArchiveReader *reader, UmiWorkbenchSelectionQueryRecord *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->record_id, sizeof(value->record_id));
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    UmiArchiveReadText(reader, value->related_id, sizeof(value->related_id));
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    UmiArchiveReadText(reader, value->description, sizeof(value->description));
    value->selection_kind = (UmiWorkbenchSelectionKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->activation = (UmiWorkbenchSelectionActivation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiWorkbenchSelectionState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->context_kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchSelectionQueryRecordArchiveValidate(const UmiWorkbenchSelectionQueryRecord *value)
{
    return umi_workbench_selection_query_record_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_selection_query_record_archive_encode, umi_workbench_selection_query_record_archive_decode,
    UmiWorkbenchSelectionQueryRecord, UmiWorkbenchSelectionQueryRecordArchiveSchema, UmiWorkbenchSelectionQueryRecordArchiveBound, UmiWorkbenchSelectionQueryRecordArchiveWrite, UmiWorkbenchSelectionQueryRecordArchiveRead, UmiWorkbenchSelectionQueryRecordArchiveValidate)
