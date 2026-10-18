/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/test_mapping.c
 *
 * PURPOSE:
 *   Implement bounded mutation, validation and hashing for the test model-to-selection mapping.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../base/conditional_archive_internal.h"
#include "umicom/workbench_selection_provider/test_mapping.h"
#include "../base/value_archive_internal.h"
#include "../base/record_update_internal.h"
#include "umicom/base/text.h"

#include <string.h>

/*
 * Initialise workbench selection provider test mapping from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_selection_provider_test_mapping_init(
    UmiWorkbenchSelectionProviderTestMapping *record,
    const char *record_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->provider_kind = UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC;
    record->state = UMI_WORKBENCH_SELECTION_PROVIDER_CREATED;
    record->selection_kind = UMI_WORKBENCH_SELECTION_GENERIC;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record_id != NULL) {
        (void)umi_workbench_selection_provider_copy_text(
            record->record_id,
            sizeof(record->record_id),
            record_id);
    }
}

/*
 * Check that workbench selection provider test mapping satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_test_mapping_validate(
    const UmiWorkbenchSelectionProviderTestMapping *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->record_id, '\0', sizeof(record->record_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->provider_id, '\0', sizeof(record->provider_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_id, '\0', sizeof(record->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subject_id, '\0', sizeof(record->subject_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->related_id, '\0', sizeof(record->related_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->group_id, '\0', sizeof(record->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->description, '\0', sizeof(record->description)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/* Text and its observation token are one publication. The shared Framework
 * edit checks bounds and revision exhaustion before writing, including when
 * input aliases this field. Keep the former copy/increment for review. */
#if 0
#define UMI_PROVIDER_SETTER(fn, field) \
UmiStatus fn(UmiWorkbenchSelectionProviderTestMapping *record, const char *value) \
{ \
    UmiStatus status; \
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    status = umi_workbench_selection_provider_copy_text( \
        record->field, sizeof(record->field), value); \
    if (status == UMI_STATUS_OK) ++record->revision; \
    return status; \
}
#endif
#define UMI_PROVIDER_SETTER(fn, field) \
UmiStatus fn(UmiWorkbenchSelectionProviderTestMapping *record, const char *value) \
{ \
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    return umi_text_update(record->field, sizeof record->field, value, &record->revision); \
}

UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_test_mapping_set_provider,
    provider_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_test_mapping_set_source,
    source_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_test_mapping_set_subject,
    subject_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_test_mapping_set_related,
    related_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_test_mapping_set_group,
    group_id)
UMI_PROVIDER_SETTER(
    umi_workbench_selection_provider_test_mapping_set_description,
    description)

#undef UMI_PROVIDER_SETTER

/*
 * Provide the workbench selection provider test mapping hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_selection_provider_test_mapping_hash(
    const UmiWorkbenchSelectionProviderTestMapping *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the workbench selection provider test mapping touch operation used by this
 * module and its client applications.
 */
/* Guard sequence/timestamp publication before any write so an exhausted
 * revision never wraps and makes an old review appear current. The original
 * touch operation remains below for engineering review. */
#if 0
void umi_workbench_selection_provider_test_mapping_touch(
    UmiWorkbenchSelectionProviderTestMapping *record,
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
#endif
void umi_workbench_selection_provider_test_mapping_touch(
    UmiWorkbenchSelectionProviderTestMapping *record,
    uint64_t sequence,
    uint64_t timestamp_ms)
{
    /* This legacy void API cannot report exhaustion. Preserve the whole record
     * when its token cannot advance; use reviewed replacement for a status. */
    if (record == NULL || record->revision == UINT64_MAX) return;
    record->sequence = sequence;
    record->timestamp_ms = timestamp_ms;
    ++record->revision;
}


/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiWorkbenchSelectionProviderTestMappingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x865e92f76874742d);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->record_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->provider_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->subject_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->related_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->group_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->description)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchSelectionProviderTestMappingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->record_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->provider_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->source_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->subject_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->related_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->group_id) - 1U +
        8U + sizeof(((UmiWorkbenchSelectionProviderTestMapping *)0)->description) - 1U +
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
static void UmiWorkbenchSelectionProviderTestMappingArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchSelectionProviderTestMapping *value)
{
    UmiArchiveWriteText(writer, value->record_id, sizeof(value->record_id));
    UmiArchiveWriteText(writer, value->provider_id, sizeof(value->provider_id));
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteText(writer, value->related_id, sizeof(value->related_id));
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteText(writer, value->description, sizeof(value->description));
    UmiArchiveWriteSigned(writer, (int64_t)value->provider_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteSigned(writer, (int64_t)value->selection_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->context_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchSelectionProviderTestMappingArchiveRead(UmiArchiveReader *reader, UmiWorkbenchSelectionProviderTestMapping *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->record_id, sizeof(value->record_id));
    UmiArchiveReadText(reader, value->provider_id, sizeof(value->provider_id));
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    UmiArchiveReadText(reader, value->related_id, sizeof(value->related_id));
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    UmiArchiveReadText(reader, value->description, sizeof(value->description));
    value->provider_kind = (UmiWorkbenchSelectionProviderKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiWorkbenchSelectionProviderRuntimeState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->selection_kind = (UmiWorkbenchSelectionKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->context_kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchSelectionProviderTestMappingArchiveValidate(const UmiWorkbenchSelectionProviderTestMapping *value)
{
    return umi_workbench_selection_provider_test_mapping_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_selection_provider_test_mapping_archive_encode, umi_workbench_selection_provider_test_mapping_archive_decode,
    UmiWorkbenchSelectionProviderTestMapping, UmiWorkbenchSelectionProviderTestMappingArchiveSchema, UmiWorkbenchSelectionProviderTestMappingArchiveBound, UmiWorkbenchSelectionProviderTestMappingArchiveWrite, UmiWorkbenchSelectionProviderTestMappingArchiveRead, UmiWorkbenchSelectionProviderTestMappingArchiveValidate)

/* SDK callers choose staging storage independently of their thread stack.
 * Reuse this owner\'s exact schema, reader and validation before publishing
 * a destination; decoded values remain data, never execution authority. */
UMI_DEFINE_VALUE_ARCHIVE_WORKSPACE(umi_workbench_selection_provider_test_mapping_archive_decode_with_workspace, UmiWorkbenchSelectionProviderTestMapping,
    UmiWorkbenchSelectionProviderTestMappingArchiveSchema, UmiWorkbenchSelectionProviderTestMappingArchiveBound, UmiWorkbenchSelectionProviderTestMappingArchiveRead, UmiWorkbenchSelectionProviderTestMappingArchiveValidate)

/* A delayed test mapping draft must name the revision it reviewed.
 * Domain validation remains with this value owner; the shared guard publishes
 * all fields together without emitting events or changing any external service. */
UMI_DEFINE_REVIEWED_IDENTITY_RECORD_EDIT(umi_workbench_selection_provider_test_mapping_replace_if_current,
    UmiWorkbenchSelectionProviderTestMapping, record_id, UmiWorkbenchSelectionProviderTestMappingArchiveValidate)

/* Restore this typed value only while its encoded state still matches the
 * caller's reviewed snapshot. The shared guard checks every encoded byte;
 * domain decoding and validation stay with this owner. */
UMI_DEFINE_CONDITIONAL_ARCHIVE(umi_workbench_selection_provider_test_mapping_archive_decode_if_unchanged, umi_workbench_selection_provider_test_mapping_archive_encode,
    UmiWorkbenchSelectionProviderTestMapping, UmiWorkbenchSelectionProviderTestMappingArchiveSchema, UmiWorkbenchSelectionProviderTestMappingArchiveBound, UmiWorkbenchSelectionProviderTestMappingArchiveRead, UmiWorkbenchSelectionProviderTestMappingArchiveValidate)
