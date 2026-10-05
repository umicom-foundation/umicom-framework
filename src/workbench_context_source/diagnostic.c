/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/diagnostic.c
 *
 * PURPOSE:
 *   Implement validation, bounded mutation and stable hashing for the source diagnostic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/diagnostic.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise workbench context source diagnostic from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_diagnostic_init(
    UmiWorkbenchContextSourceDiagnostic *record,
    const char *record_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return;
    memset(record, 0, sizeof(*record));
    record->structure_size = (uint32_t)sizeof(*record);
    record->source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC;
    record->trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT;
    record->state = UMI_WORKBENCH_CONTEXT_SOURCE_STATE_CREATED;
    record->context_kind = UMI_CONTEXT_KIND_SELECTION;
    record->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record_id != NULL) {
        (void)umi_workbench_context_source_copy_text(
            record->record_id, sizeof(record->record_id), record_id);
    }
}

/*
 * Check that workbench context source diagnostic satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_source_diagnostic_validate(
    const UmiWorkbenchContextSourceDiagnostic *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->record_id, '\0', sizeof(record->record_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_id, '\0', sizeof(record->source_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->panel_id, '\0', sizeof(record->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subject_id, '\0', sizeof(record->subject_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->group_id, '\0', sizeof(record->group_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->label, '\0', sizeof(record->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
UmiStatus function_name(UmiWorkbenchContextSourceDiagnostic *record, const char *value) \
{ \
    UmiStatus status; \
    if (record == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    status = umi_workbench_context_source_copy_text( \
        record->field_name, sizeof(record->field_name), value); \
    if (status == UMI_STATUS_OK) ++record->revision; \
    return status; \
}

UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_diagnostic_set_source, source_id)
UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_diagnostic_set_panel, panel_id)
UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_diagnostic_set_subject, subject_id)
UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_diagnostic_set_group, group_id)
UMI_CONTEXT_SOURCE_SETTER(
    umi_workbench_context_source_diagnostic_set_label, label)

#undef UMI_CONTEXT_SOURCE_SETTER

/*
 * Provide the workbench context source diagnostic hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_source_diagnostic_hash(
    const UmiWorkbenchContextSourceDiagnostic *record)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the workbench context source diagnostic touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_source_diagnostic_touch(
    UmiWorkbenchContextSourceDiagnostic *record,
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
static uint64_t UmiWorkbenchContextSourceDiagnosticArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc871a8e309d12a06);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->record_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->source_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->subject_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->group_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiWorkbenchContextSourceDiagnosticArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->record_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->source_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->panel_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->subject_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->group_id) - 1U +
        8U + sizeof(((UmiWorkbenchContextSourceDiagnostic *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiWorkbenchContextSourceDiagnosticArchiveWrite(UmiArchiveWriter *writer, const UmiWorkbenchContextSourceDiagnostic *value)
{
    UmiArchiveWriteText(writer, value->record_id, sizeof(value->record_id));
    UmiArchiveWriteText(writer, value->source_id, sizeof(value->source_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteText(writer, value->group_id, sizeof(value->group_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteSigned(writer, (int64_t)value->source_kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->trigger);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteSigned(writer, (int64_t)value->context_kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiWorkbenchContextSourceDiagnosticArchiveRead(UmiArchiveReader *reader, UmiWorkbenchContextSourceDiagnostic *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->record_id, sizeof(value->record_id));
    UmiArchiveReadText(reader, value->source_id, sizeof(value->source_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    UmiArchiveReadText(reader, value->group_id, sizeof(value->group_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->source_kind = (UmiWorkbenchContextSourceKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->trigger = (UmiWorkbenchContextSourceTrigger)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiWorkbenchContextSourceState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->context_kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiWorkbenchContextSourceDiagnosticArchiveValidate(const UmiWorkbenchContextSourceDiagnostic *value)
{
    return umi_workbench_context_source_diagnostic_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_workbench_context_source_diagnostic_archive_encode, umi_workbench_context_source_diagnostic_archive_decode,
    UmiWorkbenchContextSourceDiagnostic, UmiWorkbenchContextSourceDiagnosticArchiveSchema, UmiWorkbenchContextSourceDiagnosticArchiveBound, UmiWorkbenchContextSourceDiagnosticArchiveWrite, UmiWorkbenchContextSourceDiagnosticArchiveRead, UmiWorkbenchContextSourceDiagnosticArchiveValidate)
