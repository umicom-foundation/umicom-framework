/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/binding_record.c
 *
 * PURPOSE:
 *   Implement bounded mutation, validation and hashing for the selection binding record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/binding_record.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench selection binding record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_binding_record_init(
    UmiWorkbenchSelectionBindingRecord *record,
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
 * Check that workbench selection binding record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_selection_binding_record_validate(
    const UmiWorkbenchSelectionBindingRecord *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->record_id, '\0', sizeof(record->record_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_id, '\0', sizeof(record->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subject_id, '\0', sizeof(record->subject_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->secondary_id, '\0', sizeof(record->secondary_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->group_id, '\0', sizeof(record->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->label, '\0', sizeof(record->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

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

#define UMI_SELECTION_SETTER(fn, field) \
UmiStatus fn(UmiWorkbenchSelectionBindingRecord *record, const char *value) \
{ \
    UmiStatus status; \
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    status = umi_workbench_selection_copy_text( \
        record->field, sizeof(record->field), value); \
    if (status == UMI_STATUS_OK) ++record->revision; \
    return status; \
}

UMI_SELECTION_SETTER(umi_workbench_selection_binding_record_set_source, source_id)
UMI_SELECTION_SETTER(umi_workbench_selection_binding_record_set_subject, subject_id)
UMI_SELECTION_SETTER(umi_workbench_selection_binding_record_set_secondary, secondary_id)
UMI_SELECTION_SETTER(umi_workbench_selection_binding_record_set_group, group_id)
UMI_SELECTION_SETTER(umi_workbench_selection_binding_record_set_label, label)

#undef UMI_SELECTION_SETTER

/*
 * Provide the workbench selection binding record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_selection_binding_record_hash(
    const UmiWorkbenchSelectionBindingRecord *record)
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
        hash, record->secondary_id, sizeof(record->secondary_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->group_id, sizeof(record->group_id));
    hash = umi_workbench_selection_hash_text(
        hash, record->label, sizeof(record->label));
    hash ^= (uint64_t)record->selection_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->context_kind;
    hash *= UINT64_C(1099511628211);
    return hash;
}

/*
 * Provide the workbench selection binding record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_selection_binding_record_touch(
    UmiWorkbenchSelectionBindingRecord *record,
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
static uint64_t UmiWorkbenchSelectionBindingRecordArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0bb404291c0c0985);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->record_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->subject_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->secondary_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->group_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchSelectionBindingRecordArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->record_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->source_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->subject_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->secondary_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->group_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionBindingRecord *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchSelectionBindingRecordArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchSelectionBindingRecord *value)
{
    UmiArchiveWriteText(writer, value->record_id, sizeof(value->record_id));
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteText(writer, value->secondary_id, sizeof(value->secondary_id));
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->selection_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->activation);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteSigned(writer, (int64_t)value->context_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchSelectionBindingRecordArchiveRead(UmiArchiveReader *reader, UmiWorkbenchSelectionBindingRecord *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->record_id, sizeof(value->record_id));
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    UmiArchiveReadText(reader, value->secondary_id, sizeof(value->secondary_id));
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->selection_kind = (UmiWorkbenchSelectionKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->activation = (UmiWorkbenchSelectionActivation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiWorkbenchSelectionState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->context_kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchSelectionBindingRecordArchiveValidate(const UmiWorkbenchSelectionBindingRecord *value)
{
    return umi_workbench_selection_binding_record_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_selection_binding_record_archive_encode, umi_workbench_selection_binding_record_archive_decode,
    UmiWorkbenchSelectionBindingRecord, UmiWorkbenchSelectionBindingRecordArchiveSchema, UmiWorkbenchSelectionBindingRecordArchiveBound, UmiWorkbenchSelectionBindingRecordArchiveWrite, UmiWorkbenchSelectionBindingRecordArchiveRead, UmiWorkbenchSelectionBindingRecordArchiveValidate)
