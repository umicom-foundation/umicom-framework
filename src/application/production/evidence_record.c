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

