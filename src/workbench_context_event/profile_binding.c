/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/profile_binding.c
 *
 * PURPOSE:
 *   Implement validation, bounded mutation and hashing for the profile event binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/profile_binding.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context event profile binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_profile_binding_init(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *record_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->event_kind = UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->priority = UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_NORMAL;
    record->state = UMI_WORKBENCH_CONTEXT_EVENT_CREATED;
    record->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record_id != NULL) {
        (void)umi_workbench_context_event_copy_text(
            record->record_id, sizeof(record->record_id), record_id);
    }
}

/*
 * Check that workbench context event profile binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_profile_binding_validate(
    const UmiWorkbenchContextEventProfileBinding *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->record_id, '\0', sizeof(record->record_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_id, '\0', sizeof(record->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subject_id, '\0', sizeof(record->subject_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->group_id, '\0', sizeof(record->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->label, '\0', sizeof(record->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->structure_size != sizeof(*record) ||
        record->record_id[0] == '\0' ||
        record->event_kind <= UMI_WORKBENCH_CONTEXT_EVENT_NONE ||
        record->event_kind > UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION ||
        record->context_kind < UMI_CONTEXT_KIND_GENERIC ||
        record->context_kind > UMI_CONTEXT_KIND_SELECTION) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context event profile binding set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_profile_binding_set_source(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *source_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || source_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->source_id, sizeof(record->source_id), source_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context event profile binding set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_profile_binding_set_subject(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *subject_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || subject_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->subject_id, sizeof(record->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context event profile binding set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_profile_binding_set_group(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *group_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->group_id, sizeof(record->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context event profile binding set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_profile_binding_set_label(
    UmiWorkbenchContextEventProfileBinding *record,
    const char *label)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_event_copy_text(
        record->label, sizeof(record->label), label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++record->revision;
    return status;
}

/*
 * Provide the workbench context event profile binding hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_profile_binding_hash(
    const UmiWorkbenchContextEventProfileBinding *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0U;
    hash = umi_workbench_context_event_hash_text(
        hash, record->record_id, sizeof(record->record_id));
    hash = umi_workbench_context_event_hash_text(
        hash, record->source_id, sizeof(record->source_id));
    hash = umi_workbench_context_event_hash_text(
        hash, record->subject_id, sizeof(record->subject_id));
    hash = umi_workbench_context_event_hash_text(
        hash, record->group_id, sizeof(record->group_id));
    hash = umi_workbench_context_event_hash_text(
        hash, record->label, sizeof(record->label));
    hash ^= (uint64_t)record->event_kind;
    hash *= UINT64_C(1099511628211);
    hash ^= (uint64_t)record->context_kind;
    hash *= UINT64_C(1099511628211);
    return hash;
}

/*
 * Provide the workbench context event profile binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_profile_binding_touch(
    UmiWorkbenchContextEventProfileBinding *record,
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
static uint64_t UmiWorkbenchContextEventProfileBindingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x71c8b357e5ed1a45);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->record_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->subject_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->group_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextEventProfileBindingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->record_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->source_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->subject_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->group_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextEventProfileBinding *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchContextEventProfileBindingArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextEventProfileBinding *value)
{
    UmiArchiveWriteText(writer, value->record_id, sizeof(value->record_id));
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->event_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->context_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->priority);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchContextEventProfileBindingArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextEventProfileBinding *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->record_id, sizeof(value->record_id));
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->event_kind = (UmiWorkbenchContextEventKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->context_kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->priority = (UmiWorkbenchContextEventPriority)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiWorkbenchContextEventState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchContextEventProfileBindingArchiveValidate(const UmiWorkbenchContextEventProfileBinding *value)
{
    return umi_workbench_context_event_profile_binding_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_event_profile_binding_archive_encode, umi_workbench_context_event_profile_binding_archive_decode,
    UmiWorkbenchContextEventProfileBinding, UmiWorkbenchContextEventProfileBindingArchiveSchema, UmiWorkbenchContextEventProfileBindingArchiveBound, UmiWorkbenchContextEventProfileBindingArchiveWrite, UmiWorkbenchContextEventProfileBindingArchiveRead, UmiWorkbenchContextEventProfileBindingArchiveValidate)
