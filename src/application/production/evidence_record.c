/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/evidence_record.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/evidence_record.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Copy application production evidence record into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_application_production_evidence_record_set(
    UmiApplicationProductionEvidenceRecord *record,
    const char *evidence_id, UmiApplicationProductionEvidenceKind kind,
    UmiApplicationProductionEvidenceState state, const char *reference,
    uint64_t revision)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || revision == 0U ||
        kind < UMI_APPLICATION_PRODUCTION_EVIDENCE_MANIFEST ||
        kind > UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTANCE ||
        state < UMI_APPLICATION_PRODUCTION_EVIDENCE_MISSING ||
        state > UMI_APPLICATION_PRODUCTION_EVIDENCE_REJECTED)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(record, 0, sizeof(*record));
    status = copy_text(record->evidence_id, sizeof(record->evidence_id),
                       evidence_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(record->reference, sizeof(record->reference),
                       reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    record->kind = kind;
    record->state = state;
    record->revision = revision;
    return umi_application_production_evidence_record_validate(record);
}

/*
 * Check that application production evidence record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_production_evidence_record_validate(
    const UmiApplicationProductionEvidenceRecord *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->evidence_id, '\0', sizeof(record->evidence_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->reference, '\0', sizeof(record->reference)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL || record->evidence_id[0] == '\0' ||
        record->reference[0] == '\0' || record->revision == 0U ||
        record->kind < UMI_APPLICATION_PRODUCTION_EVIDENCE_MANIFEST ||
        record->kind > UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTANCE ||
        record->state < UMI_APPLICATION_PRODUCTION_EVIDENCE_MISSING ||
        record->state > UMI_APPLICATION_PRODUCTION_EVIDENCE_REJECTED)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}


/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiApplicationProductionEvidenceRecordArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfcb6fa477197b7f3);
    schema = (schema ^ (uint64_t)sizeof(((UmiApplicationProductionEvidenceRecord *)0)->evidence_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiApplicationProductionEvidenceRecord *)0)->reference)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiApplicationProductionEvidenceRecordArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiApplicationProductionEvidenceRecord *)0)->evidence_id) - 1U +
        8U + sizeof(((UmiApplicationProductionEvidenceRecord *)0)->reference) - 1U +
        8U +
        8U +
        8U;
}
static void UmiApplicationProductionEvidenceRecordArchiveWrite(UmiArchiveWriter *writer, const UmiApplicationProductionEvidenceRecord *value)
{
    UmiArchiveWriteText(writer, value->evidence_id, sizeof(value->evidence_id));
    UmiArchiveWriteText(writer, value->reference, sizeof(value->reference));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiApplicationProductionEvidenceRecordArchiveRead(UmiArchiveReader *reader, UmiApplicationProductionEvidenceRecord *value)
{
    UmiArchiveReadText(reader, value->evidence_id, sizeof(value->evidence_id));
    UmiArchiveReadText(reader, value->reference, sizeof(value->reference));
    value->kind = (UmiApplicationProductionEvidenceKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->state = (UmiApplicationProductionEvidenceState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiApplicationProductionEvidenceRecordArchiveValidate(const UmiApplicationProductionEvidenceRecord *value)
{
    return umi_application_production_evidence_record_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_application_production_evidence_record_archive_encode, umi_application_production_evidence_record_archive_decode,
    UmiApplicationProductionEvidenceRecord, UmiApplicationProductionEvidenceRecordArchiveSchema, UmiApplicationProductionEvidenceRecordArchiveBound, UmiApplicationProductionEvidenceRecordArchiveWrite, UmiApplicationProductionEvidenceRecordArchiveRead, UmiApplicationProductionEvidenceRecordArchiveValidate)
